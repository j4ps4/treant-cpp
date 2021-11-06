#pragma once

#include <string_view>
#include <array>
#include <stddef.h>

#include <cxxopts.hpp>

#define FOREST 1
constexpr size_t FOREST_X = 54;
constexpr size_t FOREST_Y = 7;

#include <string>
#include <memory>
#include "../result.hpp"
#include "../DF2/DF_def.h"
#include "../RobustForest/RobustForest.h"
#include "common.h"

namespace covertype
{
    void train_and_save(const cxxopts::ParseResult& options);
    void batch_train_and_save(const cxxopts::ParseResult& options, const std::string& batch_file);
    void argument_sweep(const cxxopts::ParseResult& options);

    void load_and_test(const std::filesystem::path& model, const std::string& attack_file,
        const std::set<size_t>& id_set, int max_budget, int n_inst);
    void put_gain_values(const std::filesystem::path& model);
    void classify(const std::filesystem::path& model, const std::vector<double>& inst);
    void attack_instance(const std::string& attack_file, const std::vector<double>& inst,
        const std::set<size_t>& id_set, int budget, int cost);
    void set_verbosity(int verb);
}
