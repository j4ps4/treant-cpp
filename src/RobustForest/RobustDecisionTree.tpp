#include "../util.h"

#include <algorithm>
#include <numeric>

template<size_t NX, size_t NY>
Node<NY>* RobustDecisionTree<NX,NY>::private_fit(const DF<NX>& X_train, const DF<NY>& y_train, const std::vector<size_t> rows,
    std::map<int64_t,int>& costs, const Row<NY>& node_prediction, std::set<size_t> feature_blacklist, size_t depth)
{
    if (X_train.size() == 0)
        return new Node<NY>();

    DF<NX> X = DF_index<NX>(X_train, rows);
    DF<NY> y = DF_index<NY>(y_train, rows);
    Node<NY>* node = new Node<NY>(y.rows());
    node->set_prediction(node_prediction);
    Util::log("tree {}: current depth: {}", id_, depth);
    auto current_prediction = node->get_prediction();
    auto current_score = optimizer_->evaluate_split(y, node_prediction);
    node->set_loss(current_score);
    Util::log("current node's loss: {:.5f}", current_score);

    if (depth == max_depth_)
    {
        Util::log("current depth {} is equal to maximum depth of this tree", depth);
        return node;
    }

    if (rows.size() < min_instances_per_node_)
    {
        Util::log("Number of instances ended up in the current node {} are less than the minimum number of instances at each node of this tree {}",
            rows.size(), min_instances_per_node_);
        return node;
    }
    // OptimTupl optimize_gain(const DF<NX>& X, const DF<N>& y, const IdxVec& rows, int n_sample_features, 
    //    Attacker<NA>& attacker, const CostVec& costs, double current_score);
    // using OptimTupl = std::tuple<double,IdxVec,IdxVec,size_t,double,double,NRow,NRow,double,CostMap,CostMap>;
    auto [best_gain, best_split_left, best_split_right, best_split_feature_id,
           best_split_feature_value, next_best_split_feature_id,
           best_pred_left, best_pred_right, best_loss,
           costs_left, costs_right] = optimizer_->optimize_gain(X_train, y_train, rows, feature_blacklist, -1, *(attacker_.get()), costs, current_score);

    Util::log("best_gain: {}", best_gain);
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

        node->set_left(private_fit(X_train, y_train, best_split_left, costs_left, best_pred_left, updated_feature_bl, depth+1));
        node->set_right(private_fit(X_train, y_train, best_split_right, costs_right, best_pred_right, updated_feature_bl, depth+1));
    }
    return node;
}

template<size_t NX, size_t NY>
void RobustDecisionTree<NX,NY>::fit(const DF<NX>& X_train, const DF<NY>& y_train)
{
    std::vector<size_t> rows(X_train.rows());
    std::iota(rows.begin(), rows.end(), 0);
    // null prediction
    Row<NY> node_prediction = y_train.colwise().mean();
    std::map<int64_t,int> costs;
    for (int64_t i = 0; i < X_train.rows(); i++)
        costs[i] = 0;
    root_.reset(private_fit(X_train, y_train, rows, costs, node_prediction, start_feature_bl_, 0));

    if (!root_->is_dummy())
    {
        isTrained_ = true;
        Util::log("Fitting Tree ID {} completed (is_trained = {})!",
            id_, isTrained_);
    }
}

template<size_t NX, size_t NY>
Row<NY> RobustDecisionTree<NX,NY>::private_predict(const Row<NX>& instance, const Node<NY>* node) const
{
    if (node->is_leaf())
        return node->get_prediction_score();
    
    auto best_feature_id = node->get_best_split_id();
    auto best_feature_value = node->get_best_split_value();
    auto x_feature_value = instance(best_feature_id);

    if (x_feature_value <= best_feature_value) // go left
        return private_predict(instance, node->left());
    else // go right
        return private_predict(instance, node->right());

}

template<size_t NX, size_t NY>
DF<NY> RobustDecisionTree<NX,NY>::predict(const DF<NX>& X_test) const
{
    if (isTrained_)
    {
        const auto rows = X_test.rows();
        DF<NY> out = Eigen::ArrayXXd::Zero(rows, NY);
        for (int64_t i = 0; i < rows; i++)
        {
            out.row(i) << private_predict(X_test.row(i), root_.get());
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