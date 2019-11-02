#pragma once

#include "bptnode.hpp"

// Everything will be moved to a single header file at the end of the project.
// It makes it easier to use it as a dependency.
namespace BPT
{
  template<typename T>
  class BPTree {
    public:
      BPTree(unsigned int degree) : degree_(degree), root_(degree) {}

      unsigned int GetDegree() const {
        return degree_;
      }

      BPTNode<T> GetRootNode() const {
        return root_;
      }

    private:
      unsigned int degree_;
      BPTNode<T> root_;
  };
} // namespace BPT