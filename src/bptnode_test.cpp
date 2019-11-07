#include "../third_party/googletest/googletest/include/gtest/gtest.h"

#include "bptnode.hpp"

#include <exception>

TEST (NodeGetDegreeTest, Complete) { 
  BPT::BPTNode<int> node(5);

  ASSERT_EQ(node.GetDegree(), 5);
}

TEST (NodeAddItem, NoSplit) {
  BPT::BPTNode<int> node(5);

  node.AddItem(5, 5);

  ASSERT_EQ(node.GetItem(5), 5);
  ASSERT_EQ(node.GetThisItem(0), 5);

  ASSERT_THROW(node.GetItem(42), std::out_of_range);
  ASSERT_THROW(node.GetThisItem(3), std::out_of_range);
}

TEST (NodeAddItem, SimpleSplit) {
  BPT::BPTNode<int> node(5);

  for (int i(0); i < 7 ; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node.Length(), 2);
  ASSERT_EQ(node[0].second->IsLeaf(), true);
  ASSERT_EQ(node[1].second->IsLeaf(), true);
  ASSERT_THROW(node[2], std::out_of_range);

  ASSERT_EQ(node.IsLeaf(), false);

  auto children = node.GetChildren();

  ASSERT_EQ(children[0].first, 0);
  ASSERT_EQ(children[0].second->Length(), 3);
  ASSERT_EQ(children[0].second->GetThisItem(1), 1);
  ASSERT_EQ(children[1].first, 3);
  ASSERT_EQ(children[1].second->Length(), 4);
  ASSERT_EQ(children[1].second->GetThisItem(1), 4);
}