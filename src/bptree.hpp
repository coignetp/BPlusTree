#pragma once

#include <algorithm>
#include <exception>
#include <functional>
#include <list>
#include <memory>
#include <vector>
#include <stdexcept>

// Everything is in the same header file here.
// It makes it easier to use it as a dependency.

namespace BPT {
// BPTNode
// A node can be a leaf or a core node.
// A leaf node contains the item to store, and the core node contains other
// core nodes or leaf nodes
template <typename T>
class BPTNode {
 public:
  template <typename U>
  using BPTContainer = std::map<uint64_t, U>;

 public:
  BPTNode(const unsigned int degree, BPTNode<T>* parent = nullptr)
      : parent_(parent), degree_(degree), isLeaf_(true) {}
  BPTNode(const BPTNode<T>& n) : degree_(n.GetDegree()) {
    isLeaf_ = n.IsLeaf();
    parent_ = n.GetParent();
    children_ = n.GetChildren();
    keys_ = n.GetKeys();
  }

  ~BPTNode() {
    for (std::pair<uint64_t, BPTNode<T>*> p : children_) {
      delete p.second;
    }
  }

  BPTContainer<BPTNode<T>*> GetChildren() const { return children_; }

  BPTContainer<T> GetKeys() const { return keys_; }

  BPTNode<T>* GetParent() const { return parent_; }

  void SetParent(BPTNode<T>* parent) { parent_ = parent; }

  unsigned int GetDegree() const { return degree_; }

  bool IsLeaf() const { return isLeaf_; }

  int Length() const {
    if (isLeaf_) return keys_.size();
    return children_.size();
  }

  T& GetItem(const uint64_t& hash) {
    if (!isLeaf_) throw std::logic_error("Not a leaf node");

    auto it = keys_.find(hash);

    if (it == keys_.end())
      throw std::out_of_range("Item not found");

    return it->second;
  }

  // Get the item number index in this leaf node
  T& GetThisItem(const int index) {
    if ((unsigned)index >= keys_.size())
      throw std::out_of_range("Index out of range");
    return std::next(keys_.begin(), index)->second;
  }

  // Get the hash of the element at position index
  uint64_t GetKeyHash(const int index) {
    if ((unsigned)index >= keys_.size())
      throw std::out_of_range("Index out of range");
    return std::next(keys_.begin(), index)->first;
  }

  // Get the child node corresponding to the hash
  BPTNode<T>* GetChild(const uint64_t& hash) {
    if (!isLeaf_) throw std::logic_error("Not a leaf node");

    auto it = children_.find(hash);

    if (it == children_.end())
      throw std::out_of_range("Item not found");

    return it->second;
  }

  // Get the child node number index
  BPTNode<T>* GetThisChild(const int index) {
    if ((unsigned)index >= children_.size())
      throw std::out_of_range("Index out of range");
    return std::next(children_.begin(), index)->second;
  }

  // Get the hash of the child node number index
  uint64_t GetChildHash(const int index) {
    if ((unsigned)index >= children_.size())
      throw std::out_of_range("Index out of range");
    return std::next(children_.begin(), index)->first;
  }

  // Get the lower child bound
  BPTNode<T> *GetLowerBoundChild(const uint64_t &hash) {
    if (isLeaf_)
      throw std::exception_ptr();
    
    auto it = children_.find(hash);
    if (it != children_.end())
      return it->second;

    it = children_.upper_bound(hash);

    if (it == children_.begin())
      return it->second;

    return std::prev(it)->second;
  }

  // Add an item to this node or one of its child node
  void AddItem(uint64_t hash, T item) {
    if (isLeaf_) {
      uint64_t firstHash = keys_.begin()->first;
      keys_.insert(std::make_pair(hash, item));

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
      GetLowerBoundChild(hash)->AddItem(hash, item);
    }
  }

