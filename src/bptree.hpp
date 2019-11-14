#pragma once

#include "bptnode.hpp"

#include <exception>

// Everything will be moved to a single header file at the end of the project.
// It makes it easier to use it as a dependency.
namespace BPT
{
  template<typename T>
  class BPTree {
    public:
      BPTree(unsigned int degree, std::function<uint64_t (const T&)> hashing) :
        degree_(degree),
        root_(new BPTNode<T>(degree)),
        hashing_(hashing) {}

      ~BPTree() {
        delete root_;
      }

      unsigned int GetDegree() const {
        return degree_;
      }

      BPTNode<T>* GetRootNode() const {
        return root_;
      }

      T& SearchItem(const T& item) {
        return SearchItem(hashing_(item));
      }

      T& SearchItem(const uint64_t& item) {
        return SearchLeaf(item)->GetItem(item);
      }

      BPTNode<T>* SearchLeaf(const uint64_t& item) {
        BPTNode<T>* node = root_;

        if (node == nullptr)
          throw std::out_of_range("Searching in an empty BPTree");
        
        while (!node->IsLeaf()) {
          bool found = false;
          for(int i(1) ; i < node->Length() && !found; i++) {
            if (item < (*node)[i].first) {
              node = (*node)[i-1].second;
              found = true;
            }
          }
          if (!found)
            node = (*node)[node->Length() - 1].second;
        }

        return node;
      }

      void AddItem(T item) {
        uint64_t hash = hashing_(item);
        BPTNode<T>* leaf = SearchLeaf(hash);

        leaf->AddItem(hash, item);
      }

      bool DeleteItem(T item) {
        return DeleteItem(hashing_(item));
      }

      bool DeleteItem(uint64_t item) {
        uint64_t hash = hashing_(item);
        BPTNode<T>* leaf = SearchLeaf(hash);

        return leaf->DeleteItem(item);
      }

    private:
      unsigned int degree_;
      BPTNode<T>* root_;
      std::function<uint64_t (const T&)> hashing_;
  };
} // namespace BPT