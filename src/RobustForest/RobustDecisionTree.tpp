
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>

#include <algorithm>
#include <numeric>

#include "../util.h"

template<size_t NX, size_t NY>
Node<NY>* RobustDecisionTree<NX,NY>::fit_(const DF<NX>& X_train, const DF<NY>& y_train, const IdxVec rows,
    const CostMap& costs, ConstrVec constraints, const Row<NY>& node_prediction, std::set<size_t> feature_blacklist,
    size_t depth, thread_pool& pool, const bool quiet, size_t& split_num)
{
    if (X_train.size() == 0)
        return new Node<NY>();

    split_num++;
    if (!quiet)
        fmt::print(stderr, "split {}/{} (upper bound)\r", split_num, max_splits_);

    DF<NY> y = DF_index<NY>(y_train, rows);
    Node<NY>* node = new Node<NY>(new_node);
    node->set_prediction(node_prediction);
    if (!quiet)
        Util::log<4>("tree {}: current depth: {}", id_, depth);
    auto current_prediction = node->get_prediction();
    // not needed?
    //auto current_prediction_score = node->get_prediction_score(); 
    auto current_score = optimizer_->evaluate_split(y, node_prediction);
    node->set_loss(current_score);

    if (!quiet)
        Util::log<4>("tree {}: current node's loss: {:.5f}", id_, current_score);

    if (depth == max_depth_)
    {
        if (!quiet)
            Util::log<4>("tree {}: current depth {} is equal to maximum depth of this tree", id_, depth);
        return node;
    }

    if (rows.size() < min_instances_per_node_)
    {
        if (!quiet)
            Util::log<4>("tree {}: number of instances ended up in the current node ({}) are less than the minimum ({})",
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
           constraints_left, constraints_right] = optimizer_->optimize_gain(X_train, y_train, rows, feature_blacklist,
                                      *(attacker_.get()), costs, constraints, current_score, node_prediction,
                                      bootstrap_features_, n_sample_features_, rd_, useParallel_, pool);

    if (!quiet)
        Util::log<4>("tree {}: best_gain: {}", id_, best_gain);
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

        node->set_left(fit_(X_train, y_train, best_split_left, costs_left, constraints_left, best_pred_left, updated_feature_bl, depth+1, pool, quiet, split_num));

        node->set_right(fit_(X_train, y_train, best_split_right, costs_right, constraints_right, best_pred_right, updated_feature_bl, depth+1, pool, quiet, split_num));

    }
    return node;
}

template<size_t NX, size_t NY>
void RobustDecisionTree<NX,NY>::fit(const DF<NX>& X_train, const DF<NY>& y_train, bool quiet)
{
    if (isTrained_)
    {
        Util::err("tree {} is already trained", id_);
        return;
    }

    max_splits_ = static_cast<size_t>(std::pow(2, max_depth_+1)) - 1;

    std::vector<size_t> rows;
    const size_t NR = X_train.rows();
    if (bootstrap_samples_)
    {
        auto n_sample_instances = std::min(NR, static_cast<size_t>(max_samples_ * NR));
        rows.reserve(n_sample_instances);
        if (replace_samples_)
        {
            std::uniform_int_distribution<size_t> distrib(0, NR);
            for (size_t i = 0; i < n_sample_instances; i++)
            {
                rows.push_back(distrib(rd_));
            }
            std::sort(rows.begin(), rows.end());
        } 
        else
        {
            std::vector<size_t> pop(NR);
            std::iota(pop.begin(), pop.end(), 0UL);
            std::sample(pop.begin(), pop.end(), std::back_inserter(rows), 
                n_sample_instances, rd_);
        }
        // TODO record OOB samples
    }
    else
    {
        rows.reserve(NR);
        for (size_t i = 0; i < NR; i++)
            rows.push_back(i);
    }
    if (bootstrap_features_)
    {
        n_sample_features_ = std::min(NX, static_cast<size_t>(max_features_ * NX));
    }

    // null prediction
    Row<NY> node_prediction;
    if (bootstrap_samples_)
    {
        node_prediction = DF_index<NY>(y_train, rows).colwise().mean();
    }
    else
        node_prediction = y_train.colwise().mean();

    CostMap costs;
    for (int64_t i = 0; i < rows.size(); i++)
        costs[rows.at(i)] = 0;

    thread_pool pool;
    ConstrVec constraints;
    size_t split_num = 0;
    root_.reset(fit_(X_train, y_train, rows, costs, constraints, node_prediction, start_feature_bl_, 0, pool, quiet, split_num));

    if (!root_->is_dummy())
    {
        isTrained_ = true;
        if (!quiet)
            Util::log<3>("Fitting Tree ID {} completed (is_trained = {})!",
                id_, isTrained_);
    }
}

template<size_t NX, size_t NY>
size_t RobustDecisionTree<NX,NY>::predict_(const Row<NX>& instance, const Node<NY>* node) const
{
    if (node->is_leaf())
        return node->get_prediction();
    
    auto best_feature_id = node->get_best_split_id();
    auto best_feature_value = node->get_best_split_value();
    auto x_feature_value = instance(best_feature_id);

    if (x_feature_value <= best_feature_value) // go left
        return predict_(instance, node->left());
    else // go right
        return predict_(instance, node->right());

}

