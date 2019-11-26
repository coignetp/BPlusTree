#pragma once

#include <algorithm>
#include <map>
#include <memory>

#include <iostream>

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

      std::map<uint64_t, BPTNode<T>* > GetChildren() const {
        return children_;
      }
      
      std::map<uint64_t, T> GetKeys() const {
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
        
        auto it(keys_.find(hash));

        if (it == keys_.end())
          throw std::out_of_range("Item not found");

        return it->second;
      }

      T GetThisItem(const int index) {
        if (index >= keys_.size())
          throw std::out_of_range("Index out of range");
        return std::next(keys_.begin(), index)->second;
      }

      uint64_t GetThisItemHash(const int index) {
        if (index >= keys_.size())
          throw std::out_of_range("Index out of range");
        return std::next(keys_.begin(), index)->first;
      }

      BPTNode<T>* GetThisChild(const int index) {
        if (index >= children_.size())
          throw std::out_of_range("Index out of range");
        return std::next(children_.begin(), index)->second;
      }

      uint64_t GetThisChildrenHash(const int index) {
        if (index >= children_.size())
          throw std::out_of_range("Index out of range");
        return std::next(children_.begin(), index)->first;
      }

      void AddItem(uint64_t hash, T item) {
        if (isLeaf_) {
          uint64_t firstHash = (keys_.empty()) ? 0 : GetThisItemHash(0);
          std::cout << keys_.size() << std::endl;
          std::cout << firstHash << std::endl;
          keys_[hash] = item;

          if (hash < firstHash && parent_ != nullptr) {
            parent_->UpdateKeysFromChild(firstHash, hash);
          }

          // std::sort(keys_.begin(), keys_.end(), CompareKeys);

          if (keys_.size() >= degree_) {
            Split();
            if (keys_.empty()) {
              isLeaf_ = false;
            } else {
              isLeaf_ = true;
            }
          }

        } else {
          // for(int i(1) ; i < children_.size() ; i++) {
          //   if (hash < children_[i].first) {
          //     children_[i-1].second->AddItem(hash, item);
          //     return;
          //   }
          // }
          for (auto it(std::next(children_.begin())) ; it != children_.end() ; ++it) {
            if (hash < it->first) {
              BPTNode<T> *node = (--it)->second;
              it = children_.end();
              node->AddItem(hash, item);
              return;
            }
          }
          GetThisChild(children_.size() - 1)->AddItem(hash, item);
        }
      }

      void Split() {
        BPTNode<T> newNode(degree_, parent_ ? parent_ : this);
        int s = keys_.size();
        std::pair<uint64_t, T> k;

        // for (int i(s) ; i > s / 2; i--) {
        //   newNode.AddItem(keys_.back().first, keys_.back().second);
        //   keys_.pop_back();
        // }
        while (keys_.size() > s / 2) {
          k = *keys_.rbegin();
          newNode.AddItem(k.first, k.second);
          keys_.erase(k.first);
        }

        if (parent_ != nullptr) {
          parent_->AddNode(new BPTNode<T>(newNode));
        } else {
          BPTNode<T> oldNode(degree_, newNode.GetParent());

          while(!keys_.empty()) {
            k = *keys_.rbegin();
            // oldNode.AddItem(keys_.back().first, keys_.back().second);
            oldNode.AddItem(k.first, k.second);
            // keys_.pop_back();
            keys_.erase(k.first);
          }

          keys_.clear();
          children_[oldNode.GetThisItem(0)] = new BPTNode<T>(oldNode);
          children_[newNode.GetThisItem(0)] = new BPTNode<T>(newNode);
        }
      }

      void AddNode(BPTNode<T>* n) {
        uint64_t h = 0;
        isLeaf_ = false;
        if (n->IsLeaf())
          h = n->GetThisItemHash(0);
        else
          h = n->GetThisChildrenHash(0);
        children_[h] = n;
        // std::sort(children_.begin(), children_.end(), CompareChildren);

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
            p = children_.begin()->second->GetParent();
          }
        }
        BPTNode<T>* newNode = new BPTNode<T> (degree_, p);
        int s = children_.size();
        std::pair<uint64_t, BPTNode<T> *>  child;

        // for (int i(s) ; i > s / 2; i--) {
        //   children_.back().second->SetParent(newNode);
        //   newNode->AddNode(children_.back().second);
        //   children_.pop_back();
        // }
        while (children_.size() > s / 2) {
          child = *children_.rbegin();
          child.second->SetParent(newNode);
          newNode->AddNode(child.second);
          children_.erase(child.first);
        }

        if (parent_ != nullptr) {
          parent_->AddNode(newNode);
        } else {
          BPTNode<T>* oldNode = new BPTNode<T>(degree_, newNode->GetParent());

          while(!children_.empty()) {
            // children_.back().second->SetParent(oldNode);
            // oldNode->AddNode(children_.back().second);
            // children_.pop_back();
            child = *children_.rbegin();
            child.second->SetParent(oldNode);
            oldNode->AddNode(child.second);
            children_.erase(child.first);
          }

          children_.clear();
          children_[oldNode->GetThisChildrenHash(0)] = oldNode;
          children_[newNode->GetThisChildrenHash(0)] = newNode;
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
              BPTNode<T> *node = (--it)->second;
              const uint64_t hash = it->first;
              it = children_.end();
              ret = node->DeleteItem(item);

              if (
                  (node->IsLeaf() && !node->HasEnoughKey()) ||
                  (!node->IsLeaf() && !node->HasEnoughChildren())
              ) {
                RemoveChild(hash);
              }

              return ret;
            }
          }
          BPTNode<T> *lastNode = children_.rbegin()->second;
          ret = lastNode->DeleteItem(item);
        }

        return ret;
      }

      bool RemoveChild(const uint64_t& hash, const bool deleteChildrenItems = false) {
        for (auto it(children_.begin()) ; it != children_.end() ; it++) {
          if (it->first == hash) {
            bool updateNecessary(false);
            BPTNode<T>* child = it->second;

            if (it == children_.begin() && parent_ != nullptr) {
              updateNecessary = true;
            }
            children_.erase(it);
            it = children_.end();

            if (!deleteChildrenItems) {
              if (child->isLeaf_) {
                std::map<uint64_t, T> childKeys = child->GetKeys();
                for (auto k : childKeys) {
                  AddItem(k.first, k.second);
                }
              } else {
                std::map<uint64_t, BPTNode<T> *> childChildren = child->GetChildren();
                for (auto c : childChildren) {
                  AddNode(c.second);
                }
              }
            }
            if (updateNecessary) {
              parent_->UpdateKeysFromChild(hash, children_.begin()->first);
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

      T& Search(const uint64_t &item) {
        if (isLeaf_) {
          auto it(keys_.find(item));

          if (it == keys_.end()) {
            throw std::out_of_range("Item not found");
          }

          return it->second;
        }

        for (auto child(std::next(children_.begin())) ; child != children_.end() ; ++child) {
          if (item < child->first) {
            return std::prev(child)->second->Search(item);
          }
        }

        return children_.rbegin()->second->Search(item);
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
          // for (int i(0) ; i < children_.size() ; i++) {
          //   if (children_[i].first == oldKey) {
          //     children_[i].first = newKey;
          //     if (i == 0 && parent_ != nullptr) {
          //       return parent_->UpdateKeysFromChild(oldKey, newKey);
          //     }
          //     return true;
          //   }
          // }
          auto it(children_.find(oldKey));
          if (it != children_.end()) {
            children_[newKey] = it->second;

            if (it == children_.begin() && parent_ != nullptr) {
              children_.erase(it);
              return parent_->UpdateKeysFromChild(oldKey, newKey);
            }
            children_.erase(it);
            return true;
          }
        }
        return false;
      }

    private:
      BPTNode<T>* parent_;
      std::map<uint64_t, BPTNode<T>* > children_; // stored in increasing order by default
      std::map<uint64_t, T> keys_;
      const unsigned int degree_;
      bool isLeaf_;
  };
} // namespace BPT