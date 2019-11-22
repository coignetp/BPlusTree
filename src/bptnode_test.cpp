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

  for (int i(0); i < 6 ; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node.Length(), 2);
  ASSERT_EQ(node[0].second->IsLeaf(), true);
  ASSERT_EQ(node[1].second->IsLeaf(), true);
  ASSERT_THROW(node[2], std::out_of_range);

  ASSERT_EQ(node.IsLeaf(), false);

  auto children = node.GetChildren();

  ASSERT_EQ(children[0].first, 0);
  ASSERT_EQ(children[0].second->Length(), 2);
  ASSERT_EQ(children[0].second->GetThisItem(1), 1);
  ASSERT_EQ(children[1].first, 2);
  ASSERT_EQ(children[1].second->Length(), 4);
  ASSERT_EQ(children[1].second->GetThisItem(1), 3);
}

TEST (NodeAddItem, MultipleSplit) {
  BPT::BPTNode<int> node(5);

  for (int i(0) ; i < 18 ; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node.Length(), 2);
  ASSERT_EQ(node[0].second->Length(), 3);
  ASSERT_EQ(node[0].first, 0);
  ASSERT_EQ(node[1].second->Length(), 5);
  ASSERT_EQ(node[1].first, 6);

  for (int i(0) ; i < 3 ; i++) {
    ASSERT_EQ((*node[0].second)[i].second->Length(), 2);
    ASSERT_EQ((*node[1].second)[i].second->Length(), 2);
  }
  ASSERT_EQ((*node[1].second)[3].second->Length(), 2);
  ASSERT_EQ((*node[1].second)[4].second->Length(), 4);

  ASSERT_EQ((*node[0].second)[2].second->GetThisItem(1), 5);
  ASSERT_EQ((*node[1].second)[1].second->GetThisItem(1), 9);
}

TEST (NodeDeleteItem, SimpleRootDelete) {
  BPT::BPTNode<int> node(5);

  node.AddItem(0, 0);
  node.AddItem(1, 1);

  ASSERT_EQ(node.GetThisItem(1), 1);

  ASSERT_TRUE(node.DeleteItem(1));
  ASSERT_FALSE(node.DeleteItem(2));

  ASSERT_THROW(node.GetThisItem(1), std::out_of_range);
}

TEST (NodeDeleteItem, SimpleNodeDelete) {
  BPT::BPTNode<int> node(5);

  for (int i(0) ; i < 9 ; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node[3].second->GetThisItem(2), 8);

  ASSERT_TRUE(node.DeleteItem(8));
  ASSERT_FALSE(node.DeleteItem(8));

  ASSERT_THROW(node[3].second->GetThisItem(2), std::out_of_range);
}

TEST (NodeDeleteItem, NodeDelete) {
  BPT::BPTNode<int> node(5);

  for (int i(0) ; i < 9 ; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node[0].second->GetThisItem(0), 0);

  ASSERT_TRUE(node.DeleteItem(0));
  ASSERT_FALSE(node.DeleteItem(0));

  ASSERT_EQ(node[0].second->Length(), 3);
  ASSERT_EQ(node[0].first, 1);

  ASSERT_EQ(node[0].second->GetThisItem(0), 1);
}

TEST (NodeDeleteNode, SimpleDelete) {
  BPT::BPTNode<int> node(5);

  for (int i(0) ; i < 9 ; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node[0].second->GetThisItem(0), 0);

  ASSERT_TRUE(node.RemoveChild(0, true));
  ASSERT_FALSE(node.RemoveChild(0));

  ASSERT_EQ(node[0].second->GetThisItem(0), 2);
}