template<size_t NX, size_t NY>
Row<NY> RobustDecisionTree<NX,NY>::predict_proba_(const Row<NX>& instance, const Node<NY>* node) const
{
    if (node->is_leaf())
        return node->get_prediction_score();
    
    auto best_feature_id = node->get_best_split_id();
    auto best_feature_value = node->get_best_split_value();
    auto x_feature_value = instance(best_feature_id);

    if (x_feature_value <= best_feature_value) // go left
        return predict_proba_(instance, node->left());
    else // go right
        return predict_proba_(instance, node->right());

}

template<size_t NX, size_t NY>
size_t RobustDecisionTree<NX,NY>::predict(const Row<NX>& instance) const
{
    if (isTrained_)
    {
        return predict_(instance, root_.get());
    }
    else
    {
        Util::warn("tree {} is not trained", id_);
        return -1;
    }
}
template<size_t NX, size_t NY>
size_t RobustDecisionTree<NX,NY>::predict(const std::vector<double>& instance) const
{
    if (isTrained_)
    {
        if (instance.size() < NX)
            throw std::invalid_argument(fmt::format("predict: expected a vector of length {}", NX));
        Row<NX> converted(1, NX);
        for (int64_t i = 0; i < NX; i++)
            converted(i) = instance.at(i);
        return predict_(converted, root_.get());
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
            out.row(i) = predict_proba_(X_test.row(i), root_.get());
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
double RobustDecisionTree<NX,NY>::test_score(const DF<NX>& X_test, const DF<NY>& Y_test) const
{
    auto Y_pred = predict_proba(X_test);
    auto test_acc = 100.0 - 100.0 * classification_error(Y_test, Y_pred);
    return test_acc;
}

template<size_t NX, size_t NY>
void RobustDecisionTree<NX,NY>::dump_to_disk(const std::filesystem::path& fn) const
{
    try 
    {
        std::ofstream os(fn, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(root_, id_, max_depth_, min_instances_per_node_, isTrained_, affine_,
            attacker_);
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
        std::shared_ptr<Attacker<NX>> attacker;
        archive(root, id, max_depth, min_instances_per_node, isTrained, affine, attacker);
        return RobustDecisionTree<NX,NY>(root, id, max_depth, min_instances_per_node, isTrained, affine,
            attacker);
    }
    catch (std::exception& e)
    {
        Util::die("failed to load model {}: {}", fn.c_str(), e.what());
    }
}

template<size_t NX, size_t NY>
std::string RobustDecisionTree<NX,NY>::get_model_name() const
{
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
    	return fmt::format("{}-D{}.tree", algo_str, max_depth_);
    else
    	return fmt::format("{}-B{}-D{}.tree", algo_str, attacker_->get_budget(), max_depth_);
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
struct CompareRows
{
    bool operator()(const Row<NX>& f, const Row<NX>& s) const
    {
        for (uint64_t i = 0; i < NX; i++)
        {
            if (f(i) != s(i))
                return f(i) < s(i);
        }
        return false;
    }
};

// template<size_t NX>
// using RowSet = std::set<Row<NX>, CompareRows<NX>>;

// template<size_t NX>
// static void att_recur(RowSet<NX>& set, const Row<NX>& inst, const Attacker<NX>& attacker, 
//     const std::set<size_t>& features, int budget)
// {
//     std::list<Row<NX>> out;
//     for (auto& f : features)
//     {
//         auto attacks = attacker.single_attack(inst, f, budget, false);
//         for (const auto& [att, new_budg] : attacks)
//         {
//             if (!set.contains(att))
//             {
//                 set.insert(att);
//                 att_recur<NX>(set, att, attacker, features, new_budg);
//             }
//         }
//     }
// }

template<size_t NX, size_t NY>
double RobustDecisionTree<NX,NY>::get_attacked_score(const Attacker<NX>& attacker,
    const DF<NX>& X, const DF<NY>& Y) const
{
    if (!isTrained_)
        Util::die("tree {} is not trained", id_);

    Util::die("call RobustForest::get_attacker_score() instead");
    
    // const size_t N = X.rows();
    // size_t correct = 0;
    // const auto& feats = attacker.target_features();
    // for (int64_t i = 0; i < X.rows(); i++)
    // {

    //     Eigen::Index max_ind;
    //     Y.row(i).maxCoeff(&max_ind);
    //     const auto& inst = X.row(i);
    //     const auto true_y = static_cast<size_t>(max_ind);
    //     const auto pred_y = predict(inst);
    //     if (true_y == pred_y)
    //     {
    //         correct++;
    //         RowSet<NX> set;
    //         att_recur<NX>(set, inst, attacker, feats, attacker.get_budget());
    //         for (auto& att : set)
    //         {
    //             const auto pred_att = predict(att);
    //             if (true_y != pred_att)
    //             {
    //                 correct--;
    //                 break;
    //             }
    //         }
    //     }
    // }
    // return 100.0 * static_cast<double>(correct) / static_cast<double>(N);
}

template<size_t NX, size_t NY>
double RobustDecisionTree<NX,NY>::get_own_attacked_score(const DF<NX>& X, const DF<NY>& Y) const
{
    return get_attacked_score(*attacker_, X, Y);
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