#include "../../include/kdtree/kdtree.h"

static void compute_aabb(const std::vector<Primitive::ptr>& prims)
{
  Vec3 min(FLT_MAX); Vec3 max(-FLT_MAX);

  for(auto p : prims)
  {
      AABB p_aabb; p->aabb(p_aabb);
      for(int i = 0; i < 3; ++i)
      {
        //TODO: is it necessary to take the minimum of
        //p_aabb's min and max corners? I don't think so
        min[i] = std::fmin(p_aabb.min[i], min[i]);
        max[i] = std::fmax(p_aabb.max[i], max[i]);
      }
  }
}

//-----------------------------------
//---------- FROM KDTREE.H ----------
//-----------------------------------
float KdNode::split_at(const KdNode& n, int& axis)
{

}

KdNode::KdNode(const std::vector<int>& prims, const AABB& aabb)
{

}

void KdTree::build(const std::vector<Primitive::ptr>& prims)
{
  //this vector will contain indices to all primitives
  //we want to store inside the tree
  std::vector<int> all_prims;
  for(int i = 0; i < prims.size(); ++i)
    all_prims.push_back(i);

  root = KdNode();
}
