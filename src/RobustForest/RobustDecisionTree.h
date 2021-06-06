#include "Node.h"

class RobustDecisionTree
{
    
private:
    Node root_;
    int id_;
    size_t max_depth_;
    size_t min_inst_per_node_;
    float max_features_; // proportion of instances sampled without replacement
};