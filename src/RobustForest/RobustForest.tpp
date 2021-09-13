#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "../thread_pool.hpp"

#include "../util.h"

template<size_t NX, size_t NY>
RobustForest<NX,NY>::RobustForest(size_t N, TreeArguments<NX,NY>&& args) :
	tree_args_(std::move(args)), n_trees_(N), is_trained_(false), type_(ForestType::Forest)
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
	tree_args_(std::move(args)), n_trees_(N), is_trained_(false), type_(ForestType::Bundle)
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
void RobustForest<NX,NY>::fit(const DF<NX>& X_train, const DF<NY>& y_train)
{
	thread_pool pool;
	Util::info("spawning a pool of {} threads...", pool.get_thread_count());

	for (size_t i = 0; i < n_trees_; i++)
	{
		auto& tree = trees_.at(i);
		pool.push_task([&]{
			tree.fit(X_train, y_train);
		});
	}
	pool.wait_for_tasks();
	is_trained_ = true;
	Util::log<3>("{} trees have been fit!", n_trees_);
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
        if (type_ == ForestType::Bundle)
            Util::warn("predict: forest is bundle, calculating mode doesn't make sense");

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
        if (type_ == ForestType::Bundle)
            Util::warn("predict_proba: forest is bundle, calculating mean doesn't make sense");

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
	const DF<NY>& Y_train) const
{
	if (type_ == ForestType::Bundle)
	{
		for (const auto& t : trees_)
		{
			auto Y_pred = t.predict_proba(X_test);
			auto test_acc = 100.0 - 100.0 * t.classification_error(Y_test, Y_pred);
			auto train_dom = dominant_class<NY>(Y_train);
			auto dummy_score = 100.0 * class_proportion<NY>(Y_test, train_dom);
			fmt::print("tree {}: test score: {:.2f}% (dummy classifier: {:.2f}%)\n", t.get_id(), test_acc, dummy_score);
		}
	}
	else
	{
		auto Y_pred = predict_proba(X_test);
		auto test_acc = 100.0 - 100.0 * classification_error(Y_test, Y_pred);
		auto train_dom = dominant_class<NY>(Y_train);
		auto dummy_score = 100.0 * class_proportion<NY>(Y_test, train_dom);
		fmt::print("test score: {:.2f}% (dummy classifier: {:.2f}%)\n", test_acc, dummy_score);
	}
}

template<size_t NX, size_t NY>
std::string RobustForest<NX,NY>::get_model_name() const
{
	std::string algo_str;
    if (!tree_args_.optimizer)
        return "null-forest";
    const auto algo = tree_args_.optimizer->get_algorithm();
    if (algo == TrainingAlgo::Icml2019)
		algo_str = fmt::format("ICML2019-E{}", tree_args_.optimizer->get_epsilon());
    else if (algo == TrainingAlgo::Robust)
    	algo_str = "Robust";
    else
        algo_str = "Standard";
	std::string extension;
	if (type_ == ForestType::Bundle)
		extension = "bundle";
	else
		extension = n_trees_ > 1 ? "forest" : "tree";
    if (algo == TrainingAlgo::Standard)
    	return fmt::format("{}-N{}-D{}.{}", algo_str, n_trees_, tree_args_.max_depth, extension);
    else if (algo == TrainingAlgo::Icml2019)
    	return fmt::format("{}-N{}-D{}.{}", algo_str, n_trees_, tree_args_.max_depth, extension);
    else
    {
	    int budget = trees_[0].get_attacker_budget();
    	return fmt::format("{}-N{}-B{}-D{}.{}", algo_str, n_trees_, budget, tree_args_.max_depth, extension);
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

template<size_t NX, size_t NY>
std::vector<double> RobustForest<NX,NY>::get_attacked_score(const Attacker<NX>& attacker, const DF<NX>& X, const DF<NY>& Y) const
{
    if (!is_trained_)
        Util::die("forest is not trained");

    std::vector<double> out;
    
    if (type_ == ForestType::Bundle)
    {
        for (size_t i = 0; i < n_trees_; i++)
        {
            out.push_back(trees_[i].get_attacked_score(attacker, X, Y));
        }
        return out;
    }

    const size_t N = X.rows();
    size_t correct = 0;
    // size_t attack_success = 0;
    // size_t total_attacks = 0;
    const auto& feats = attacker.target_features();
    for (int64_t i = 0; i < N; i++)
    {
        Eigen::Index max_ind;
        Y.row(i).maxCoeff(&max_ind);
        const auto& inst = X.row(i);
        const auto true_y = static_cast<size_t>(max_ind);
        const auto pred_y = predict(inst);
        if (true_y == pred_y)
        {
            correct++;
            RowSet<NX> set;
            att_recur<NX>(set, inst, attacker, feats, attacker.get_budget());
            for (auto& att : set)
            {
                const auto pred_att = predict(att);
                if (true_y != pred_att)
                {
                    correct--;
                    break;
                }
            }
        }
    }
    // double ret = 100.0 - 100.0 * static_cast<double>(score) / static_cast<double>(total_attacks);
    double ret = 100.0 * static_cast<double>(correct) / static_cast<double>(N);
    out.push_back(ret);
    return out;
}

template<size_t NX, size_t NY>
std::vector<double> RobustForest<NX,NY>::get_own_attacked_score(const DF<NX>& X, const DF<NY>& Y) const
{
    if (type_ == ForestType::Bundle)
    {
        std::vector<double> out;
        for (size_t i = 0; i < n_trees_; i++)
        {
            out.push_back(trees_[i].get_own_attacked_score(X, Y));
        }
        return out;
    }
    else
    {
        const auto& attacker = *trees_[0].get_attacker();
        return get_attacked_score(attacker, X, Y);
    }
}

template<size_t NX, size_t NY>
void RobustForest<NX,NY>::set_attacker_budget(int budget)
{
    if (type_ == ForestType::Bundle)
    {
        for (size_t i = 0; i < n_trees_; i++)
        {
            trees_[i].set_attacker_budget(budget);
        }
    }
    else
    {
        trees_[0].set_attacker_budget(budget);
    }

}

template<size_t NX, size_t NY>
std::map<size_t, double> RobustForest<NX,NY>::feature_importance(size_t tree_id) const
{
    if (type_ == ForestType::Bundle)
        return trees_[tree_id].feature_importance();
    else
    {
        std::map<size_t, double> sum_map;
        for (size_t i = 0; i < n_trees_; i++)
        {
            auto import = trees_[i].feature_importance();
            for (const auto& [fid, gain] : import)
                sum_map[fid] += gain;
        }
        return sum_map;
    }
}