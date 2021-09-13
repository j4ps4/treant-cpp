#pragma once

#include "../RobustForest/RobustDecisionTree.h"

template<size_t NX, size_t NY>
struct TrainArguments
{
	TreeArguments<NX,NY> tree_args;
	std::string attack_file;
	int n_inst;
	int budget;
	std::set<size_t> feature_ids;
	std::string output;
    SplitFunction split;
    TrainingAlgo algo;
    int maxiter;
    size_t n_trees;
	double epsilon;
};

template<size_t NX>
std::vector<std::tuple<int,Attacker<NX>*>> parse_batch_file(const std::string& batch_file,
	const std::string& attack_file, int budget);

#include "common.tpp"