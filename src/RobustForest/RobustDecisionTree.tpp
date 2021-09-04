
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>

#include <algorithm>
#include <numeric>

#include "../util.h"

static std::mutex mut;
const static auto N_T = std::thread::hardware_concurrency();

template<size_t NX, size_t NY>
Node<NY>* RobustDecisionTree<NX,NY>::private_fit(thread_pool& pool, const DF<NX>& X_train, const DF<NY>& y_train, const std::vector<size_t> rows,
    std::map<int64_t,int>& costs, ConstrVec& constraints, const Row<NY>& node_prediction, std::set<size_t> feature_blacklist, size_t depth)
{
    if (X_train.size() == 0)
        return new Node<NY>();

    DF<NX> X = DF_index<NX>(X_train, rows);
    DF<NY> y = DF_index<NY>(y_train, rows);
    Node<NY>* node = new Node<NY>(y.rows());
    node->set_prediction(node_prediction);
    Util::log("tree {}: current depth: {}", id_, depth);
    auto current_prediction = node->get_prediction();
    // not needed?
    //auto current_prediction_score = node->get_prediction_score(); 
    auto current_score = optimizer_->evaluate_split(y, node_prediction);
    node->set_loss(current_score);
    Util::log("tree {}: current node's loss: {:.5f}", id_, current_score);

    if (depth == max_depth_)
    {
        Util::log("tree {}: current depth {} is equal to maximum depth of this tree", id_, depth);
        return node;
    }

    if (rows.size() < min_instances_per_node_)
    {
        Util::log("tree {}: number of instances ended up in the current node ({}) are less than the minimum ({})",
            id_, rows.size(), min_instances_per_node_);
        return node;
    }
    // OptimTupl optimize_gain(const DF<NX>& X, const DF<N>& y, const IdxVec& rows, int n_sample_features, 
    //    Attacker<NA>& attacker, const CostVec& costs, double current_score);
    // using OptimTupl = std::tuple<double,IdxVec,IdxVec,size_t,double,double,NRow,NRow,double,CostMap,CostMap>;
    auto [best_gain, best_split_left, best_split_right, best_split_feature_id,
           best_split_feature_value, next_best_split_feature_id,
           best_pred_left, best_pred_right, best_loss,
           costs_left, costs_right, 
           constraints_left, constraints_right] = optimizer_->optimize_gain(X_train, y_train, rows, feature_blacklist, -1,
                                      *(attacker_.get()), costs, constraints, current_score, node_prediction);

    Util::log("tree {}: best_gain: {}", id_, best_gain);
    if (best_gain > EPS)
    {
        node->set_loss(best_loss);
        node->set_gain(best_gain);
        node->set_best_split_id(best_split_feature_id);
        node->set_best_split_value(best_split_feature_value);

        std::set<size_t> updated_feature_bl = feature_blacklist;
        // Update feature_blacklist
        if (affine_)
        {
            updated_feature_bl.insert(best_split_feature_id);
        }

        bool waiting_left = false;
        bool waiting_right = false;
        std::future<Node<NY>*> left_fut;
        std::future<Node<NY>*> right_fut;
        {
            std::unique_lock lock(mut);
            if (pool.get_tasks_total() <= N_T - 1)
            {
                left_fut = std::move(pool.submit(
                    [&,best_split_left,updated_feature_bl,depth]{
                        return private_fit(pool,X_train, y_train, best_split_left, costs_left, constraints_left, best_pred_left, updated_feature_bl, depth+1);
                    }));
                waiting_left = true;
            }
        }
        {
            std::unique_lock lock(mut);
            if (pool.get_tasks_total() <= N_T - 1)
            {
                right_fut = std::move(pool.submit(
                    [&,best_split_right,updated_feature_bl,depth]{
                        return private_fit(pool,X_train, y_train, best_split_right, costs_right, constraints_right, best_pred_right, updated_feature_bl, depth+1);
                    }));
                waiting_right = true;
            }
        }
        if (waiting_left && waiting_right)
        {
            node->set_left(left_fut.get());
            node->set_right(right_fut.get());
        }
        else if (waiting_left)
        {
            node->set_right(private_fit(pool,X_train, y_train, best_split_right, costs_right, constraints_right, best_pred_right, updated_feature_bl, depth+1));
            node->set_left(left_fut.get());
        }
        else if (waiting_right)
        {
            node->set_left(private_fit(pool,X_train, y_train, best_split_left, costs_left, constraints_left, best_pred_left, updated_feature_bl, depth+1));
            node->set_right(right_fut.get());
        }
        else
        {
            node->set_left(private_fit(pool,X_train, y_train, best_split_left, costs_left, constraints_left, best_pred_left, updated_feature_bl, depth+1));
            node->set_right(private_fit(pool,X_train, y_train, best_split_right, costs_right, constraints_right, best_pred_right, updated_feature_bl, depth+1));
        }
    }
    return node;
}

