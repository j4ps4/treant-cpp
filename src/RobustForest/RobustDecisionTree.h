#pragma once

#include <memory>
#include <vector>
#include <array>
#include <set>
#include <filesystem>

#include "Node.h"
#include "../Attacker.h"
#include "SplitOptimizer.h"
#include "Constraint.h"
#include "../DF2/DF_util.h"
#include "../thread_pool.hpp"

template<size_t NX, size_t NY>
struct TreeArguments
{
    int id;
    std::unique_ptr<Attacker<NX>> attacker;
    SplitFunction fun;
    TrainingAlgo algo;
    size_t max_depth;
    size_t min_instances_per_node;
    int maxiter;
    bool affine;
    std::set<size_t> feature_bl;
};

template<size_t NX, size_t NY>
class RobustDecisionTree
{
    using ConstrVec = std::vector<Constraint<NX,NY>>;
public:
    RobustDecisionTree(TreeArguments<NX,NY>&& args) :
        id_(args.id), max_depth_(args.max_depth), min_instances_per_node_(args.min_instances_per_node),
        attacker_(std::move(args.attacker)), affine_(args.affine), start_feature_bl_(args.feature_bl)
    {
        optimizer_ = std::make_unique<SplitOptimizer<NX,NY>>(args.fun, args.algo, args.maxiter);
        isTrained_ = false;
    }

    RobustDecisionTree(std::unique_ptr<Node<NY>>& root, int id, size_t max_depth,
        size_t min_instances_per_node, bool isTrained, bool affine)
        {
            root_.swap(root);
            id_ = id;
            max_depth_ = max_depth;
            min_instances_per_node_ = min_instances_per_node;
            isTrained_ = isTrained;
            affine_ = affine;
        }
    
    RobustDecisionTree() = default;

    bool is_affine() const {return affine_;}

    void fit(const DF<NX>& X_train, const DF<NY>& y_train);

    size_t predict(const Row<NX>& instance) const;
    DF<NY> predict_proba(const DF<NX>& X_test) const;

    double classification_error(const DF<NY>& Y_test, const DF<NY>& Y_pred) const;

    void dump_to_disk(const std::filesystem::path& fn) const;

    static RobustDecisionTree<NX, NY> load_from_disk(const std::filesystem::path& fn);
    
    std::string get_model_name() const;
    
    void print_test_score(const DF<NX>& X_test, const DF<NY>& Y_test, const DF<NY>& Y_train) const;

    double get_attacked_score(Attacker<NX>& attacker, const DF<NX>& X, const DF<NY>& Y) const;

    std::map<size_t, double> feature_importance() const;
    
private:
    Node<NY>* private_fit(thread_pool& pool, const DF<NX>& X_train, const DF<NY>& y_train, const std::vector<size_t> rows,
        std::map<int64_t,int>& costs, ConstrVec& constraints,
        const Row<NY>& node_prediction, std::set<size_t> feature_blacklist, size_t depth);

    size_t private_predict(const Row<NX>& instance, const Node<NY>* node) const;
    Row<NY> private_predict_proba(const Row<NX>& instance, const Node<NY>* node) const;

    void feature_importance_(const Node<NY>* node, std::map<size_t,double>& dict) const;
    
    std::unique_ptr<Node<NY>> root_;
    int id_;
    size_t max_depth_;
    size_t min_instances_per_node_;
    bool isTrained_;
    std::unique_ptr<Attacker<NX>> attacker_;
    bool affine_;
    std::set<size_t> start_feature_bl_;
    std::unique_ptr<SplitOptimizer<NX,NY>> optimizer_;
};

#include "RobustDecisionTree.tpp"
