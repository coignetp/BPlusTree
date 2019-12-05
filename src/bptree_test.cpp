#include "../third_party/googletest/googletest/include/gtest/gtest.h"

#include "bptree.hpp"

uint64_t hashFunc(int a) { return a; }

TEST(TreeGetDegreeTest, Complete) {
  BPT::BPTree<int> bpt(5, hashFunc);

  ASSERT_EQ(bpt.GetDegree(), (unsigned)5);
}

TEST(TreeGetRootTest, Complete) {
  BPT::BPTree<int> bpt(42, hashFunc);

  ASSERT_EQ(bpt.GetRootNode()->GetDegree(), (unsigned)42);
}

TEST(TreeAddItemTest, Complete) {
  BPT::BPTree<int> bpt(5, hashFunc);

  for (int i(0); i < 100; i++) bpt.AddItem(i);

  for (int i(0); i < 100; i++) ASSERT_EQ(bpt.SearchItem((uint64_t)i), i);
}

TEST(TreeDeleteItemTest, Complete) {
  BPT::BPTree<int> bpt(5, hashFunc);

  for (int i(0); i < 100; i++) bpt.AddItem(i);

  for (int i(0); i < 100; i++) ASSERT_EQ(bpt.SearchItem((uint64_t)i), i);

  for (int i(30); i < 80; i++) {
    ASSERT_TRUE(bpt.DeleteItem(i));
    ASSERT_FALSE(bpt.DeleteItem(i));
  }
}

TEST(TreeDeepCopy, Complete) {
  BPT::BPTree<int> bpt(5, hashFunc);

  for (int i(0); i < 100; i++) {
    bpt.AddItem(i);
  }

  BPT::BPTree<int> clone(5, hashFunc);
  clone.DeepCopyFrom(&bpt);

  for (int i(0); i < 100; i++) ASSERT_EQ(clone.SearchItem(i), i);
}

// Test for BPTNode

TEST(NodeGetDegreeTest, Complete) {
  BPT::BPTNode<int> node(5);

  ASSERT_EQ(node.GetDegree(), (unsigned)5);
}

TEST(NodeAddItem, NoSplit) {
  BPT::BPTNode<int> node(5);

  node.AddItem(5, 5);

  ASSERT_EQ(node.GetItem(5), 5);
  ASSERT_EQ(node.GetThisItem(0), 5);

  ASSERT_THROW(node.GetItem(42), std::out_of_range);
  ASSERT_THROW(node.GetThisItem(3), std::out_of_range);
}

TEST(NodeAddItem, SimpleSplit) {
  BPT::BPTNode<int> node(5);

  for (int i(0); i < 6; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node.Length(), 2);
  ASSERT_EQ(node.GetThisChild(0)->IsLeaf(), true);
  ASSERT_EQ(node.GetThisChild(1)->IsLeaf(), true);
  ASSERT_THROW(node.GetThisChild(2), std::out_of_range);

  ASSERT_EQ(node.IsLeaf(), false);

  ASSERT_EQ(node.GetChildHash(0), (unsigned)0);
  ASSERT_EQ(node.GetThisChild(0)->Length(), 2);
  ASSERT_EQ(node.GetThisChild(0)->GetThisItem(1), 1);
  ASSERT_EQ(node.GetChildHash(1), (unsigned)2);
  ASSERT_EQ(node.GetThisChild(1)->Length(), 4);
  ASSERT_EQ(node.GetThisChild(1)->GetThisItem(1), 3);
}

TEST(NodeAddItem, MultipleSplit) {
  BPT::BPTNode<int> node(5);

  for (int i(0); i < 18; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node.Length(), 2);
  ASSERT_EQ(node.GetThisChild(0)->Length(), 3);
  ASSERT_EQ(node.GetChildHash(0), (unsigned)0);
  ASSERT_EQ(node.GetThisChild(1)->Length(), 5);
  ASSERT_EQ(node.GetChildHash(1), (unsigned)6);

  for (int i(0); i < 3; i++) {
    ASSERT_EQ(node.GetThisChild(0)->GetThisChild(i)->Length(), 2);
    ASSERT_EQ(node.GetThisChild(1)->GetThisChild(i)->Length(), 2);
  }
  ASSERT_EQ(node.GetThisChild(1)->GetThisChild(3)->Length(), 2);
  ASSERT_EQ(node.GetThisChild(1)->GetThisChild(4)->Length(), 4);

  ASSERT_EQ(node.GetThisChild(0)->GetThisChild(2)->GetThisItem(1), 5);
  ASSERT_EQ(node.GetThisChild(1)->GetThisChild(1)->GetThisItem(1), 9);
}

TEST(NodeDeleteItem, SimpleRootDelete) {
  BPT::BPTNode<int> node(5);

  node.AddItem(0, 0);
  node.AddItem(1, 1);

  ASSERT_EQ(node.GetThisItem(1), 1);

  ASSERT_TRUE(node.DeleteItem(1));
  ASSERT_FALSE(node.DeleteItem(2));

  ASSERT_THROW(node.GetThisItem(1), std::out_of_range);
}

TEST(NodeDeleteItem, SimpleNodeDelete) {
  BPT::BPTNode<int> node(5);

  for (int i(0); i < 9; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node.GetThisChild(3)->GetThisItem(2), 8);

  ASSERT_TRUE(node.DeleteItem(8));
  ASSERT_FALSE(node.DeleteItem(8));

  ASSERT_THROW(node.GetThisChild(3)->GetThisItem(2), std::out_of_range);
}

TEST(NodeDeleteItem, NodeDelete) {
  BPT::BPTNode<int> node(5);

  for (int i(0); i < 9; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node.GetThisChild(0)->GetThisItem(0), 0);

  ASSERT_TRUE(node.DeleteItem(0));
  ASSERT_FALSE(node.DeleteItem(0));

  ASSERT_EQ(node.GetThisChild(0)->Length(), 3);
  ASSERT_EQ(node.GetChildHash(0), (unsigned)1);

  ASSERT_EQ(node.GetThisChild(0)->GetThisItem(0), 1);
}

TEST(NodeDeleteNode, SimpleDelete) {
  BPT::BPTNode<int> node(5);

  for (int i(0); i < 9; i++) {
    node.AddItem(i, i);
  }

  ASSERT_EQ(node.GetThisChild(0)->GetThisItem(0), 0);

  ASSERT_TRUE(node.RemoveChild(0, true));
  ASSERT_FALSE(node.RemoveChild(0));

  ASSERT_EQ(node.GetThisChild(0)->GetThisItem(0), 2);
}

TEST(NodeDeepCopy, SimpleNode) {
  BPT::BPTNode<int> node(8);

  for (int i(0); i < 7; i++) {
    node.AddItem(i, i);
  }

  BPT::BPTNode<int> clone(8);
  clone.DeepCopyFrom(&node);

  for (int i(0); i < 7; i++) ASSERT_EQ(clone.GetItem(i), i);
}
