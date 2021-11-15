#include <fstream>
#include <charconv>


template<size_t NX>
std::vector<std::tuple<int,Attacker<NX>*>> parse_batch_file(const std::string& batch_file,
	const std::string& attack_file, int budget, double epsilon)
{
	if (attack_file.empty())
		throw std::invalid_argument("attack file must be given");
	static const auto chars = {',', '\n'};
	std::vector<std::tuple<int,Attacker<NX>*>> result;
	try
	{
		std::string line;
		std::ifstream is(batch_file);
		while (!is.eof())
		{
			std::getline(is, line);
			if (line.empty())
				continue;
	        auto pos = line.find(':');
			int id;
			std::from_chars(line.c_str(), line.c_str()+pos, id);
			auto rest = line.substr(pos+1);
			auto searchBeg = rest.data();
			const auto dataEnd = searchBeg + line.size();
			std::set<size_t> id_set;
			while (searchBeg < dataEnd)
			{
				auto comma = std::find_first_of(searchBeg, dataEnd, chars.begin(), chars.end());
				size_t fid;
				std::from_chars(searchBeg, comma, fid);
				searchBeg = comma+1;
				id_set.insert(fid);
			}

			auto res = load_attack_rules(attack_file, {}, id_set, epsilon);
			if (res.has_error())
				throw std::runtime_error(res.error());
			auto& rulz = res.value();
			auto atkr = new Attacker<NX>(std::move(rulz), budget, std::set<size_t>());
			result.push_back(std::make_tuple(id, atkr));
		}
		return result;
	}
	catch (std::exception& e)
	{
		Util::die("can't read batch file: {}", e.what());
	}
}

template<size_t NX, size_t NY>
std::optional<TrainArguments<NX,NY>> generate_arg_from_options(const cxxopts::ParseResult& opts,
    const std::string& sweep_param, size_t sweep_index)
{
    auto feature_id_vec = opts["feature-ids"].as<std::vector<size_t>>();
    std::set<size_t> feature_id;
    for (auto f : feature_id_vec)
        feature_id.insert(f);
    auto attack_file = opts["attack-file"].as<std::string>();
    std::string batch_file;
    if (opts.count("batch"))
        batch_file = opts["batch"].as<std::string>();
    auto cost = opts["cost"].as<int>();
    auto n_inst = opts["n-inst"].as<int>();
    auto n_feats = opts["n-feats"].as<int>();
    auto budget_v = opts["budget"].as<std::vector<int>>();
    auto maxdepth_v = opts["maxdepth"].as<std::vector<size_t>>();
    auto algostr = opts["algo"].as<std::string>();
    auto outputstr = opts["output"].as<std::string>();
    auto logfile = opts["log-file"].as<std::string>();
    auto maxiter = opts["maxiter"].as<int>();
    auto epsilon_v = opts["epsilon"].as<std::vector<double>>();
    auto n_trees = opts["n-trees"].as<size_t>();
    auto min_inst_v = opts["min-instances-per-node"].as<std::vector<size_t>>();
    auto min_inst = min_inst_v.front();
    auto par_par = opts["par-par"].as<double>();
    auto affine_v = opts["affine"].as<std::vector<bool>>();
    auto affine = affine_v.front();
    auto par = opts["par"].as<bool>();
    auto always_ret = opts["nlopt-always-return"].as<bool>();
    auto use_constraints = opts["use-constraints"].as<bool>();
    auto bootstrap_samples = opts["sample-instances"].as<bool>();
    auto bootstrap_features = opts["sample-features"].as<bool>();
    auto replace_samples = opts["replace-instances"].as<bool>();
    auto replace_features = opts["replace-features"].as<bool>();
    auto instance_prop_v = opts["instance-prop"].as<std::vector<double>>();
    auto feature_prop_v = opts["feature-prop"].as<std::vector<double>>();
    auto feature_bl_vec = opts["feature-blacklist"].as<std::vector<size_t>>();
    std::set<size_t> feature_bl;
    for (auto f : feature_bl_vec)
        feature_bl.insert(f);
    auto verb = opts["verbosity"].as<int>();
    auto algo = Util::parse_algo(algostr);

    int budget;
    size_t maxdepth;
    double epsilon;
    double instance_prop;
    double feature_prop;
    if (sweep_param == "budget")
    {
        if (sweep_index >= budget_v.size())
            return {};
        budget = budget_v[sweep_index];
    }
    else
        budget = budget_v.front();
    if (sweep_param == "maxdepth")
    {
        if (sweep_index >= maxdepth_v.size())
            return {};
        maxdepth = maxdepth_v[sweep_index];
    }
    else
        maxdepth = maxdepth_v.front();
    if (sweep_param == "epsilon")
    {
        if (sweep_index >= epsilon_v.size())
            return {};
        epsilon = epsilon_v[sweep_index];
    }
    else
        epsilon = epsilon_v.front();
    if (sweep_param == "instance-prop")
    {
        if (sweep_index >= instance_prop_v.size())
            return {};
        instance_prop = instance_prop_v[sweep_index];
    }
    else
        instance_prop = instance_prop_v.front();
    if (sweep_param == "feature-prop")
    {
        if (sweep_index >= feature_prop_v.size())
            return {};
        feature_prop = feature_prop_v[sweep_index];
    }
    else
        feature_prop = feature_prop_v.front();


    TrainArguments<NX,NY> args = {.tree_args = {.attacker=nullptr, .optimizer=nullptr, .feature_bl=feature_bl,
                .id=0, .max_depth=maxdepth, .min_instances_per_node=min_inst, .affine=affine,
                .useParallel=par, .par_par=par_par, .bootstrap_samples=bootstrap_samples, 
                .bootstrap_features=bootstrap_features, .replace_samples=replace_samples, 
                .replace_features=replace_features, .max_samples=instance_prop, 
                .max_features=feature_prop},
        .attack_file = attack_file, .batch_file = batch_file, .n_inst = n_inst, .n_feats = n_feats, .budget = budget, .feature_ids = feature_id,
        .output = outputstr, .split=SplitFunction::LogLoss, .algo=algo, .maxiter=maxiter,
        .n_trees=n_trees, .epsilon=epsilon, .always_ret=always_ret, .use_constraints=use_constraints, .verbosity = verb};
    return args;
}