  // Split a leaf node in 2 and balance the node
  void Split() {
    BPTNode<T> newNode(degree_, parent_ ? parent_ : this);
    int s = keys_.size();

    auto splitIterator = std::next(keys_.begin(), s/2);
    newNode.AddMultipleItems(splitIterator, keys_.end());
    keys_.erase(splitIterator, keys_.end());

    if (parent_ != nullptr) {
      parent_->AddNode(new BPTNode<T>(newNode));
    } else {
      BPTNode<T> oldNode(degree_, newNode.GetParent());

      while (!keys_.empty()) {
        oldNode.AddItem(std::prev(keys_.end())->first, std::prev(keys_.end())->second);
        keys_.erase(std::prev(keys_.end()));
      }

      keys_.clear();
      children_.insert(
          std::make_pair(oldNode.GetThisItem(0), new BPTNode<T>(oldNode)));
      children_.insert(
          std::make_pair(newNode.GetThisItem(0), new BPTNode<T>(newNode)));
    }
  }

  // Add a child node to this node
  void AddNode(BPTNode<T>* n) {
    uint64_t h = 0;
    isLeaf_ = false;
    if (n->IsLeaf())
      h = n->GetKeyHash(0);
    else
      h = n->GetChildHash(0);

    children_.insert(std::make_pair(h, n));

    if (children_.size() > degree_) {
      SplitNode();
    }
  }

  // Split a core node and balance this node
  void SplitNode() {
    BPTNode<T>* p(parent_);
    if (!parent_) {
      if (children_.empty()) {
        p = this;
      } else {
        p = children_.begin()->second->GetParent();
      }
    }
    BPTNode<T>* newNode = new BPTNode<T>(degree_, p);
    int s = children_.size();

    for (int i(s); i > s / 2; i--) {
      std::prev(children_.end())->second->SetParent(newNode);
      newNode->AddNode(std::prev(children_.end())->second);
      children_.erase(std::prev(children_.end()));
    }

    if (parent_ != nullptr) {
      parent_->AddNode(newNode);
    } else {
      BPTNode<T>* oldNode = new BPTNode<T>(degree_, newNode->GetParent());

      while (!children_.empty()) {
        std::prev(children_.end())->second->SetParent(oldNode);
        oldNode->AddNode(std::prev(children_.end())->second);
        children_.erase(std::prev(children_.end()));
      }

      children_.clear();
      children_.insert(std::make_pair(oldNode->GetChildHash(0), oldNode));
      children_.insert(std::make_pair(newNode->GetChildHash(0), newNode));
    }
  }

  // Delete an item corresponding to the hash given
  // Balance the tree once it's done
  bool DeleteItem(const uint64_t& item) {
    bool ret = false;
    if (isLeaf_) {
      auto it = keys_.find(item);
      if (it == keys_.end())
        return false;

      keys_.erase(it);
      return true;
    } else {
      for (auto it(std::next(children_.begin())); it != children_.end(); ++it) {
        if (item < it->first) {
          --it;
          BPTNode<T>* node = it->second;
          uint64_t t = it->first;

          ret = node->DeleteItem(item);

          if ((node->IsLeaf() && !node->HasEnoughKey()) ||
              (!node->IsLeaf() && !node->HasEnoughChildren())) {
            RemoveChild(t);
          }

          return ret;
        }
      }
      BPTNode<T>* backNode = std::prev(children_.end())->second;
      ret = backNode->DeleteItem(item);
      if ((backNode->IsLeaf() && !backNode->HasEnoughKey()) ||
          (!backNode->IsLeaf() && !backNode->HasEnoughChildren())) {
        RemoveChild(std::prev(children_.end())->first);
      }
    }

    return ret;
  }