template<size_t NX, size_t NY>
void RobustDecisionTree<NX,NY>::fit(const DF<NX>& X_train, const DF<NY>& y_train)
{
    if (isTrained_)
    {
        Util::err("tree {} is already trained, cannot train again", id_);
        return;
    }
    std::vector<size_t> rows(X_train.rows());
    std::iota(rows.begin(), rows.end(), 0);
    // null prediction
    Row<NY> node_prediction = y_train.colwise().mean();
    std::map<int64_t,int> costs;
    for (int64_t i = 0; i < X_train.rows(); i++)
        costs[i] = 0;

    ConstrVec constraints;
    thread_pool pool(N_T);
    root_.reset(private_fit(pool, X_train, y_train, rows, costs, constraints, node_prediction, start_feature_bl_, 0));

    if (!root_->is_dummy())
    {
        isTrained_ = true;
        Util::log("Fitting Tree ID {} completed (is_trained = {})!",
            id_, isTrained_);
    }
}

template<size_t NX, size_t NY>
size_t RobustDecisionTree<NX,NY>::private_predict(const Row<NX>& instance, const Node<NY>* node) const
{
    if (node->is_leaf())
        return node->get_prediction();
    
    auto best_feature_id = node->get_best_split_id();
    auto best_feature_value = node->get_best_split_value();
    auto x_feature_value = instance(best_feature_id);

    if (x_feature_value <= best_feature_value) // go left
        return private_predict(instance, node->left());
    else // go right
        return private_predict(instance, node->right());

}

template<size_t NX, size_t NY>
Row<NY> RobustDecisionTree<NX,NY>::private_predict_proba(const Row<NX>& instance, const Node<NY>* node) const
{
    if (node->is_leaf())
        return node->get_prediction_score();
    
    auto best_feature_id = node->get_best_split_id();
    auto best_feature_value = node->get_best_split_value();
    auto x_feature_value = instance(best_feature_id);

    if (x_feature_value <= best_feature_value) // go left
        return private_predict_proba(instance, node->left());
    else // go right
        return private_predict_proba(instance, node->right());

}

template<size_t NX, size_t NY>
size_t RobustDecisionTree<NX,NY>::predict(const Row<NX>& instance) const
{
    if (isTrained_)
    {
        return private_predict(instance, root_.get());
    }
    else
    {
        Util::warn("tree {} is not trained", id_);
        return -1;
    }
}

template<size_t NX, size_t NY>
DF<NY> RobustDecisionTree<NX,NY>::predict_proba(const DF<NX>& X_test) const
{
    if (isTrained_)
    {
        const auto rows = X_test.rows();
        DF<NY> out = Eigen::ArrayXXd::Zero(rows, NY);
        for (int64_t i = 0; i < rows; i++)
        {
            out.row(i) = private_predict_proba(X_test.row(i), root_.get());
        }
        return out;
    }
    else
    {
        Util::warn("tree {} is not trained", id_);
        return Eigen::ArrayXXd::Zero(0, NY);
    }
}

template<size_t NX, size_t NY>
double RobustDecisionTree<NX,NY>::classification_error(const DF<NY>& Y_test,
    const DF<NY>& Y_pred) const
{
    if (Y_test.rows() != Y_pred.rows())
    {
        Util::warn("classification_error(): argument size mismatch");
        return -1.0;
    }
    const auto rows = Y_test.rows();
    int64_t accum = 0;
    for (int64_t i = 0; i < rows; i++)
    {
        const auto& test_row = Y_test.row(i);
        const auto& pred_row = Y_pred.row(i);
        Eigen::Index test_max_ind;
        test_row.maxCoeff(&test_max_ind);
        Eigen::Index pred_max_ind;
        pred_row.maxCoeff(&pred_max_ind);
        if (test_max_ind != pred_max_ind)
            accum++;
    }
    return static_cast<double>(accum) / static_cast<double>(rows);
}

