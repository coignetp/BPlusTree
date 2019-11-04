#pragma once

#include <memory>
#include <vector>

namespace BPT
{
  template<typename T>
  class BPTNode {
    public:
      BPTNode(const unsigned int degree, BPTNode<T> *parent = nullptr) : parent_(parent), degree_(degree), isLeaf_(true) {}

      unsigned int GetDegree() const {
        return degree_;
      }

      bool IsLeaf() const {
        return isLeaf_;
      }

      std::pair<uint64_t, BPTNode<T>*> &operator[](int index) {
        return children_[index];
      }

      int Length() const {
        return children_.size();
      }

      T& GetItem(const uint64_t& hash) {
        if (!isLeaf_)
          throw std::exception("Not a leaf node");
        
        for (int i(0) ; i < keys_.size() ; i++) {
          if (hash == keys_[i].first) {
            return keys_[i].second;
          }
        }

        throw std::exception("Item not found");

        return keys_[0].second;
      }

      bool AddItem(T item) {
        // TODO
        return true;
      }

      bool DeleteItem(uint64_t item) {
        // TODO
        return true;
      } 
    
    private:
      BPTNode<T> *parent_;
      std::vector< std::pair<uint64_t, BPTNode<T>*> > children_;
      std::vector< std::pair<uint64_t, T> > keys_;
      const unsigned int degree_;
      bool isLeaf_;
  };
} // namespace BPT