#pragma once

#include <algorithm>
#include <memory>
#include <vector>

namespace BPT
{
  template<typename T>
  class BPTNode {
    public:
      BPTNode(const unsigned int degree, std::shared_ptr<BPTNode<T>> parent = nullptr) : parent_(parent), degree_(degree), isLeaf_(true) {}
      BPTNode(const BPTNode<T>& n) : degree_(n.GetDegree()) {
        isLeaf_ = n.IsLeaf();
        parent_ = n.GetParent();
        children_ = n.GetChildren();
        keys_ = n.GetKeys();
      }

      std::vector< std::pair<uint64_t, std::shared_ptr<BPTNode<T>>> > GetChildren() const {
        return children_;
      }
      
      std::vector< std::pair<uint64_t, T> > GetKeys() const {
        return keys_;
      }

      std::shared_ptr<BPTNode> GetParent() const {
        return parent_;
      }

      unsigned int GetDegree() const {
        return degree_;
      }

      bool IsLeaf() const {
        return isLeaf_;
      }

      std::pair<uint64_t, std::shared_ptr<BPTNode<T>>> &operator[](int index) {
        return children_.at(index);
      }

      int Length() const {
        if (isLeaf_)
          return keys_.size();
        return children_.size();
      }

      T& GetItem(const uint64_t& hash) {
        if (!isLeaf_)
          throw std::logic_error("Not a leaf node");
        
        for (int i(0) ; i < keys_.size() ; i++) {
          if (hash == keys_[i].first) {
            return keys_[i].second;
          }
        }

        throw std::out_of_range("Item not found");

        return keys_[0].second;
      }

      T& GetThisItem(const int index) {
        return keys_.at(index).second;
      }

      void AddItem(uint64_t hash, T item) {
        if (isLeaf_) {
          keys_.push_back(std::make_pair(hash, item));
          std::sort(keys_.begin(), keys_.end(), CompareKeys);

          if (keys_.size() > degree_) {
            Split();
          }
        } else {
          for(int i(1) ; i < children_.size() ; i++) {
            if (hash <= children_[i].first) {
              children_[i-1].second->AddItem(hash, item);
              return;
            }
          }
          children_.back().second->AddItem(hash, item);
        }
      }

      void Split() {
        BPTNode<T> newNode(degree_, parent_ ? parent_ : std::make_shared<BPTNode<T>>(*this));
        int s = keys_.size();

        for (int i(s) ; i > s / 2; i--) {
          newNode.AddItem(keys_.back().first, keys_.back().second);
          keys_.pop_back();
        }

        if (parent_ != nullptr) {
          parent_->AddNode(std::make_shared<BPTNode<T>>(newNode));
        } else {
          BPTNode<T> oldNode(degree_, std::make_shared<BPTNode<T>>(*this));

          while(!keys_.empty()) {
            oldNode.AddItem(keys_.back().first, keys_.back().second);
            keys_.pop_back();
          }

          keys_.clear();
          children_.push_back(std::make_pair(oldNode.GetThisItem(0), std::make_shared<BPTNode<T>>(oldNode)));
          children_.push_back(std::make_pair(newNode.GetThisItem(0), std::make_shared<BPTNode<T>>(newNode)));
          isLeaf_ = false;
        }
      }

      void AddNode(std::shared_ptr<BPTNode<T>> n) {
        uint64_t h = 0;
        isLeaf_ = false;
        if (n->IsLeaf())
          h = n->GetKeys()[0].first;
        else
          h = (*n)[0].first;
        children_.push_back(std::make_pair(h, n));
        std::sort(children_.begin(), children_.end(), CompareChildren);

        if (children_.size() > degree_) {
          SplitNode();
        }
      }

      void SplitNode() {
        BPTNode<T> newNode(degree_, parent_ ? parent_ : std::make_shared<BPTNode<T>>(*this));
        int s = children_.size();

        for (int i(s) ; i > s / 2; i--) {
          newNode.AddNode(children_.back().second);
          children_.pop_back();
        }

        if (parent_ != nullptr) {
          parent_->AddNode(std::make_shared<BPTNode<T> >(newNode));
        } else {
          BPTNode<T> oldNode(degree_, std::make_shared<BPTNode<T>>(*this));

          for (int i(s/2) ; i >= 0; i--) {
            oldNode.AddNode(children_.back().second);
            children_.pop_back();
          }

          children_.clear();
          children_.push_back(std::make_pair(oldNode[0].first, std::make_shared<BPTNode<T> >(oldNode)));
          children_.push_back(std::make_pair(newNode[0].first, std::make_shared<BPTNode<T> >(newNode)));
        }
      }

      bool DeleteItem(uint64_t item) {
        // TODO
        return true;
      } 

    public:
      static bool CompareKeys(std::pair<uint64_t, T> t1, std::pair<uint64_t, T> t2) {
        return t1.first < t2.first;
      }

      static bool CompareChildren(std::pair<uint64_t, std::shared_ptr<BPTNode<T>> > t1, std::pair<uint64_t, std::shared_ptr<BPTNode<T>> > t2) {
        return t1.first < t2.first;
      }

    private:
      std::shared_ptr<BPTNode<T>> parent_;
      std::vector< std::pair<uint64_t, std::shared_ptr<BPTNode<T>> > > children_;
      std::vector< std::pair<uint64_t, T> > keys_;
      const unsigned int degree_;
      bool isLeaf_;
  };
} // namespace BPT