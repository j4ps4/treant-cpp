// Human Activity Recognition dataset

#pragma once

#include <string_view>
#include <array>
#include <stddef.h>

#include <string>
#include <memory>
#include <cxxopts.hpp>

#include "../result.hpp"
#include "../DF2/DF_def.h"
#include "../Attacker.h"
#include "../RobustForest/RobustForest.h"
#include "common.h"

namespace har
{
    void train_and_save(const cxxopts::ParseResult& options);
    void batch_train_and_save(const cxxopts::ParseResult& options);
    void argument_sweep(const cxxopts::ParseResult& options);

    void load_and_test(const cxxopts::ParseResult& options, const std::filesystem::path& model_path, int mpi_np, int mpi_rank);
    void put_gain_values(const std::filesystem::path& model);
    void classify_inst(const std::filesystem::path& model, const std::vector<double>& inst);
    void attack_instance(const std::string& attack_file, const std::vector<double>& inst,
        const std::set<size_t>& id_set, int budget, int cost, double epsilon);
    void set_verbosity(int verb);
}