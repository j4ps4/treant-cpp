
#define TRAIN_AND_SAVE(Namespace, NX, NY) \
    auto args = generate_arg_from_options<NX,NY>(options).value(); \
    verbosity = args.verbosity; \
    \
    auto m_df = Namespace::read_train_and_valid(); \
    if (m_df.has_error()) \
        Util::die("{}", m_df.error()); \
    auto& df_tupl = m_df.value(); \
    auto& X = std::get<0>(df_tupl); \
    auto& Y = std::get<1>(df_tupl); \
    \
    if (args.n_inst > 0) \
    { \
        X.conservativeResize(args.n_inst, Eigen::NoChange); \
        Y.conservativeResize(args.n_inst, Eigen::NoChange); \
    } \
    \
    auto m_test = Namespace::read_test(); \
    if (m_test.has_error()) \
        Util::die("{}", m_test.error()); \
    auto& test_tupl = m_test.value(); \
    auto& X_test = std::get<0>(test_tupl); \
    auto& Y_test = std::get<1>(test_tupl); \
    \
    Util::log<4>("X: a dataframe of size ({}x{})", X.rows(), X.cols()); \
    Util::log<4>("Y: a dataframe of size ({}x{})", Y.rows(), Y.cols()); \
    \
    if (args.algo == TrainingAlgo::Robust) \
    { \
        Namespace::json_file = args.attack_file; \
        auto m_atkr = Namespace::new_Attacker(args.budget, X, args.feature_ids, args.epsilon); \
        if (m_atkr.has_error()) \
            Util::die("{}", m_atkr.error()); \
        args.tree_args.attacker = std::move(m_atkr.value()); \
    } \
    if (args.algo == TrainingAlgo::Icml2019) \
    { \
        auto optimz = std::make_shared<SplitOptimizer<NX,NY>>(args.split, args.algo, args.maxiter, \
            args.epsilon, args.feature_ids, args.always_ret, args.use_constraints, Namespace::EPSILON_COEFF); \
        args.tree_args.optimizer = std::move(optimz); \
    } \
    else \
    { \
        auto optimz = std::make_shared<SplitOptimizer<NX,NY>>(args.split, args.algo, args.maxiter, \
            args.epsilon, args.feature_ids, args.always_ret, args.use_constraints); \
        args.tree_args.optimizer = std::move(optimz); \
    } \
    if (args.tree_args.feature_bl.empty())\
        args.tree_args.feature_bl = Namespace::DEFAULT_BL;\
    \
    RobustForest<NX,NY> forest(args.n_trees, std::move(args.tree_args)); \
    \
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now(); \
    forest.fit(X, Y); \
    double linear_time = TIME; \
    fmt::print("time elapsed: "); \
    fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time)); \
    forest.print_test_score(X_test, Y_test, Y, false); \
    auto model_name = args.output.empty() ? forest.get_model_name() : args.output; \
    auto full_model_name = models_dir / model_name; \
    Util::info("saving trained model to {}", full_model_name.native()); \
    if (!std::filesystem::exists(models_dir)) \
        std::filesystem::create_directory(models_dir); \
    forest.dump_to_disk(full_model_name); 

#define LOAD_AND_TEST(Namespace, NX, NY) \
    auto args = generate_arg_from_options<NX,NY>(options).value(); \
    verbosity = args.verbosity; \
    \
    auto m_df = Namespace::read_train_and_valid(); \
    if (m_df.has_error()) \
        Util::die("{}", m_df.error()); \
    auto& df_tupl = m_df.value(); \
    auto& X = std::get<0>(df_tupl); \
    auto& Y = std::get<1>(df_tupl); \
    \
    auto m_test = Namespace::read_test(); \
    if (m_test.has_error()) \
        Util::die("{}", m_test.error()); \
    auto& test_tupl = m_test.value(); \
    auto& X_test = std::get<0>(test_tupl); \
    auto& Y_test = std::get<1>(test_tupl); \
    \
    if (args.n_inst > 0) \
    { \
        X_test.conservativeResize(args.n_inst, Eigen::NoChange); \
        Y_test.conservativeResize(args.n_inst, Eigen::NoChange); \
    } \
    \
    auto forest = RobustForest<NX,NY>::load_from_disk(model_path); \
    \
    if (mpi_rank == 0) \
        forest.print_test_score(X_test, Y_test, Y, false); \
    \
    if (args.n_feats > 0) \
    {   \
        args.feature_ids = forest.most_important_feats(args.n_feats); \
        if (mpi_rank == 0) \
            fmt::print("testing with id_set = {}\n", args.feature_ids); \
    } \
    \
    std::vector<int> budgets(args.budget); \
    std::iota(budgets.begin(), budgets.end(), 1); \
    \
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now(); \
    \
    if (!args.attack_file.empty()) \
    {   \
        json_file = args.attack_file; \
        for (int budget : budgets) \
        { \
            auto m_atkr = Namespace::new_Attacker(budget, X_test, args.feature_ids, args.epsilon); \
            if (m_atkr.has_error()) \
                Util::die("{}", m_atkr.error()); \
            auto ptr = m_atkr.value().get(); \
            auto score = forest.get_attacked_score(*ptr, X_test, Y_test); \
            if (mpi_rank == 0) \
            { \
                fmt::print("budget {}: test score {:.2f}%\n", budget, score); \
            } \
        } \
    } \
    else \
    { \
        if (!args.feature_ids.empty()) \
            forest.set_attacker_feats(args.feature_ids); \
        for (int budget : budgets) \
        { \
            forest.set_attacker_budget(budget); \
            auto score = forest.get_own_attacked_score(X_test, Y_test); \
            if (mpi_rank == 0) \
            { \
                fmt::print("budget {}: test score {:.2f}%\n", budget, score); \
            } \
        } \
    } \
    \
    if (mpi_rank != 0) \
        return; \
    double linear_time = TIME; \
    fmt::print("time elapsed: "); \
    fmt::print(fg(fmt::color::yellow_green), "{}\n", Util::pretty_timediff(linear_time)); 

