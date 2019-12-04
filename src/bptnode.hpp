#pragma once

#include <algorithm>
#include <list>
#include <memory>
#include <vector>

namespace BPT
{
  template<typename T>
  class BPTNode {
    public:
      template<typename U>
      using BPTContainer = std::list< std::pair<uint64_t, U> >;
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

      BPTContainer<BPTNode<T>*> GetChildren() const {
        return children_;
      }
      
      BPTContainer<T> GetKeys() const {
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

        for (auto it(keys_.begin()) ; it != keys_.end() ; ++it) {
          if (hash == it->first) {
            T& ret = it->second;
            return ret;
          }
        }
        throw std::out_of_range("Item not found");

        return keys_.begin()->second;
      }

      T& GetThisItem(const int index) {
        if ((unsigned)index >= keys_.size())
          throw std::out_of_range("Index out of range");
        return std::next(keys_.begin(), index)->second;
      }

      uint64_t GetKeyHash(const int index) {
        if ((unsigned)index >= keys_.size())
          throw std::out_of_range("Index out of range");
        return std::next(keys_.begin(), index)->first;
      }

      BPTNode<T>* GetChild(const uint64_t& hash) {
        if (!isLeaf_)
          throw std::logic_error("Not a leaf node");

        for (auto it(children_.begin()) ; it != children_.end() ; ++it) {
          if (hash == it->first) {
            BPTNode<T>* ret = it->second;
            return ret;
          }
        }
        throw std::out_of_range("Item not found");

        return children_.begin()->second;
      }

      BPTNode<T>* GetThisChild(const int index) {
        if ((unsigned)index >= children_.size())
          throw std::out_of_range("Index out of range");
        return std::next(children_.begin(), index)->second;
      }

      uint64_t GetChildHash(const int index) {
        if ((unsigned)index >= children_.size())
          throw std::out_of_range("Index out of range");
        return std::next(children_.begin(), index)->first;
      }

      void AddItem(uint64_t hash, T item) {
        if (isLeaf_) {
          uint64_t firstHash = keys_.front().first;
          SortedInsertKeys(hash, item);

          if (hash < firstHash && parent_ != nullptr) {
            parent_->UpdateKeysFromChild(firstHash, hash);
          }

          if (keys_.size() >= degree_) {
            Split();
            if (keys_.empty()) {
              isLeaf_ = false;
            } else {
              isLeaf_ = true;
            }
          }

        } else {
          for (auto it(std::next(children_.begin())) ; it != children_.end() ; ++it) {
            if (hash < it->first) {
              BPTNode<T> *node = std::prev(it)->second;
              node->AddItem(hash, item);
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
          h = n->GetKeyHash(0);
        else
          h = n->GetChildHash(0);

        SortedInsertChildren(h, n);

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
            p = children_.front().second->GetParent();
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
        bool ret = false;
        if (isLeaf_) {
          for (auto it(keys_.begin()) ; it != keys_.end() ; ++it) {
            if (it->first == item) {
              it = keys_.erase(it);
              return true;
            }
          }
        } else {
          for (auto it(std::next(children_.begin())) ; it != children_.end() ; ++it) {
            if (item < it->first) {
              --it;
              BPTNode<T> *node = it->second;
              uint64_t t = it->first;

              ret = node->DeleteItem(item);

              if ((node->IsLeaf() && !node->HasEnoughKey())
                 || (!node->IsLeaf() && !node->HasEnoughChildren())) {
                RemoveChild(t);
              }

              return ret;
            }
          }
          BPTNode<T> *backNode = children_.back().second;
          ret = backNode->DeleteItem(item);
          if ((backNode->IsLeaf() && !backNode->HasEnoughKey())
            || (!backNode->IsLeaf() && !backNode->HasEnoughChildren())) {
            RemoveChild(children_.back().first);
          }
        }
        

        return ret;
      }

      bool RemoveChild(const uint64_t& hash, const bool deleteChildrenItems = false) {
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

      void DeepCopyFrom(BPTNode<T> *n) {
        isLeaf_ = n->IsLeaf();
        if (n->IsLeaf()) {
          for (std::pair<uint64_t, T> k : n->GetKeys()) {
            AddItem(k.first, k.second);
          }
        } else {
          for (std::pair<uint64_t, BPTNode<T>*> c : n->GetChildren()) {
            children_.push_back(std::make_pair(c.first, new BPTNode<T>(degree_, this)));
            children_.back().second->DeepCopyFrom(c.second);
          }
        }
      }

    public:
      static bool CompareKeys(const std::pair<uint64_t, T>& t1, const std::pair<uint64_t, T>& t2) {
        return t1.first < t2.first;
      }

      static bool CompareChildren(const std::pair<uint64_t, BPTNode<T>* >& t1, const std::pair<uint64_t, BPTNode<T>* >& t2) {
        return t1.first < t2.first;
      }

    private:
      bool UpdateKeysFromChild(const uint64_t& oldKey, const uint64_t newKey) {
        if (!isLeaf_) {
          for (auto it(children_.begin()) ; it != children_.end() ; ++it) {
            if (it->first == oldKey) {
              bool updateParent = (parent_ != nullptr && it == children_.begin());

              it->first = newKey;
              if (updateParent) {
                return parent_->UpdateKeysFromChild(oldKey, newKey);
              }
              return true;
            }
          }
        }
        return false;
      }

      void SortedInsertKeys(uint64_t hash, T item) {
        for (auto it(keys_.begin()) ; it != keys_.end() ; ++it) {
          if (hash < it->first) {
            keys_.insert(it, std::make_pair(hash, item));
            return;
          }
        }
        keys_.push_back(std::make_pair(hash, item));
      }

      void SortedInsertChildren(uint64_t hash, BPTNode<T> *n) {
        for (auto it(children_.begin()) ; it != children_.end() ; ++it) {
          if (hash < it->first) {
            children_.insert(it, std::make_pair(hash, n));
            return;
          }
        }
        children_.push_back(std::make_pair(hash, n));
      }

    private:
      BPTNode<T>* parent_;
      BPTContainer<BPTNode<T>*> children_;
      BPTContainer<T> keys_;
      const unsigned int degree_;
      bool isLeaf_;
  };
} // namespace BPT