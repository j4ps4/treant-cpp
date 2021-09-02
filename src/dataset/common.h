#pragma once

#include "../RobustForest/RobustDecisionTree.h"

template<size_t NX, size_t NY>
struct train_args
{
	TreeArguments<NX,NY> tree_args;
	std::string attack_file;
	int n_inst;
	int budget;
};