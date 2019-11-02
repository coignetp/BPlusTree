#pragma once

#include <memory>
#include <vector>

namespace BPT
{
  template<typename T>
  class BPTNode {
    public:
      BPTNode(const unsigned int degree) : degree_(degree) {}

      unsigned int GetDegree() const {
        return degree_;
      }
    
    private:
      std::vector< BPTNode<T>* > children_;
      const unsigned int degree_;
  };
} // namespace BPT