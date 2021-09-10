#pragma once

#include <string_view>
#include <array>
#include <stddef.h>

#define CREDIT 1
constexpr size_t CREDIT_X = 23;
constexpr size_t CREDIT_Y = 2;

#include <string>
#include <memory>
#include "../result.hpp"
#include "../DF2/DF_def.h"
#include "../Attacker.h"
#include "../RobustForest/RobustDecisionTree.h"
#include "../RobustForest/RobustForest.h"
#include "common.h"

namespace credit
{
    // Create a robust decision tree
    RobustDecisionTree<CREDIT_X,CREDIT_Y> new_RDT(TreeArguments<CREDIT_X,CREDIT_Y>&& args);

    void train_and_save(TrainArguments<CREDIT_X,CREDIT_Y>&& args);
    void batch_train_and_save(TrainArguments<CREDIT_X,CREDIT_Y>&& args, const std::string& batch_file);

    void load_and_test(const std::filesystem::path& model, const std::string& attack_file,
        const std::set<size_t>& id_set, int max_budget);
    void put_gain_values(const std::filesystem::path& model);
    void classify(const std::filesystem::path& model, const std::vector<double>& inst);
    void attack_instance(const std::string& attack_file, const std::vector<double>& inst,
        const std::set<size_t>& id_set, int budget, int cost);
}
