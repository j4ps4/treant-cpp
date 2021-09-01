/** This class represents an individual node of each tree eventually composing the forest.
  Intuitively, each node contains a horizontal slice of the original matrix of data; in other words, it contains a subset X' of the rows of X.
  In addition to that, each node keep track of the feature_id along with the corresponding feature_value which lead to the **best splitting** of the data "located" at that node (i.e., X'). The best splitting is computed on the basis of a specific function utilized to generate the tree/forest.
  Furthermore, at each node we are able to tell what would be the prediction of the label/target value, on the basis of the subset of instances located at that node. More specifically, at each node we also keep track of the labels/target values associated with the instances located at that node. Prediction can be simply obtained by:
  -   take the most representative class label (i.e., majority voting) in case of classification;
  -   take the average of the target values in case of regression.

  Finally, each node contains a reference to its left and right child, respectively. A **leaf node** is a special node whose left and right childs are both equal to nullptr, as well as its best splitting feature id/value.
**/

#pragma once

#include <memory>
#include <array>
#include <algorithm>

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "../DF2/DF_util.h"

template<size_t NY>
class Node
{
public:
    Node(size_t n_inst, std::unique_ptr<Node<NY>> left = nullptr, 
         std::unique_ptr<Node<NY>> right = nullptr, size_t best_split_id = -1, double best_split_val = 0) :
         dummy_(false), n_inst_(n_inst), left_(std::move(left)), right_(std::move(right)), 
         best_split_id_(best_split_id), best_split_val_(best_split_val)
        {
            prediction_score_ = Eigen::ArrayXXd::Zero(1, NY);
            prediction_ = -1;
            loss_ = 0;
            gain_ = 0;
        }

    Node() : dummy_(true) {}

    bool is_dummy() const noexcept {return dummy_;}

    void set_prediction(const Row<NY>& pred)
    {
        prediction_score_ = pred;
        Eigen::Index max_ind;
        pred.maxCoeff(&max_ind);
        prediction_ = max_ind;
        // prediction_ = std::distance(prediction_score_.begin(),
        //               std::max_element(prediction_score_.begin(), prediction_score_.end())
        // );
    }
    Row<NY> get_prediction_score() const noexcept {return prediction_score_;}
    size_t get_prediction() const noexcept {return prediction_;}
    bool is_leaf() const noexcept
    {
        return (!left_ && !right_);
    }
    void set_left(Node<NY>* ptr) {left_.reset(ptr);}
    void set_right(Node<NY>* ptr) {right_.reset(ptr);}
    Node<NY>* left() const noexcept {return left_.get();}
    Node<NY>* right() const noexcept {return right_.get();}
    void set_loss(double loss) noexcept {loss_ = loss;}
    void set_gain(double gain) noexcept {gain_ = gain;}
    double get_gain() const noexcept {return gain_;}
    void set_best_split_id(size_t id) noexcept {best_split_id_ = id;}
    void set_best_split_value(double val) noexcept {best_split_val_ = val;}
    size_t get_best_split_id() const noexcept {return best_split_id_;}
    double get_best_split_value() const noexcept {return best_split_val_;}

    template<typename Archive>
    void save(Archive& archive) const
    {
        std::vector<double> temp;
        temp.reserve(NY);
        temp.insert(temp.end(), prediction_score_.data(), prediction_score_.data()+NY);
        archive(dummy_, n_inst_, left_, right_, best_split_id_,
                best_split_val_, temp, prediction_, loss_, gain_);
    }

    template<typename Archive>
    void load(Archive& archive)
    {
        std::vector<double> temp;
        archive(dummy_, n_inst_, left_, right_, best_split_id_,
                best_split_val_, temp, prediction_, loss_, gain_);
        for (size_t i = 0; i < temp.size(); i++)
            prediction_score_[i] = temp[i];
    }

private:
    bool dummy_;
    size_t n_inst_; // number of instances
    std::unique_ptr<Node<NY>> left_; // left child
    std::unique_ptr<Node<NY>> right_; // right child
    size_t best_split_id_; // index of the feature associated with the best split of this node
    double best_split_val_; // value of the corresponding feature
    Row<NY> prediction_score_;
    size_t prediction_;
    double loss_;
    double gain_;
};