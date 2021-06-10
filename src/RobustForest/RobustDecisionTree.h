#include "Node.h"

class RobustDecisionTree
{
public:
    RobustDecisionTree(int id, size_t max_depth, size_t min_inst_per_node, float max_features,
                       size_t N_classes) :
        id_(id), max_depth_(max_depth), min_inst_per_node_(min_inst_per_node),
        max_features_(max_features), N_classes_(N_classes)
    {
        isTrained_ = false;
    }
    
private:
    Node root_;
    int id_;
    size_t max_depth_;
    size_t min_inst_per_node_;
    float max_features_; // proportion of instances sampled without replacement
    bool isTrained_;
    const size_t N_classes_;
};