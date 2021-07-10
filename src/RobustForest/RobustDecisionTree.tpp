#include "../util.h"

#include <algorithm>
#include <numeric>

template<size_t NX, size_t NY>
Node* RobustDecisionTree<NX,NY>::private_fit(const DF<NX>& X_train, const DF<NY>& y_train, const std::vector<size_t> rows,
    std::map<size_t,int>& costs, const Row<NY>& node_prediction, std::set<size_t> feature_blacklist, size_t depth)
{
    if (X_train.size() == 0)
        return new Node();

    DF<NX> X = DF_index(X_train, rows);
    DF<NY> y = DF_index(y_train, rows);
    Node* node = new Node(y.rows());
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
    auto& [best_gain, best_split_left, best_split_right, best_split_feature_id,
           best_split_feature_value, next_best_split_feature_id,
           best_pred_left, best_pred_right, best_loss,
           costs_left, costs_right] = optimizer_->optimize_gain(X_train, y_train, rows, feature_blacklist, -1, attacker_, costs, current_score);
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
    std::map<size_t,int> costs;
    for (size_t i = 0; i < X_train.rows(); i++)
        costs[i] = 0;
    root_ = private_fit(X_train, y_train, rows, costs, node_prediction, start_feature_bl_, 0);

    if (!root_->is_dummy())
    {
        isTrained_ = true;
        Util::log("Fitting Tree ID {} completed (is_trained = {})!",
            id_, isTrained_);
    }
}