template<size_t NX, size_t NY>
void RobustDecisionTree<NX,NY>::dump_to_disk(const std::filesystem::path& fn) const
{
    try 
    {
        std::ofstream os(fn, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(root_, id_, max_depth_, min_instances_per_node_, isTrained_, affine_);
    }
    catch (std::exception& e)
    {
        Util::err("failed to save the model: {}", e.what());
    }
}

template<size_t NX, size_t NY>
RobustDecisionTree<NX,NY> RobustDecisionTree<NX,NY>::load_from_disk(const std::filesystem::path& fn)
{
    try
    {
        std::ifstream is(fn, std::ios::binary);
        cereal::BinaryInputArchive archive(is);
        std::unique_ptr<Node<NY>> root;
        int id;
        size_t max_depth;
        size_t min_instances_per_node;
        bool isTrained;
        bool affine;
        archive(root, id, max_depth, min_instances_per_node, isTrained, affine);
        return RobustDecisionTree<NX,NY>(root, id, max_depth, min_instances_per_node, isTrained, affine);
    }
    catch (std::exception& e)
    {
        Util::die("failed to load model {}: {}", fn.c_str(), e.what());
    }
}

template<size_t NX, size_t NY>
std::string RobustDecisionTree<NX,NY>::get_model_name() const
{
    auto bl_string = [this]()->std::string{
        if (start_feature_bl_.empty())
            return "";
        std::stringstream ss;
        ss << "-BL";
        for (auto f : start_feature_bl_)
            ss << f << ",";
        auto s = ss.str();
        return s.substr(0, s.size()-1); 
    };
	std::string algo_str;
    if (!optimizer_)
        return "null-tree";
    const auto algo = optimizer_->get_algorithm();
    if (algo == TrainingAlgo::Icml2019)
		algo_str = "ICML2019";
    else if (algo == TrainingAlgo::Robust)
    	algo_str = "Robust";
    else
        algo_str = "Standard";
    if (algo == TrainingAlgo::Standard)
    	return fmt::format("{}-D{}{}.cereal", algo_str, max_depth_, bl_string());
    else
    	return fmt::format("{}-B{}-D{}{}.cereal", algo_str, attacker_->get_budget(), max_depth_, bl_string());
}

template<size_t NX, size_t NY>
void RobustDecisionTree<NX,NY>::print_test_score(const DF<NX>& X_test, const DF<NY>& Y_test, const DF<NY>& Y_train) const
{
    auto Y_pred = predict_proba(X_test);
    auto test_acc = 100.0 - 100.0 * classification_error(Y_test, Y_pred);
    auto train_dom = dominant_class<NY>(Y_train);
    auto dummy_score = 100.0 * class_proportion<NY>(Y_test, train_dom);
    fmt::print("test score: {:.2f}% (dummy classifier: {:.2f}%)\n", test_acc, dummy_score);
}

template<size_t NX>
static std::list<Row<NX>> att_recur(const Row<NX>& inst, const Attacker<NX>& attacker, 
    const std::vector<size_t>& features, int budget)
{
    std::list<Row<NX>> out;
    for (auto& f : features)
    {
        auto m_att = attacker.single_attack(inst, f, budget);
        if (m_att)
        {
            auto& [att, new_budg] = m_att.value();
            out.push_back(att);
            auto newlist = att_recur<NX>(att, attacker, features, new_budg);
            out.splice(out.end(), newlist);
        }
    }
    return out;
}

template<size_t NX, size_t NY>
double RobustDecisionTree<NX,NY>::get_attacked_score(Attacker<NX>& attacker,
    const DF<NX>& X, const DF<NY>& Y) const
{
    if (!isTrained_)
        Util::die("tree {} is not trained", id_);
    
    size_t score = 0;
    size_t total_attacks = 0;
    // (instance, true y)
    // std::vector<std::tuple<Row<NX>,size_t>> attack_vec;
    const auto& feats = attacker.target_features();
    //attack_vec.reserve(X.rows()*feats.size());
    for (int64_t i = 0; i < X.rows(); i++)
    {
        total_attacks++;
        Eigen::Index max_ind;
        Y.row(i).maxCoeff(&max_ind);
        const auto inst = X.row(i);
        const auto true_y = static_cast<size_t>(max_ind);
        const auto pred_y = predict(inst);
        if (true_y != pred_y)
        {
            score++;
        }
        auto att_list = att_recur<NX>(inst, attacker, feats, attacker.get_budget());
        for (auto& att : att_list)
        {
            total_attacks++;
            const auto pred_att = predict(att);
            if (true_y != pred_att)
            {
                score++;
            }
        }
    }
    return 100.0 - 100.0 * static_cast<double>(score) / static_cast<double>(total_attacks);
}

template<size_t NX, size_t NY>
std::map<size_t, double> RobustDecisionTree<NX,NY>::feature_importance() const
{
    if (!isTrained_)
        Util::die("tree {} is not trained", id_);

    std::map<size_t, double> fimp;
    if (root_->is_leaf())
        return fimp;
    feature_importance_(root_->left(), fimp);
    feature_importance_(root_->right(), fimp);
    return fimp;
}

template<size_t NX, size_t NY>
void RobustDecisionTree<NX,NY>::feature_importance_(const Node<NY>* node, std::map<size_t,double>& dict) const
{
    if (node->is_leaf())
        return;
    else
    {
        feature_importance_(node->left(), dict);
        feature_importance_(node->right(), dict);
        dict[node->get_best_split_id()] += node->get_gain();
    }
}