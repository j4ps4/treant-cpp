#pragma once

#include <string_view>
#include <array>
#include <stddef.h>

#define FOREST 1
constexpr size_t FOREST_X = 54;
constexpr size_t FOREST_Y = 7;

#include <string>
#include <memory>
#include "../result.hpp"
#include "../DF2/DF_def.h"
#include "../Attacker.h"
#include "../RobustForest/RobustDecisionTree.h"

namespace covertype
{
    // Read bzipped csv to dataframe.
    cpp::result<std::tuple<DF<FOREST_X>,DF<FOREST_Y>>,std::string> read_train();
    cpp::result<std::tuple<DF<FOREST_X>,DF<FOREST_Y>>,std::string> read_test();

    // Create attacker from given attacks file and budget.
    cpp::result<std::unique_ptr<Attacker<FOREST_X>>,std::string> new_Attacker(int budget, const DF<FOREST_X>& X);

    // Create a robust decision tree
    RobustDecisionTree<FOREST_X,FOREST_Y> new_RDT(TreeArguments<FOREST_X,FOREST_Y>&& args);

    void train_and_test(SplitFunction fun, TrainingAlgo algo, size_t max_depth, 
        size_t min_instances_per_node, int budget, bool affine);
}
