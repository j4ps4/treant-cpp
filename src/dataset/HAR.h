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
#include "common.h"

namespace har
{
    // Create a robust decision tree
    RobustDecisionTree<HAR_X,HAR_Y> new_RDT(TreeArguments<HAR_X,HAR_Y>&& args);

    void train_and_save(TrainArguments<HAR_X,HAR_Y>&& args);

    void load_and_test(const std::filesystem::path& model, const std::string& attack_file);
    void put_gain_values(const std::filesystem::path& model);
    void classify(const std::filesystem::path& model, const std::vector<double>& inst);
}