  // Remove a child node. If deleteChildrenItems is false, also delete the
  // children of the node to delete.
  bool RemoveChild(const uint64_t& hash,
                   const bool deleteChildrenItems = false) {
    for (auto it(children_.begin()); it != children_.end(); it++) {
      if (it->first == hash) {
        BPTNode<T>* child = it->second;

        if (it == children_.begin() && parent_ != nullptr) {
          parent_->UpdateKeysFromChild(hash,
                                       std::next(children_.begin())->first);
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

  // True if the leaf node doesn't need to be splitted
  bool HasEnoughKey() const {
    return (isLeaf_ && keys_.size() >= degree_ / 2) || parent_ == nullptr;
  }

  // True if the core node doesn't need to be splitted
  bool HasEnoughChildren() const {
    return (!isLeaf_ && children_.size() >= degree_ / 2) || parent_ == nullptr;
  }

  // Deep copy the given node here. Creates new children nodes
  void DeepCopyFrom(BPTNode<T>* n) {
    isLeaf_ = n->IsLeaf();
    if (n->IsLeaf()) {
      for (std::pair<uint64_t, T> k : n->GetKeys()) {
        AddItem(k.first, k.second);
      }
    } else {
      for (std::pair<uint64_t, BPTNode<T>*> c : n->GetChildren()) {
        children_.insert(
            std::make_pair(c.first, new BPTNode<T>(degree_, this)));
        std::prev(children_.end())->second->DeepCopyFrom(c.second);
      }
    }
  }

  // Add multiple items and then sort it. Used only by the split method
  void AddMultipleItems(typename BPTContainer<T>::iterator firstItem, typename BPTContainer<T>::iterator lastItem) {
    while (firstItem != lastItem) {
      keys_.insert(keys_.end(), std::make_pair(firstItem->first, firstItem->second));
      firstItem++;
    }
  }

 private:
  // True if the hash of the node needs to be updated to to an insertion or a
  // deletion
  bool UpdateKeysFromChild(const uint64_t& oldKey, const uint64_t newKey) {
    if (!isLeaf_) {
      children_.insert(std::make_pair(newKey, nullptr));
      bool updateParent = (parent_ != nullptr && children_.begin()->first != newKey);
      for (auto it(children_.begin()); it != children_.end(); ++it) {
        if (it->first == oldKey) {

          children_[newKey] = it->second;
          it = children_.erase(it);
          if (updateParent) {
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
  BPTContainer<BPTNode<T>*> children_;
  BPTContainer<T> keys_;
  const unsigned int degree_;
  bool isLeaf_;
};

// BPTree
template <typename T>
class BPTree {
 public:
  BPTree(unsigned int degree, std::function<uint64_t(const T&)> hashing)
      : degree_(degree), root_(new BPTNode<T>(degree)), hashing_(hashing) {}

  BPTree(const BPTree<T>& bpt)
      : degree_(bpt.GetDegree()),
        root_(bpt.GetRootNode()),
        hashing_(bpt.GetHashFunction()) {}

  ~BPTree() { delete root_; }

  unsigned int GetDegree() const { return degree_; }

  BPTNode<T>* GetRootNode() const { return root_; }

  std::function<uint64_t(const T&)> GetHashFunction() const { return hashing_; }

  // Deep copy the given tree here. Reallocate every node
  void DeepCopyFrom(BPTree<T>* bpt) {
    degree_ = bpt->GetDegree();
    hashing_ = bpt->GetHashFunction();
    delete root_;

    root_ = new BPTNode<T>(degree_);
    root_->DeepCopyFrom(bpt->GetRootNode());
  }

  T& SearchItem(const T& item) { return SearchItem(hashing_(item)); }

  T& SearchItem(const uint64_t& item) {
    return SearchLeaf(item)->GetItem(item);
  }

  // Search a leaf that could contain the item with the given hash
  BPTNode<T>* SearchLeaf(const uint64_t& item) {
    BPTNode<T>* node = root_;

    if (node == nullptr)
      throw std::out_of_range("Searching in an empty BPTree");

    while (!node->IsLeaf()) {
      node = node->GetLowerBoundChild(item);

    }

    return node;
  }

  // Add an item to the tree
  void AddItem(T item) {
    uint64_t hash = hashing_(item);
    BPTNode<T>* leaf = SearchLeaf(hash);

    leaf->AddItem(hash, item);
  }

  // Delete an item from the tree
  bool DeleteItem(T item) { return DeleteItem(hashing_(item)); }

  // Delete an item from the tree with its hash
  bool DeleteItem(uint64_t item) {
    uint64_t hash = hashing_(item);
    BPTNode<T>* leaf = SearchLeaf(hash);

    return leaf->DeleteItem(item);
  }

  // Simple copy of a BPTree
  BPTree<T>& operator=(const BPTree<T>& other) {
    degree_ = other.GetDegree();
    root_ = other.GetRootNode();
    hashing_ = other.GetHashFunction();

    return *this;
  }

 private:
  unsigned int degree_;
  BPTNode<T>* root_;
  std::function<uint64_t(const T&)> hashing_;
};

}  // namespace BPT
