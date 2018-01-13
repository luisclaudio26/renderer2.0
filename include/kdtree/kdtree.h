#ifndef KDTREE_H
#define KDTREE_H

#include "../core/primitive.h"
#include "../core/geometry.h"

class KdNode
{
public:
  AABB aabb;
  KdNode *left, *right;

  //TODO: we're mixing Leaf and internal node
  //data. split this (union? inheritance?)
  std::vector<int> prims;
};

class KdTree
{
private:
  float split_at(const KdNode& n);

  KdNode root;

public:
  void build(const std::vector<Primitive::ptr>& prims);
};

#endif
