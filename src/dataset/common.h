#pragma once

#include <cxxopts.hpp>

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
	bool always_ret;
	bool use_constraints;
};

struct CrossvalArguments
{
	size_t N_folds;
	std::string logfile;
	std::vector<size_t> maxdepth;
	std::vector<size_t> min_inst;
	std::vector<bool> affine;
};

template<size_t NX>
std::vector<std::tuple<int,Attacker<NX>*>> parse_batch_file(const std::string& batch_file,
	const std::string& attack_file, int budget);

template<size_t NX, size_t NY>
std::optional<TrainArguments<NX,NY>> generate_arg_from_options(const cxxopts::ParseResult& opts,
    const std::string& sweep_param = "", size_t sweep_index = 0);

template<size_t NX, size_t NY>
double get_sweep_value(const TrainArguments<NX,NY>& args, const std::string& sweep_param);

#include "common.tpp"