#pragma once

#include <memory>
#include <vector>
#include <array>
#include <set>
#include <filesystem>
#include <random>

#include "Node.h"
#include "../Attacker.h"
#include "SplitOptimizer.h"
#include "Constraint.h"
#include "../DF2/DF_util.h"

template<size_t NX, size_t NY>
struct TreeArguments
{
    std::shared_ptr<Attacker<NX>> attacker;
    std::shared_ptr<SplitOptimizer<NX,NY>> optimizer;
    std::set<size_t> feature_bl;
    int id;
    size_t max_depth;
    size_t min_instances_per_node;
    bool affine;
    bool useParallel;
	double par_par;
    bool bootstrap_samples;
    bool bootstrap_features;
    bool replace_samples;
    bool replace_features;
    double max_samples;
    double max_features;
};

template<size_t NX, size_t NY>
class RobustDecisionTree
{
    using ConstrVec = std::vector<Constraint<NX,NY>>;
public:
    RobustDecisionTree(TreeArguments<NX,NY>&& args, uint64_t seed = 0) :
        attacker_(args.attacker), optimizer_(args.optimizer), start_feature_bl_(args.feature_bl),
        id_(args.id), max_depth_(args.max_depth),
        min_instances_per_node_(args.min_instances_per_node), affine_(args.affine), 
        useParallel_(args.useParallel), par_par_(args.par_par), bootstrap_samples_(args.bootstrap_samples),
        bootstrap_features_(args.bootstrap_features), replace_samples_(args.replace_samples),
        replace_features_(args.replace_features), max_samples_(args.max_samples),
        max_features_(args.max_features), rd_(seed), isTrained_(false)
    {
    }

    RobustDecisionTree(std::unique_ptr<Node<NY>>& root, int id, size_t max_depth,
        size_t min_instances_per_node, bool isTrained, bool affine, 
        std::shared_ptr<Attacker<NX>>& attacker) :
            attacker_(attacker),
            id_(id),
            max_depth_(max_depth),
            min_instances_per_node_(min_instances_per_node),
            isTrained_(isTrained),
            affine_(affine)
        {
            root_.swap(root);
        }
    
    RobustDecisionTree() = default;

    bool is_affine() const {return affine_;}

    void fit(const DF<NX>& X_train, const DF<NY>& y_train);

    size_t predict(const Row<NX>& instance) const;
    size_t predict(const std::vector<double>& instance) const;
    DF<NY> predict_proba(const DF<NX>& X_test) const;
    Row<NY> predict_proba_single(const Row<NX>& instance) const
    {
        return predict_proba_(instance, root_.get());
    }

    double classification_error(const DF<NY>& Y_test, const DF<NY>& Y_pred) const;

    void dump_to_disk(const std::filesystem::path& fn) const;

    static RobustDecisionTree<NX, NY> load_from_disk(const std::filesystem::path& fn);

    int get_id() const noexcept {return id_;}
    
    std::string get_model_name() const;
    
    void print_test_score(const DF<NX>& X_test, const DF<NY>& Y_test, const DF<NY>& Y_train) const;

    double get_attacked_score(const Attacker<NX>& attacker, const DF<NX>& X, const DF<NY>& Y) const;
    double get_own_attacked_score(const DF<NX>& X, const DF<NY>& Y) const;

    int get_attacker_budget() const {return attacker_->get_budget();}
    
    const Attacker<NX>* get_attacker() const {return attacker_.get();}

    void set_attacker_budget(int budget) 
    {
        if (!attacker_)
            Util::die("attacker = NULL!!!");
        attacker_->set_budget(budget);
    }

    std::map<size_t, double> feature_importance() const;

    template<typename Archive>
    void serialize(Archive& archive)
    {
        archive(root_, attacker_, id_, isTrained_);
    }

private:
    Node<NY>* fit_(const DF<NX>& X_train, const DF<NY>& y_train, size_t spawn_thresh, const std::vector<size_t> rows,
        std::map<int64_t,int> costs, ConstrVec constraints,
        const Row<NY>& node_prediction, std::set<size_t> feature_blacklist, size_t depth);

    size_t predict_(const Row<NX>& instance, const Node<NY>* node) const;
    Row<NY> predict_proba_(const Row<NX>& instance, const Node<NY>* node) const;

    void feature_importance_(const Node<NY>* node, std::map<size_t,double>& dict) const;
    
    std::unique_ptr<Node<NY>> root_;
    std::shared_ptr<Attacker<NX>> attacker_;
    std::shared_ptr<SplitOptimizer<NX,NY>> optimizer_;
    std::set<size_t> start_feature_bl_;
    std::mt19937_64 rd_;
    int id_;
    size_t max_depth_;
    size_t min_instances_per_node_;
    bool isTrained_;
    bool affine_;
    bool useParallel_;
    double par_par_;
    bool bootstrap_samples_;
    bool bootstrap_features_;
    bool replace_samples_;
    bool replace_features_;
    double max_samples_;
    double max_features_;
};

#include "RobustDecisionTree.tpp"
