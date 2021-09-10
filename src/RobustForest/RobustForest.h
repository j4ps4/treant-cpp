#pragma once

#include "RobustDecisionTree.h"

enum class ForestType
{
    Forest,
    Bundle
};

template<size_t NX, size_t NY>
class RobustForest
{
private:
    RobustForest(size_t n_trees, std::vector<RobustDecisionTree<NX,NY>> trees,
        bool is_trained, ForestType type) :
        n_trees_(n_trees), trees_(trees), is_trained_(is_trained), type_(type) {}
public:
	RobustForest(size_t N, TreeArguments<NX,NY>&& args);

	RobustForest(size_t N, TreeArguments<NX,NY>&& args, 
        const std::vector<std::tuple<int,Attacker<NX>*>>& atkrs);

    RobustForest() = default;
	
    void fit(const DF<NX>& X_train, const DF<NY>& y_train);

    size_t predict(const Row<NX>& instance) const;
    size_t predict(const std::vector<double>& instance) const;
    DF<NY> predict_proba(const DF<NX>& X_test) const;

    static double classification_error(const DF<NY>& Y_test, const DF<NY>& Y_pred);

    void dump_to_disk(const std::filesystem::path& fn) const;

    static RobustForest<NX, NY> load_from_disk(const std::filesystem::path& fn);
    
    std::string get_model_name() const;
    
    void print_test_score(const DF<NX>& X_test, const DF<NY>& Y_test, const DF<NY>& Y_train) const;

    double get_attacked_score(Attacker<NX>& attacker, const DF<NX>& X, const DF<NY>& Y) const;
    double get_own_attacked_score(const DF<NX>& X, const DF<NY>& Y) const;

    void set_attacker_budget(int budget);

    std::map<size_t, double> feature_importance() const;

private:
	TreeArguments<NX,NY> tree_args_;
	size_t n_trees_;
	std::vector<RobustDecisionTree<NX,NY>> trees_;
	bool is_trained_;
    ForestType type_;
};

#include "RobustForest.tpp"