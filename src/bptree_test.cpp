#include "../third_party/googletest/googletest/include/gtest/gtest.h"

#include "bptree.hpp"

uint64_t hashFunc(int a) {
  return a;
}

TEST (TreeGetDegreeTest, Complete) { 
  BPT::BPTree<int> bpt(5, hashFunc);

  ASSERT_EQ(bpt.GetDegree(), 5);
}

TEST (TreeGetRootTest, Complete) {
  BPT::BPTree<int> bpt(42, hashFunc);

  ASSERT_EQ(bpt.GetRootNode()->GetDegree(), 42);
}

TEST (TreeAddItemTest, Complete) {
  BPT::BPTree<int> bpt(5, hashFunc);

  for (int i(0) ; i < 100 ; i++)
    bpt.AddItem(i);

  for (int i(0) ; i < 100 ; i++)
    ASSERT_EQ(bpt.SearchItem((uint64_t)i), i);
}