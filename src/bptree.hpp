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
      BPTree(unsigned int degree, std::function<uint64_t (const T&)> hashing) : degree_(degree), root_(degree), hashing_(hashing) {}

      unsigned int GetDegree() const {
        return degree_;
      }

      BPTNode<T> GetRootNode() const {
        return root_;
      }

      T& SearchItem(const T& item) {
        return SearchItem(hashing_(item));
      }

      T& SearchItem(const uint64_t& item) {
        return SearchLeaf(item)->GetItem(item);
      }

      BPTNode<T> *SearchLeaf(const uint64_t& item) {
        BPTNode<T> *node = &root_;

        if (node == nullptr)
          throw std::exception("Searching in an empty BPTree");
        
        while (!node->isLeaf()) {
          int i = 0;
          while (i < node.Length() && item > node[i].first)
            i++;
           
           node = node[i].second;
        }

        return node;
      }

      bool AddItem(T item) {
        uint64_t hash = hashing_(item);
        BPTNode<T> *leaf = SearchLeaf(hash);

        return leaf.AddItem(item);
      }

      bool DeleteItem(T item) {
        return DeleteItem(hashing_(item));
      }

      bool DeleteItem(uint64_t item) {
        uint64_t hash = hashing_(item);
        BPTNode<T> *leaf = SearchLeaf(hash);

        return leaf.DeleteItem(item);
      }

    private:
      unsigned int degree_;
      BPTNode<T> root_;
      std::function<uint64_t (const T&)> hashing_;
  };
} // namespace BPT