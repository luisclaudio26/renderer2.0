#include "../../include/kdtree/kdtree.h"

static const int MAX_PRIM = 50;

static void compute_aabb(const std::vector<Primitive::ptr>& prims, AABB& target)
{
  target.min = Vec3(FLT_MAX);
  target.max = Vec3(-FLT_MAX);

  for(auto p : prims)
  {
      AABB p_aabb; p->aabb(p_aabb);
      for(int i = 0; i < 3; ++i)
      {
        //TODO: is it necessary to take the minimum of
        //p_aabb's min and max corners? I don't think so
        target.min[i] = std::fmin(p_aabb.min[i], target.min[i]);
        target.max[i] = std::fmax(p_aabb.max[i], target.max[i]);
      }
  }
}

//-----------------------------------
//---------- FROM KDTREE.H ----------
//-----------------------------------
float KdNode::split_at(const std::vector<Primitive::ptr>& prims,
                        const std::vector<int>& prims_ids, int& axis)
{
  //TODO: this is not a good way of selecting the axis!
  static int axis_cnt = 0;
  axis = (axis_cnt++) % 3;

  //TODO: this is not a good heuristic for primitive splitting
  float avg = 0.0f;
  for(auto id : prims_ids)
  {
    AABB p_aabb; prims[id]->aabb(p_aabb);
    avg += p_aabb.min[axis];
  }

  return avg / prims_ids.size();
}

KdNode::KdNode(const std::vector<Primitive::ptr>& prims,
                const std::vector<int>& prims_ids, const AABB& aabb)
{
  //stop criterion
  if(prims_ids.size() < MAX_PRIM)
  {
    this->left = this->right = NULL;
    this->prims = std::move( prims_ids );
    this->aabb = aabb;
  }
  else
  {
    //choose splitting point
    float split; int axis;
    split = split_at(prims, prims_ids, axis);

    //classify primitives into left/right
    std::vector<int> left, right;
    for(auto id : prims_ids)
    {
      //TODO: precompute bounding boxes
      Primitive::ptr p = prims[id];
      AABB p_aabb; p->aabb(p_aabb);

      if(p_aabb.min[axis] < split) left.push_back( id );
      if(p_aabb.max[axis] > split) right.push_back( id );
    }

    //compute new bounding boxes
    AABB left_aabb = aabb; left_aabb.max[axis] = split;
    AABB right_aabb = aabb; right_aabb.min[axis] = split;

    //recursively build children
    this->left = new KdNode(prims, left, left_aabb);
    this->right = new KdNode(prims, right, right_aabb);
  }
}

void KdTree::build(const std::vector<Primitive::ptr>& prims)
{
  //this vector will contain indices to all primitives
  //we want to store inside the tree
  std::vector<int> prims_ids;
  for(int i = 0; i < prims.size(); ++i)
    prims_ids.push_back(i);

  AABB full_aabb; compute_aabb(prims, full_aabb);

  root = KdNode(prims, prims_ids, full_aabb);
}
