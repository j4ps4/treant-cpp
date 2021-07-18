#include <memory>
#include <vector>
#include <array>
#include <set>

#include "Node.h"
#include "../Attacker.h"
#include "SplitOptimizer.h"
#include "../DF2/DF_util.h"

template<size_t NX, size_t NY>
struct TreeArguments
{
    int id;
    std::unique_ptr<Attacker<NX>> attacker;
    SplitFunction fun;
    bool icml2019;
    size_t max_depth;
    size_t min_instances_per_node;
    bool affine;
};

template<size_t NX, size_t NY>
class RobustDecisionTree
{
public:
    RobustDecisionTree(TreeArguments<NX,NY>&& args) :
        id_(args.id), max_depth_(args.max_depth), min_instances_per_node_(args.min_instances_per_node),
        attacker_(std::move(args.attacker)), affine_(args.affine)
    {
        optimizer_ = std::make_unique<SplitOptimizer<NX,NY>>(args.fun, args.icml2019);
        isTrained_ = false;
    }

    bool is_affine() const {return affine_;}

    void fit(const DF<NX>& X_train, const DF<NY>& y_train);
    
private:
    Node* private_fit(const DF<NX>& X_train, const DF<NY>& y_train, const std::vector<size_t> rows,
        std::map<size_t,int>& costs, const Row<NY>& node_prediction, std::set<size_t> feature_blacklist, size_t depth);
    std::unique_ptr<Node> root_;
    int id_;
    size_t max_depth_;
    size_t min_instances_per_node_;
    bool isTrained_;
    bool affine_;
    std::set<size_t> start_feature_bl_;
    std::unique_ptr<Attacker<NX>> attacker_;
    std::unique_ptr<SplitOptimizer<NX,NY>> optimizer_;
};

#include "RobustDecisionTree.tpp"