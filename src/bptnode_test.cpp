#include "../third_party/googletest/googletest/include/gtest/gtest.h"

#include "bptnode.hpp"

TEST (NodeGetDegreeTest, Complete) { 
  BPT::BPTNode<int> node(5);

  ASSERT_EQ(node.GetDegree(), 5);
}