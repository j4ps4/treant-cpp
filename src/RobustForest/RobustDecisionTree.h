#include <memory>
#include <vector>
#include <array>
#include <set>

#include "Node.h"
#include "../Attacker.h"
#include "SplitOptimizer.h"
#include "../DF2/DF_util.h"

template<size_t N>
struct TreeArguments
{
    int id;
    std::unique_ptr<Attacker<N>> attacker;
    std::unique_ptr<SplitOptimizer<N>> optimizer;
    size_t max_depth;
    size_t min_instances_per_node;
    bool affine;
};

template<size_t N>
class RobustDecisionTree
{
public:
    RobustDecisionTree(TreeArguments<N> args) :
        id_(args.id), max_depth_(args.max_depth), min_inst_per_node_(args.min_inst_per_node),
        attacker_(std::move(args.attacker)), optimizer_(std::move(args.optimizer)), affine_(args.affine)
    {
        isTrained_ = false;
    }

    bool is_affine() const {return affine_;}
    
private:
    Node* private_fit(const DF<N>& X_train, const DF<2>& y_train, const std::vector<size_t> rows,
        std::vector<int>& costs, const Row<2>& node_prediction, std::set<size_t> feature_blacklist, size_t depth);
    std::unique_ptr<Node> root_;
    int id_;
    size_t max_depth_;
    size_t min_instances_per_node_;
    bool isTrained_;
    bool affine_;
    std::unique_ptr<Attacker<N>> attacker_;
    std::unique_ptr<SplitOptimizer<N>> optimizer_;
};

#include "RobustDecisionTree.tpp"