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
#include "common.h"

namespace credit
{
    // Read bzipped csv to dataframe.
    cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_train();
    cpp::result<std::tuple<DF<CREDIT_X>,DF<CREDIT_Y>>,std::string> read_test();

    // Create attacker from given attacks file and budget.
    cpp::result<std::unique_ptr<Attacker<CREDIT_X>>,std::string> new_Attacker(int budget, 
        const DF<CREDIT_X>& X, bool print = true);

    // Create a robust decision tree
    RobustDecisionTree<CREDIT_X,CREDIT_Y> new_RDT(TreeArguments<CREDIT_X,CREDIT_Y>&& args);

    void train_and_save(TrainArguments<CREDIT_X,CREDIT_Y>&& args);

    void load_and_test(const std::filesystem::path& fn, const std::string& attack_file);
    void put_gain_values(const std::filesystem::path& fn);
}
