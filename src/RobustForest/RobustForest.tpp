#include "../thread_pool.hpp"

template<size_t NX, size_t NY>
RobustForest<NX,NY>(size_t N, TreeArguments<NX,NY>&& args) :
	tree_args_(std::move(args)), n_trees_(N), is_trained_(false)
{
	if (n_trees_ == 1)
		tree_args_.bootstrap_samples = false;
	
	uint64_t seed = 0;
	for (size_t i = 0; i < n_trees_; i++)
	{
		TreeArguments<NX,NY> new_args = tree_args_;
		trees_.emplace_back(std::move(new_args_), seed++);
	}
}