template<size_t NX, size_t NY>
double get_sweep_value(const TrainArguments<NX,NY>& args, const std::string& sweep_param)
{
    if (sweep_param == "budget")
        return args.budget;
    else if (sweep_param == "maxdepth")
        return args.tree_args.max_depth;
    else if (sweep_param == "epsilon")
        return args.epsilon;
    else if (sweep_param == "instance-prop")
        return args.tree_args.max_samples;
    else if (sweep_param == "feature-prop")
        return args.tree_args.max_features;
    else
        throw std::invalid_argument("invalid sweep_param");
    return 0.0;
}

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
        json_file = args.attack_file; \
        auto m_atkr = Namespace::new_Attacker(args.budget, X, args.feature_ids, args.epsilon); \
        if (m_atkr.has_error()) \
            Util::die("{}", m_atkr.error()); \
        args.tree_args.attacker = std::move(m_atkr.value()); \
    } \
    \
    if (args.algo == TrainingAlgo::Icml2019) \
    { \
        auto optimz = std::make_shared<SplitOptimizer<NX,NY>>(args.split, args.algo, args.maxiter, \
            args.epsilon, args.feature_ids, args.always_ret, args.use_constraints, EPSILON_COEFF); \
        args.tree_args.optimizer = std::move(optimz); \
    } \
    else \
    { \
        auto optimz = std::make_shared<SplitOptimizer<NX,NY>>(args.split, args.algo, args.maxiter, \
            args.epsilon, args.feature_ids, args.always_ret, args.use_constraints); \
        args.tree_args.optimizer = std::move(optimz); \
    } \
    if (args.tree_args.feature_bl.empty())\
        args.tree_args.feature_bl = DEFAULT_BL;\
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
