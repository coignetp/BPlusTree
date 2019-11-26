#pragma once

#include <algorithm>
#include <memory>
#include <vector>

namespace BPT
{
  template<typename T>
  class BPTNode {
    public:
      BPTNode(const unsigned int degree, BPTNode<T>* parent = nullptr) : parent_(parent), degree_(degree), isLeaf_(true) {}
      BPTNode(const BPTNode<T>& n) : degree_(n.GetDegree()) {
        isLeaf_ = n.IsLeaf();
        parent_ = n.GetParent();
        children_ = n.GetChildren();
        keys_ = n.GetKeys();
      }

      ~BPTNode() {
        for (std::pair<uint64_t, BPTNode<T>* > p : children_) {
          delete p.second;
        }
      }

      std::vector< std::pair<uint64_t, BPTNode<T>*> > GetChildren() const {
        return children_;
      }
      
      std::vector< std::pair<uint64_t, T> > GetKeys() const {
        return keys_;
      }

      BPTNode<T>* GetParent() const {
        return parent_;
      }

      void SetParent(BPTNode<T>* parent) {
        parent_ = parent;
      }

      unsigned int GetDegree() const {
        return degree_;
      }

      bool IsLeaf() const {
        return isLeaf_;
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

      uint64_t GetKeyHash(const int index) {
        return keys_.at(index).first;
      }

      BPTNode<T>* GetChild(const uint64_t& hash) {
        if (!isLeaf_)
          throw std::logic_error("Not a leaf node");
        
        for (int i(0) ; i < children_.size() ; i++) {
          if (hash == children_[i].first) {
            return children_[i].second;
          }
        }

        throw std::out_of_range("Children not found");

        return children_[0].second;
      }

      BPTNode<T>* GetThisChild(const int index) {
        return children_.at(index).second;
      }

      uint64_t GetChildHash(const int index) {
        return children_.at(index).first;
      }

      void AddItem(uint64_t hash, T item) {
        if (isLeaf_) {
          keys_.push_back(std::make_pair(hash, item));
          uint64_t firstHash = keys_.front().first;

          if (hash < firstHash && parent_ != nullptr) {
            parent_->UpdateKeysFromChild(firstHash, hash);
          }

          std::sort(keys_.begin(), keys_.end(), CompareKeys);

          if (keys_.size() >= degree_) {
            Split();
            if (keys_.empty()) {
              isLeaf_ = false;
            } else {
              isLeaf_ = true;
            }
          }

        } else {
          for(int i(1) ; i < children_.size() ; i++) {
            if (hash < children_[i].first) {
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
          BPTNode<T> oldNode(degree_, newNode.GetParent());

          while(!keys_.empty()) {
            oldNode.AddItem(keys_.back().first, keys_.back().second);
            keys_.pop_back();
          }

          keys_.clear();
          children_.push_back(std::make_pair(oldNode.GetThisItem(0), new BPTNode<T>(oldNode)));
          children_.push_back(std::make_pair(newNode.GetThisItem(0), new BPTNode<T>(newNode)));
        }
      }

      void AddNode(BPTNode<T>* n) {
        uint64_t h = 0;
        isLeaf_ = false;
        if (n->IsLeaf())
          h = n->GetKeys()[0].first;
        else
          h = n->GetChildHash(0);
        children_.push_back(std::make_pair(h, n));
        std::sort(children_.begin(), children_.end(), CompareChildren);

        if (children_.size() > degree_) {
          SplitNode();
        }
      }

      void SplitNode() {
        BPTNode<T>* p(parent_);
        if (!parent_) {
          if (children_.empty()) {
            p = this;
          } else {
            p = children_[0].second->GetParent();
          }
        }
        BPTNode<T>* newNode = new BPTNode<T> (degree_, p);
        int s = children_.size();

        for (int i(s) ; i > s / 2; i--) {
          children_.back().second->SetParent(newNode);
          newNode->AddNode(children_.back().second);
          children_.pop_back();
        }

        if (parent_ != nullptr) {
          parent_->AddNode(newNode);
        } else {
          BPTNode<T>* oldNode = new BPTNode<T>(degree_, newNode->GetParent());

          while(!children_.empty()) {
            children_.back().second->SetParent(oldNode);
            oldNode->AddNode(children_.back().second);
            children_.pop_back();
          }

          children_.clear();
          children_.push_back(std::make_pair(oldNode->GetChildHash(0), oldNode));
          children_.push_back(std::make_pair(newNode->GetChildHash(0), newNode));
        }
      }

      bool DeleteItem(const uint64_t& item) {
        uint64_t id(keys_.front().first);
        bool ret = false;
        if (isLeaf_) {
          for (auto it(keys_.begin()) ; it != keys_.end() ; ++it) {
            if (it->first == item) {
              it = keys_.erase(it);
              return true;
            }
          }
        } else {
          for(int i(1) ; i < children_.size() ; i++) {
            if (item < children_[i].first) {
              ret = children_[i-1].second->DeleteItem(item);

              if (
                  (children_[i-1].second->IsLeaf()
                  && !children_[i-1].second->HasEnoughKey()) ||
                  (!children_[i-1].second->IsLeaf()
                  && !children_[i-1].second->HasEnoughChildren())
                ) {
                  RemoveChild(children_[i-1].first);
              }

              return ret;
            }
          }
          ret = children_.back().second->DeleteItem(item);
        }

        return ret;
      }

      bool RemoveChild(const uint64_t& hash, const bool deleteChildrenItems = false) {
        uint64_t id(children_.front().first);
        for (auto it(children_.begin()) ; it != children_.end() ; it++) {
          if (it->first == hash) {
            BPTNode<T>* child = it->second;

            if (it == children_.begin() && parent_ != nullptr) {
              parent_->UpdateKeysFromChild(hash, std::next(children_.begin())->first);
            }
            it = children_.erase(it);

            if (!deleteChildrenItems) {
              if (child->isLeaf_) {
                for (auto k : child->GetKeys()) {
                  AddItem(k.first, k.second);
                }
              } else {
                for (auto c : child->GetChildren()) {
                  AddNode(c.second);
                }
              }
            }
            delete child;
            return true;
          }
        }
        return false;
      }

      bool HasEnoughKey() const {
        return (isLeaf_ && keys_.size() >= degree_ / 2) || parent_ == nullptr;
      }

      bool HasEnoughChildren() const {
        return (!isLeaf_ && children_.size() >= degree_ / 2) || parent_ == nullptr;
      }

    public:
      static bool CompareKeys(std::pair<uint64_t, T> t1, std::pair<uint64_t, T> t2) {
        return t1.first < t2.first;
      }

      static bool CompareChildren(std::pair<uint64_t, BPTNode<T>* > t1, std::pair<uint64_t, BPTNode<T>* > t2) {
        return t1.first < t2.first;
      }

    private:
      bool UpdateKeysFromChild(const uint64_t& oldKey, const uint64_t newKey) {
        if (!isLeaf_) {
          for (int i(0) ; i < children_.size() ; i++) {
            if (children_[i].first == oldKey) {
              children_[i].first = newKey;
              if (i == 0 && parent_ != nullptr) {
                return parent_->UpdateKeysFromChild(oldKey, newKey);
              }
              return true;
            }
          }
        }
        return false;
      }

    private:
      BPTNode<T>* parent_;
      std::vector< std::pair<uint64_t, BPTNode<T>* > > children_;
      std::vector< std::pair<uint64_t, T> > keys_;
      const unsigned int degree_;
      bool isLeaf_;
  };
} // namespace BPT