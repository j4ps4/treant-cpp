#pragma once

#include "RobustDecisionTree.h"

template<size_t NX, size_t NY>
class RobustForest
{
public:
	RobustForest(size_t N, TreeArguments<NX,NY>&& args);

    RobustForest() = default;
	
    void fit(const DF<NX>& X_train, const DF<NY>& y_train);

    size_t predict(const Row<NX>& instance) const;
    size_t predict(const std::vector<double>& instance) const;
    DF<NY> predict_proba(const DF<NX>& X_test) const;

    double classification_error(const DF<NY>& Y_test, const DF<NY>& Y_pred) const;

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
};

#include "RobustForest.tpp"