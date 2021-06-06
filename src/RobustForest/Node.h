/** This class represents an individual node of each tree eventually composing the forest.
  Intuitively, each node contains a horizontal slice of the original matrix of data; in other words, it contains a subset X' of the rows of X.
  In addition to that, each node keep track of the feature_id along with the corresponding feature_value which lead to the **best splitting** of the data "located" at that node (i.e., X'). The best splitting is computed on the basis of a specific function utilized to generate the tree/forest.
  Furthermore, at each node we are able to tell what would be the prediction of the label/target value, on the basis of the subset of instances located at that node. More specifically, at each node we also keep track of the labels/target values associated with the instances located at that node. Prediction can be simply obtained by:
  -   take the most representative class label (i.e., majority voting) in case of classification;
  -   take the average of the target values in case of regression.

  Finally, each node contains a reference to its left and right child, respectively. A **leaf node** is a special node whose left and right childs are both equal to nullptr, as well as its best splitting feature id/value.
**/

#include <memory>
#include <array>
#include <algorithm>

#define NUM_TYPES double,float,uint32_t,int32_t,uint8_t,int8_t,bool

class Node
{
public:
    Node(size_t n_inst, std::unique_ptr<Node>&& left = nullptr, 
         std::unique_ptr<Node>&& right = nullptr, size_t best_split_id = -1, double best_split_val = 0) :
         n_inst_(n_inst), left_(std::move(left)), right_(std::move(right)), 
         best_split_id_(best_split_id), best_split_val_(best_split_val)
        {
            prediction_score_{0, 0};
            prediction_ = -1;
        }
    void set_prediction(std::array<double,2> pred)
    {
        prediction_score_ = pred;
        prediction_ = std::distance(prediction_score_.begin(),
                      std::max_element(prediction_score_.begin(), prediction_score_.end())
        );
    }
    std::array<double,2> get_prediction_score() const noexcept {return prediction_score_;}
    size_t get_prediction() const noexcept {return prediction_;}
    bool is_leaf() const noexcept
    {
        return (!left_ && !right_);
    }
    void set_left(std::unique_ptr<Node>&& ptr) {left_ = std::move(ptr);}
    void set_right(std::unique_ptr<Node>&& ptr) {right_ = std::move(ptr);}
    Node* left() const noexcept {return left_.get();}
    Node* right() const noexcept {return right_.get();}
private:
    size_t n_inst_; // number of instances
    std::unique_ptr<Node> left_; // left child
    std::unique_ptr<Node> right_; // right child
    size_t best_split_id_; // index of the feature associated with the best split of this node
    double best_split_val_; // value of the corresponding feature
    std::array<double, 2> prediction_score_;
    size_t prediction_;
};