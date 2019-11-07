#pragma once

#include <algorithm>
#include <memory>
#include <vector>

namespace BPT
{
  template<typename T>
  class BPTNode {
    public:
      BPTNode(const unsigned int degree, BPTNode<T> *parent = nullptr) : parent_(parent), degree_(degree), isLeaf_(true) {}
      BPTNode(const BPTNode<T>& n) : degree_(n.GetDegree()) {
        isLeaf_ = n.IsLeaf();
        parent_ = n.GetParent();
        children_ = n.GetChildren();
        keys_ = n.GetKeys();
      }

      std::vector< std::pair<uint64_t, BPTNode<T>*> > GetChildren() const {
        return children_;
      }
      
      std::vector< std::pair<uint64_t, T> > GetKeys() const {
        return keys_;
      }

      BPTNode* GetParent() const {
        return parent_;
      }

      unsigned int GetDegree() const {
        return degree_;
      }

      bool IsLeaf() const {
        return isLeaf_;
      }

      std::pair<uint64_t, BPTNode<T>*> &operator[](int index) {
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
            isLeaf_ = false;
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
        BPTNode<T> newNode(degree_, parent_ ? parent_ : this);
        int s = keys_.size();

        for (int i(s) ; i > s / 2; i--) {
          newNode.AddItem(keys_.back().first, keys_.back().second);
          keys_.pop_back();
        }

        if (parent_ != nullptr) {
          parent_->AddNode(new BPTNode<T>(newNode));
        } else {
          BPTNode<T> oldNode(degree_, this);

          while(!keys_.empty()) {
            oldNode.AddItem(keys_.back().first, keys_.back().second);
            keys_.pop_back();
          }

          keys_.clear();
          children_.push_back(std::make_pair(oldNode.GetThisItem(0), new BPTNode<T>(oldNode)));
          children_.push_back(std::make_pair(newNode.GetThisItem(0), new BPTNode<T>(newNode)));
        }
      }

      void AddNode(BPTNode<T> *n) {
        children_.push_back(std::make_pair((*n)[0].first, n));
        std::sort(children_.begin(), children_.end(), CompareChildren);

        if (children_.size() > degree_) {
          SplitNode();
        }
      }

      void SplitNode() {
        BPTNode<T> newNode(degree_, parent_ ? parent_ : this);
        int s = children_.size();

        for (int i(s) ; i > s / 2; i--) {
          newNode.AddNode(children_.back().second);
          children_.pop_back();
        }

        if (parent_ != nullptr) {
          parent_->AddNode(new BPTNode<T>(newNode));
        } else {
          BPTNode<T> oldNode(degree_, this);

          for (int i(s/2) ; i >= 0; i--) {
            oldNode.AddNode(children_.back().second);
            children_.pop_back();
          }

          children_.clear();
          children_.push_back(std::make_pair(oldNode[0].first, new BPTNode<T>(oldNode)));
          children_.push_back(std::make_pair(newNode[0].first, new BPTNode<T>(newNode)));
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

      static bool CompareChildren(std::pair<uint64_t, BPTNode<T>*> t1, std::pair<uint64_t, BPTNode<T> *> t2) {
        return t1.first < t2.first;
      }

    private:
      BPTNode<T> *parent_;
      std::vector< std::pair<uint64_t, BPTNode<T>*> > children_;
      std::vector< std::pair<uint64_t, T> > keys_;
      const unsigned int degree_;
      bool isLeaf_;
  };
} // namespace BPT