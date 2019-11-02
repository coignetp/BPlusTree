#include "../third_party/googletest/googletest/include/gtest/gtest.h"

#include "bptree.hpp"

TEST (TreeGetDegreeTest, Complete) { 
  BPT::BPTree<int> bpt(5);

  ASSERT_EQ(bpt.GetDegree(), 5);
}

TEST (TreeGetRootTest, Complete) {
  BPT::BPTree<int> bpt(42);

  ASSERT_EQ(bpt.GetRootNode().GetDegree(), 42);
}