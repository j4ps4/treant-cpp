// Human Activity Recognition dataset

#pragma once

#include <string_view>
#include <array>
#include <stddef.h>

constexpr size_t HAR_X = 30; // 561
constexpr size_t HAR_Y = 6;

#include <string>
#include <memory>
#include "../result.hpp"
#include "../DF2/DF_def.h"
#include "../Attacker.h"
#include "../RobustForest/RobustDecisionTree.h"

namespace har
{
    // Read bzipped csv to dataframe.
    cpp::result<std::tuple<DF<HAR_X>,DF<HAR_Y>>,std::string> read_train();
    cpp::result<std::tuple<DF<HAR_X>,DF<HAR_Y>>,std::string> read_test();

    // Create attacker from given attacks file and budget.
    cpp::result<std::unique_ptr<Attacker<HAR_X>>,std::string> new_Attacker(int budget, const DF<HAR_X>& X,
        bool print = true);

    // Create a robust decision tree
    RobustDecisionTree<HAR_X,HAR_Y> new_RDT(TreeArguments<HAR_X,HAR_Y>&& args);

    void train_and_test(SplitFunction fun, TrainingAlgo algo, size_t max_depth, 
        size_t min_instances_per_node, int budget, int maxiter, bool affine, int n_inst);

    void load_and_test(const std::filesystem::path& fn);
}