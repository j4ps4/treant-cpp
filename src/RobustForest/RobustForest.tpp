#include "../thread_pool.hpp"

#include "../util.h"

template<size_t NX, size_t NY>
RobustForest<NX,NY>::RobustForest(size_t N, TreeArguments<NX,NY>&& args) :
	tree_args_(std::move(args)), n_trees_(N), is_trained_(false)
{
	uint64_t seed = 0;
	int id = 0;
	for (size_t i = 0; i < n_trees_; i++)
	{
		TreeArguments<NX,NY> new_args = tree_args_;
		new_args.id = id++;
		trees_.emplace_back(std::move(new_args), seed++);
	}
}

template<size_t NX, size_t NY>
void RobustForest<NX,NY>::fit(const DF<NX>& X_train, const DF<NY>& y_train)
{
	thread_pool pool;
	Util::info("spawning a pool of {} threads...", pool.get_thread_count());

	for (size_t i = 0; i < n_trees_; i++)
	{
		auto& tree = trees_.at(i);
		pool.push_task([&]{
			tree.fit(X_train, y_train);
		});
	}
	pool.wait_for_tasks();
	Util::log("{} trees have been fit!", n_trees_);
}

// template<size_t NX, size_t NY>
// Row<NY> RobustForest<NX,NY>::predict_proba_(const Row<NX>& instance, const Node<NY>* node) const
// {
//     if (node->is_leaf())
//         return node->get_prediction_score();
    
//     auto best_feature_id = node->get_best_split_id();
//     auto best_feature_value = node->get_best_split_value();
//     auto x_feature_value = instance(best_feature_id);

//     if (x_feature_value <= best_feature_value) // go left
//         return predict_proba_(instance, node->left());
//     else // go right
//         return predict_proba_(instance, node->right());

// }

// template<size_t NX, size_t NY>
// DF<NY> RobustForest<NX,NY>::predict_proba(const DF<NX>& X_test) const
// {
//     if (isTrained_)
//     {
//         const auto rows = X_test.rows();
//         DF<NY> out = Eigen::ArrayXXd::Zero(rows, NY);
//         for (int64_t i = 0; i < rows; i++)
//         {
//             out.row(i) = predict_proba_(X_test.row(i), root_.get());
//         }
//         return out;
//     }
//     else
//     {
//         Util::warn("tree {} is not trained", id_);
//         return Eigen::ArrayXXd::Zero(0, NY);
//     }
// }

template<size_t NX, size_t NY>
void RobustForest<NX,NY>::print_test_score(const DF<NX>& X_test, const DF<NY>& Y_test,
	const DF<NY>& Y_train) const
{
	for (const auto& t : trees_)
	{
		auto Y_pred = t.predict_proba(X_test);
		auto test_acc = 100.0 - 100.0 * t.classification_error(Y_test, Y_pred);
		auto train_dom = dominant_class<NY>(Y_train);
		auto dummy_score = 100.0 * class_proportion<NY>(Y_test, train_dom);
		fmt::print("tree {}: test score: {:.2f}% (dummy classifier: {:.2f}%)\n", t.get_id(), test_acc, dummy_score);
	}
}