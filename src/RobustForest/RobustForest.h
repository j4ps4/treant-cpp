#pragma once

#include "RobustDecisionTree.h"

enum class ForestType
{
    Forest, // all trees have the same arguments
    Bundle, // different attackers
    Fold // different tree arguments
};

template<size_t NX, size_t NY>
class RobustForest
{
private:
    RobustForest(size_t n_trees, std::vector<RobustDecisionTree<NX,NY>>&& trees,
        bool is_trained, ForestType type) :
        n_trees_(n_trees), trees_(std::move(trees)), is_trained_(is_trained), type_(type) {}
public:
	RobustForest(size_t N, TreeArguments<NX,NY>&& args);

	RobustForest(size_t N, TreeArguments<NX,NY>&& args, 
        const std::vector<std::tuple<int,Attacker<NX>*>>& atkrs);
    
    RobustForest(const std::vector<TreeArguments<NX,NY>>& args, size_t N_folds, const std::string& log);

    RobustForest() = default;
	
    void fit(const DF<NX>& X_train, const DF<NY>& y_train);

    size_t predict(const Row<NX>& instance) const;
    size_t predict(const std::vector<double>& instance) const;
    Row<NY> predict_proba_row(const Row<NX>& instance) const;
    Row<NY> predict_proba_row(const std::vector<double>& instance) const;
    DF<NY> predict_proba(const DF<NX>& X_test) const;

    static double classification_error(const DF<NY>& Y_test, const DF<NY>& Y_pred);

    void dump_to_disk(const std::filesystem::path& fn) const;

    static RobustForest<NX, NY> load_from_disk(const std::filesystem::path& fn);
    
    std::string get_model_name() const;

    ForestType get_type() const {return type_;}
    
    size_t get_N() const {return n_trees_;}
    
    void print_test_score(const DF<NX>& X_test, const DF<NY>& Y_test, const DF<NY>& Y_train, bool valid) const;

    double get_attacked_score(const Attacker<NX>& attacker, const DF<NX>& X, const DF<NY>& Y) const;
    double get_own_attacked_score(const DF<NX>& X, const DF<NY>& Y) const;

    void set_attacker_budget(int budget);
    void set_attacker_feats(const std::set<size_t>& feats);

    std::map<size_t, double> feature_importance() const;

    std::set<size_t> most_important_feats(int N) const;

    std::tuple<Row<NX>, double> blackbox_attack(const DF<NX>& X_train, const DF<NY>& Y_train,
        const DF<NX>& X_test, const DF<NY>& Y_test, size_t index, bool quiet, size_t num_samples = 1000,
        double alpha = 2.0, double beta = 0.005) const;

private:
    std::tuple<double, int> fine_grained_binary_search(const Row<NX>& x0, const size_t y0, const Row<NX>& theta,
        double initial_lbd, double current_best) const;

    std::tuple<double, int> fine_grained_binary_search_local(const Row<NX>& x0, const size_t y0,
        const Row<NX>& theta, double initial_lbd, double tol) const;

	TreeArguments<NX,NY> tree_args_;
	size_t n_trees_;
	std::vector<RobustDecisionTree<NX,NY>> trees_;
	bool is_trained_;
    ForestType type_;
    size_t N_folds_;
    std::string logfile_;
};

#include "RobustForest.tpp"
