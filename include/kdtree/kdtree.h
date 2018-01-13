#ifndef KDTREE_H
#define KDTREE_H

#include "../core/primitive.h"
#include "../core/geometry.h"

class KdNode
{
public:
  AABB aabb;
  int axis;
  KdNode *left, *right;

  //TODO: we're mixing Leaf and internal node
  //data. split this (union? inheritance?)
  std::vector<int> prims;

  float split_at(const KdNode& n, int& axis);
  KdNode() : left(NULL), right(NULL) {}
  KdNode(const std::vector<int>& prims, const AABB& aabb);
};

class KdTree
{
private:
  KdNode root;

public:
  void build(const std::vector<Primitive::ptr>& prims);
};

#endif
