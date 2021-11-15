#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <mpi.h>

#include "../thread_pool.hpp"

#include "../util.h"

static void send_data(const std::string& data, int dest, int tag)
{
    const int SZ = data.size() * sizeof(std::string::value_type);
    int err = MPI_Send(data.data(), SZ, MPI_BYTE, dest, tag, MPI_COMM_WORLD);
    if (err != MPI_SUCCESS)
    {
        auto str = fmt::format("MPI_Send error code {}", err);
        throw std::runtime_error(str);
    }
}

static std::string recv_data(int source, int tag)
{
    MPI_Status status;
    MPI_Probe(source, tag, MPI_COMM_WORLD, &status);
    int amount;
    MPI_Get_count(&status, MPI_BYTE, &amount);
    const int SZ = amount / sizeof(std::string::value_type);
    std::string buf(SZ, '\0');
    MPI_Recv(buf.data(), amount, MPI_BYTE, source, tag, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    return buf;
}

static void send_ulong(const size_t& val, int dest, int tag)
{
    int err = MPI_Send(&val, 1, MPI_UNSIGNED_LONG, dest, tag, MPI_COMM_WORLD);
    if (err != MPI_SUCCESS)
    {
        auto str = fmt::format("MPI_Send error code {}", err);
        throw std::runtime_error(str);
    }
}

static size_t recv_ulong(int source, int tag)
{
    size_t val;
    MPI_Recv(&val, 1, MPI_UNSIGNED_LONG, source, tag, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    return val;
}

template<size_t NX, size_t NY>
RobustForest<NX,NY>::RobustForest(size_t N, TreeArguments<NX,NY>&& args) :
	tree_args_(std::move(args)), n_trees_(N), is_trained_(false),
    type_(ForestType::Forest), N_folds_(1)
{
    if (N == 0)
        throw std::runtime_error("N must be larger than 0");
	uint64_t seed = 0;
	int id = 0;
	for (size_t i = 0; i < n_trees_; i++)
	{
		TreeArguments<NX,NY> new_args = tree_args_;
		new_args.id = id++;
		trees_.emplace_back(std::move(new_args), seed++);
	}
}

template<size_t NX, size_t NY>
RobustForest<NX,NY>::RobustForest(size_t N, TreeArguments<NX,NY>&& args,
	const std::vector<std::tuple<int,Attacker<NX>*>>& atkrs) :
	tree_args_(std::move(args)), n_trees_(N), is_trained_(false),
    type_(ForestType::Bundle), N_folds_(1)
{
    if (N == 0)
        throw std::runtime_error("N must be larger than 0");
	if (n_trees_ > atkrs.size())
		Util::die("not enough attackers");
	uint64_t seed = 0;
	for (size_t i = 0; i < n_trees_; i++)
	{
		TreeArguments<NX,NY> new_args = tree_args_;
		int id = std::get<0>(atkrs.at(i));
		auto atkr = std::get<1>(atkrs.at(i));
		new_args.id = id;
		new_args.attacker.reset(atkr);
		trees_.emplace_back(std::move(new_args), seed++);
	}
}

template<size_t NX, size_t NY>
RobustForest<NX,NY>::RobustForest(const std::vector<TreeArguments<NX,NY>>& args, size_t N_folds,
    const std::string& log) :
    is_trained_(false), type_(ForestType::Fold), N_folds_(N_folds)
{
    if (N_folds < 1)
        throw std::runtime_error("N_folds must be larger than 0");

    if (!log.empty())
        logfile_ = log;
    else
        logfile_ = "cv-out.csv";

    n_trees_ = args.size();
    int id = 0;
	for (size_t i = 0; i < args.size(); i++)
	{
        for (size_t j = 0; j < N_folds_; j++)
        {
            auto arg_i = args[i];
            arg_i.id = id++;
		    trees_.emplace_back(std::move(arg_i), 0);
        }
	}
}

template<size_t N>
static std::tuple<DF<N>, DF<N>> compute_train_and_validation_sets(const DF<N>& orig, 
    size_t set_idx, size_t total_N)
{
    const auto rows = orig.rows();
    const auto bucket_size = rows / total_N;
    if (set_idx == 1)
    {
        DF<N> valid = orig(Eigen::seqN(0, bucket_size), Eigen::all);
        DF<N> train = orig(Eigen::lastN(rows - bucket_size), Eigen::all);
        return {train, valid};
    }
    else if (set_idx == total_N)
    {
        DF<N> train = orig(Eigen::seq(0, rows - bucket_size - 1), Eigen::all);
        DF<N> valid = orig(Eigen::lastN(bucket_size), Eigen::all);
        return {train, valid};
    }
    else
    {
        const auto start = bucket_size*(set_idx-1);
        const auto end = bucket_size*set_idx;
        std::vector<size_t> test_idx;
        for (size_t i = 0; i < start; i++)
            test_idx.push_back(i);
        for (size_t i = end; i < rows; i++)
            test_idx.push_back(i);
        DF<N> train = orig(test_idx, Eigen::all);
        DF<N> valid = orig(Eigen::seqN(start, bucket_size), Eigen::all);
        return {train, valid};
    }
}

template<size_t N>
static DF<N> splice(const DF<N>& orig, size_t splice_idx, size_t N_splices)
{
    const size_t rows = orig.rows();
    const size_t splice_size = rows / N_splices;
    if (splice_idx < N_splices - 1)
    {
        const auto start = splice_size*splice_idx;
        DF<N> spliced = orig(Eigen::seqN(start, splice_size), Eigen::all);
        return spliced;
    }
    else
    {
        DF<N> spliced = orig(Eigen::lastN(splice_size), Eigen::all);
        return spliced;
    }
}

template<>
std::tuple<DF<784>, DF<784>> compute_train_and_validation_sets<784>(const DF<784>& orig, 
    size_t set_idx, size_t total_N)
{
    throw std::runtime_error("get reckt");
}

template<size_t NX, size_t NY>
void RobustForest<NX,NY>::fit(const DF<NX>& X_train, const DF<NY>& y_train)
{
    const bool treePar = trees_.front().is_parallel();
    if (n_trees_ > 1 || N_folds_ > 1)
    {

        if (type_ == ForestType::Fold)
        {
            thread_pool pool;
            Util::info("spawning a pool of {} threads...", pool.get_thread_count());

            std::ofstream log_out(logfile_);
            log_out << "maxdepth,min_inst,affine,cv_score\n";
            std::vector<DF<NX>> trainX;
            std::vector<DF<NX>> validX;
            std::vector<DF<NY>> trainY;
            std::vector<DF<NY>> validY;
            if (N_folds_ > 1)
            {
                for (size_t fold_id = 1; fold_id <= N_folds_; fold_id++)
                {
                    auto [trainX_fold, validX_fold] = compute_train_and_validation_sets<NX>(X_train, fold_id, N_folds_);
                    auto [trainY_fold, validY_fold] = compute_train_and_validation_sets<NY>(y_train, fold_id, N_folds_);
                    trainX.push_back(std::move(trainX_fold));
                    validX.push_back(std::move(validX_fold));
                    trainY.push_back(std::move(trainY_fold));
                    validY.push_back(std::move(validY_fold));
                }
            }
            else
            {
                trainX.push_back(X_train);
                validX.push_back(X_train);
                trainY.push_back(y_train);
                validY.push_back(y_train);
            }

            DynRow tree_scores(1, n_trees_);

            for (size_t i = 0; i < n_trees_; i++)
            {
                std::vector<std::future<double>> futures;
                for (size_t j = 0; j < N_folds_; j++)
                {
                    const auto index = i*N_folds_ + j;
                    auto& tree = trees_.at(index);
                    futures.push_back(pool.submit([&,j]{
                        tree.fit(trainX.at(j), trainY.at(j), true);
                        return tree.test_score(validX.at(j), validY.at(j));
                    }));
                }
                DynRow fold_scores(1, N_folds_);
                for (size_t j = 0; j < N_folds_; j++)
                {
                    fold_scores(j) = futures.at(j).get();
                }
                // calculate mean, select best tree
                tree_scores(i) = fold_scores.mean();
                auto maxdepth = trees_.at(i*N_folds_).get_max_depth();
                auto min_inst = trees_.at(i*N_folds_).get_min_inst();
                auto affine = trees_.at(i*N_folds_).get_affine();
                log_out << maxdepth << "," << min_inst << "," << affine << "," << tree_scores(i) << "\n";
            }
            Eigen::Index max_ind;
            tree_scores.maxCoeff(&max_ind);
            is_trained_ = true;
            RobustDecisionTree<NX,NY> best = std::move(trees_.at(max_ind*N_folds_));
            trees_.clear();
            trees_.push_back(std::move(best));
            type_ = ForestType::Forest;
            Util::log<3>("{} trees have been fit using {}-fold cv!", n_trees_, N_folds_);
            n_trees_ = 1;
            log_out.close();
            // for (size_t i = 0; i < n_trees_; i++)
            //     if (i != max_ind)
            //         trees_.erase(trees_.begin()+i);
        }
        else
        {

            int world_size;
            MPI_Comm_size(MPI_COMM_WORLD, &world_size);
            if (world_size != n_trees_) {
                fmt::print(stderr, "world size (= {}) must equal the amount of trees (= {})\n", world_size, n_trees_);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            int world_rank;
            MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

            // Master
            if (world_rank == 0)
            {
                trees_.front().fit(X_train, y_train, true);
                for (size_t i = 1; i < n_trees_; i++)
                {
                    const auto tree_string = recv_data(i, 1);
                    auto tree = RobustDecisionTree<NX,NY>::from_string(tree_string);
                    trees_[i] = std::move(tree);
                }
                is_trained_ = true;
                Util::log<3>("Forest of {} trees has been fit!", n_trees_);
            }
            // Servant
            else
            {
                auto& tree = trees_[world_rank];
                tree.fit(X_train, y_train, true);
                const auto tree_string = tree.to_string();
                send_data(tree_string, 0, 1);
                MPI_Finalize();
                std::exit(0);
            }

        }
    }
    else
    {
        trees_.front().fit(X_train, y_train, false);
        if (trees_.front().is_trained())
            is_trained_ = true;
    }
}

static size_t mode(const std::vector<size_t>& a)
{
    auto max = *std::max_element(a.begin(), a.end());
 
    const auto t = max + 1;
    std::vector<size_t> count(t);
 
    for (size_t i = 0; i < a.size(); i++)
        count[a[i]]++;
 
    size_t mode = 0;
    size_t k = count[0];
    for (size_t i = 1; i < t; i++) {
        if (count[i] > k) {
            k = count[i];
            mode = i;
        }
    }
 
    return mode;
}

template<size_t NX, size_t NY>
size_t RobustForest<NX,NY>::predict(const Row<NX>& instance) const
{
    if (is_trained_)
    {

		std::vector<size_t> predictions(n_trees_);
		for (int64_t j = 0; j < n_trees_; j++)
		{
			predictions[j] = trees_[j].predict(instance);
		}
		return mode(predictions);
    }
    else
    {
        Util::warn("forest is not trained");
        return -1;
    }
}


template<size_t NX, size_t NY>
size_t RobustForest<NX,NY>::predict(const std::vector<double>& instance) const
{
	if (is_trained_)
	{
        if (instance.size() != NX)
            throw std::runtime_error(fmt::format("predict: expected a vector of length {}, got {}", 
                NX, instance.size()));
        Row<NX> converted(1, NX);
        for (int64_t i = 0; i < NX; i++)
            converted(i) = instance.at(i);
		return predict(converted);
	}
    else
    {
        Util::warn("forest is not trained");
        return -1;
    }
}

template<size_t NX, size_t NY>
DF<NY> RobustForest<NX,NY>::predict_proba(const DF<NX>& X_test) const
{
    if (is_trained_)
    {

        const auto rows = X_test.rows();
        DF<NY> out = Eigen::ArrayXXd::Zero(rows, NY);
        for (int64_t i = 0; i < rows; i++)
        {
			DF<NY> temp(n_trees_, NY);
			for (int64_t j = 0; j < n_trees_; j++)
			{
            	temp.row(j) = trees_[j].predict_proba_single(X_test.row(i));
			}
			out.row(i) = temp.colwise().mean();
        }
        return out;
    }
    else
    {
        Util::warn("forest is not trained");
        return Eigen::ArrayXXd::Zero(0, NY);
    }
}

template<size_t NX, size_t NY>
double RobustForest<NX,NY>::classification_error(const DF<NY>& Y_test,
    const DF<NY>& Y_pred)
{
    if (Y_test.rows() != Y_pred.rows())
    {
        Util::warn("classification_error(): argument size mismatch");
        return -1.0;
    }
    const auto rows = Y_test.rows();
    int64_t accum = 0;
    for (int64_t i = 0; i < rows; i++)
    {
        const auto& test_row = Y_test.row(i);
        const auto& pred_row = Y_pred.row(i);
        Eigen::Index test_max_ind;
        test_row.maxCoeff(&test_max_ind);
        Eigen::Index pred_max_ind;
        pred_row.maxCoeff(&pred_max_ind);
        if (test_max_ind != pred_max_ind)
            accum++;
    }
    return static_cast<double>(accum) / static_cast<double>(rows);
}

template<size_t NX, size_t NY>
void RobustForest<NX,NY>::print_test_score(const DF<NX>& X_test, const DF<NY>& Y_test,
	const DF<NY>& Y_train, bool valid) const
{
    const char* templ;
    if (valid)
        templ = "validation score: {:.2f}% (dummy classifier: {:.2f}%)\n";
    else
        templ = "test score: {:.2f}% (dummy classifier: {:.2f}%)\n";
    auto Y_pred = predict_proba(X_test);
    auto test_acc = 100.0 - 100.0 * classification_error(Y_test, Y_pred);
    auto train_dom = dominant_class<NY>(Y_train);
    auto dummy_score = 100.0 * class_proportion<NY>(Y_test, train_dom);
    fmt::print(templ, test_acc, dummy_score);
}

template<size_t NX, size_t NY>
std::string RobustForest<NX,NY>::get_model_name() const
{
	std::string algo_str;
    const auto optim = trees_.front().get_optimizer();
    const auto algo = optim->get_algorithm();
    if (algo == TrainingAlgo::Icml2019)
		algo_str = fmt::format("ICML2019-E{}", optim->get_epsilon());
    else if (algo == TrainingAlgo::Robust)
    	algo_str = "Robust";
    else
        algo_str = "Standard";
	std::string extension;
	if (type_ == ForestType::Bundle)
		extension = "bundle";
	else
		extension = n_trees_ > 1 ? "forest" : "tree";
    auto md = trees_.front().get_max_depth();
    if (algo == TrainingAlgo::Standard)
    	return fmt::format("{}-N{}-D{}.{}", algo_str, n_trees_, md, extension);
    else if (algo == TrainingAlgo::Icml2019)
    	return fmt::format("{}-N{}-D{}.{}", algo_str, n_trees_, md, extension);
    else
    {
	    int budget = trees_.front().get_attacker_budget();
    	return fmt::format("{}-N{}-B{}-D{}.{}", algo_str, n_trees_, budget, md, extension);
    }
}

template<size_t NX, size_t NY>
void RobustForest<NX,NY>::dump_to_disk(const std::filesystem::path& fn) const
{
    try 
    {
        std::ofstream os(fn, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(n_trees_, trees_, is_trained_, type_);
	}
    catch (std::exception& e)
    {
        Util::err("failed to save the model: {}", e.what());
    }
}

template<size_t NX, size_t NY>
RobustForest<NX,NY> RobustForest<NX,NY>::load_from_disk(const std::filesystem::path& fn)
{
    try
    {
        std::ifstream is(fn, std::ios::binary);
        cereal::BinaryInputArchive archive(is);
		size_t n_trees;
		std::vector<RobustDecisionTree<NX,NY>> trees;
		bool is_trained;
		ForestType type;
        archive(n_trees, trees, is_trained, type);
        return RobustForest<NX,NY>(n_trees, std::move(trees), is_trained, type);
    }
    catch (std::exception& e)
    {
        Util::die("failed to load model {}: {}", fn.c_str(), e.what());
    }
}

template<size_t NX>
using RowSetF = std::set<Row<NX>, CompareRows<NX>>;

template<size_t NX, size_t NY>
static bool att_loop(const RobustForest<NX,NY>& rf, Row<NX>& inst, const Attacker<NX>& attacker, 
    const std::set<size_t>& features, int spent, const int budget, const size_t true_y, bool is_constant,
    double constant_deform)
{
    if (spent >= budget)
        return false;
    for (auto& f : features)
    {
        double deform;
        if (is_constant)
            deform = constant_deform;
        else
            deform = attacker.get_deformation(f);
        inst[f] += deform;
        if (rf.predict(inst) != true_y)
            return true;
        std::set<size_t> new_features;
        if (spent < budget)
        {
            new_features = features;
            new_features.erase(f);
            bool res = att_loop<NX,NY>(rf, inst, attacker, new_features, spent+1, budget, true_y,
                is_constant, constant_deform);
            if (res)
                return true;
        }
        inst[f] -= 2*deform;
        if (rf.predict(inst) != true_y)
            return true;
        if (spent < budget)
        {
            bool res = att_loop<NX,NY>(rf, inst, attacker, new_features, spent+1, budget, true_y,
                is_constant, constant_deform);
            if (res)
                return true;
        }
        inst[f] += deform;
    }
    return false;
}

template<size_t NX, size_t NY>
double RobustForest<NX,NY>::get_attacked_score(const Attacker<NX>& attacker, const DF<NX>& X, const DF<NY>& Y) const
{
    if (!is_trained_)
        Util::die("forest is not trained");

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    DF<NX> splice_X;
    DF<NY> splice_Y;
    if (world_size == 1)
    {
        splice_X = X;
        splice_Y = Y;
    }
    else
    {
        splice_X = splice<NX>(X, world_rank, world_size);
        splice_Y = splice<NY>(Y, world_rank, world_size);
    }
    const size_t N = X.rows();
    const size_t N_splice = splice_X.rows();
    const int budget = attacker.get_budget();
    std::atomic<size_t> correct = 0;
    thread_pool pool;

    pool.parallelize_loop(0, N_splice, 
        [&](const size_t& low, const size_t& high){ // block [low, high)
            if (low >= high)
                return;
            const auto feats = attacker.target_features();
            const bool is_constant = attacker.is_constant();
            const double constant_deform = attacker.get_deformation();
            for (size_t i = low; i < high; i++)
            {
                Eigen::Index max_ind;
                splice_Y.row(i).maxCoeff(&max_ind);
                Row<NX> inst = splice_X.row(i);
                const auto true_y = static_cast<size_t>(max_ind);
                const auto pred_y = predict(inst);
                if (true_y == pred_y)
                {
                    correct++;
                    if (att_loop<NX,NY>(*this, inst, attacker, feats, 0, budget, true_y,
                        is_constant, constant_deform))
                        correct--;
                }
            }
        }
    );

    MPI_Barrier(MPI_COMM_WORLD);
    // Master
    if (world_rank == 0)
    {
        for (size_t i = 1; i < world_size; i++)
        {
            size_t other_correct = recv_ulong(i, 0);
            correct += other_correct;
        }
    }
    // Servant
    else
    {
        size_t my_correct = correct;
        send_ulong(my_correct, 0, 0);
    }
    // double ret = 100.0 - 100.0 * static_cast<double>(score) / static_cast<double>(total_attacks);
    MPI_Barrier(MPI_COMM_WORLD);
    double ret = 100.0 * static_cast<double>(correct) / static_cast<double>(N);
    return ret;
}

template<size_t NX, size_t NY>
double RobustForest<NX,NY>::get_own_attacked_score(const DF<NX>& X, const DF<NY>& Y) const
{
    const auto attacker = trees_[0].get_attacker();
    return get_attacked_score(*attacker, X, Y);
}

template<size_t NX, size_t NY>
void RobustForest<NX,NY>::set_attacker_budget(int budget)
{
    trees_[0].set_attacker_budget(budget);
}

template<size_t NX, size_t NY>
void RobustForest<NX,NY>::set_attacker_feats(const std::set<size_t>& feats)
{
    trees_[0].set_attacker_feats(feats);
}

template<size_t NX, size_t NY>
std::map<size_t, double> RobustForest<NX,NY>::feature_importance() const
{
    std::map<size_t, double> sum_map;
    for (size_t i = 0; i < n_trees_; i++)
    {
        auto import = trees_[i].feature_importance();
        for (const auto& [fid, gain] : import)
            sum_map[fid] += gain;
    }
    for (const auto& [fid, gain] : sum_map)
        sum_map[fid] = gain / static_cast<double>(n_trees_);
    return sum_map;
}

template<size_t NX, size_t NY>
std::set<size_t> RobustForest<NX,NY>::most_important_feats(int N) const
{
    auto feat_map = feature_importance();
    int n = 0;
    std::set<size_t> out;
    for (auto it = std::prev(feat_map.cend()); it != std::prev(feat_map.cbegin()); it = std::prev(it))
    {
        if (n >= N)
            break;
        auto fid = it->first;
        out.insert(fid);
        n++;
    }
    return out;
}