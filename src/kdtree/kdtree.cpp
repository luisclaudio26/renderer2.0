#include "../../include/kdtree/kdtree.h"
#include <glm/gtx/string_cast.hpp>
#include <algorithm>

static const int MAX_PRIM = 15;

static void compute_aabb(const std::vector<Primitive::ptr>& prims, AABB& target)
{
  target.min = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
  target.max = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

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

static void compute_aabb(const std::vector<Primitive::ptr>& prims,
                          const std::vector<int>& prims_ids, AABB& target)
{
  target.min = Vec3(FLT_MAX);
  target.max = Vec3(-FLT_MAX);

  for(auto id : prims_ids)
  {
    AABB p_aabb; prims[id]->aabb(p_aabb);
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
bool KdTree::intersect(const Ray& r, Isect& isect) const
{

}

bool KdNode::is_leaf() const
{
  return left == NULL && right == NULL;
}

float KdNode::split_at(const std::vector<Primitive::ptr>& prims,
                        const std::vector<int>& prims_ids, const AABB& aabb,
                        int& axis)
{
  //get longest axis
  int longest = 0;
  for(int i = 0; i < 3; ++i)
    if(fabs(aabb.max[i]-aabb.min[i]) > fabs(aabb.max[longest]-aabb.min[longest]))
      longest = i;
  axis = longest;

  //TODO: this is not a good heuristic for primitive splitting
  //return (aabb.max[longest]+aabb.min[longest])*0.5f;

  std::vector<float> edges;
  for(auto id : prims_ids)
  {
    AABB p_aabb; prims[id]->aabb(p_aabb);
    edges.push_back(p_aabb.min[longest]);
    edges.push_back(p_aabb.max[longest]);
  }

  std::sort(edges.begin(), edges.end());
  return edges[edges.size()/2];

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
    split = split_at(prims, prims_ids, aabb, axis);

    printf("AABB: %s, %s\n", glm::to_string(aabb.min).c_str(),
                              glm::to_string(aabb.max).c_str());
    printf("Axis %d, split at %f\n", split, axis);

    //classify primitives into left/right
    std::vector<int> left, right;
    for(auto id : prims_ids)
    {
      //TODO: precompute bounding boxes
      AABB p_aabb; prims[id]->aabb(p_aabb);

      if(p_aabb.min[axis] < split) left.push_back( id );
      if(p_aabb.max[axis] > split) right.push_back( id );
    }

    //count the number of primitives shared in left and right
    //vectors. If they share more than x% of the primitives,
    //give up and build leaf node
    int overlap = left.size()+right.size()-prims_ids.size();
    float shared_left = (float)std::abs(overlap)/left.size();
    float shared_right = (float)std::abs(overlap)/right.size();
    printf("%f%% and %f\%% of shared primitives\n", shared_left*100, shared_right*100);

    if(shared_left < 0.5f && shared_right < 0.5f)
    {
      printf("N left: %d, N right: %d\n--------------------------\n", left.size(), right.size());

      //compute new bounding boxes
      AABB left_aabb = aabb;
      left_aabb.max[axis] = split;

      AABB right_aabb = aabb;
      right_aabb.min[axis] = split;

      //recursively build children
      this->left = new KdNode(prims, left, left_aabb);
      this->right = new KdNode(prims, right, right_aabb);
    }
    else
    {
      this->left = this->right = NULL;
      this->prims = std::move( prims_ids );
      this->aabb = aabb;
    }
  }

}

void KdTree::build(const std::vector<Primitive::ptr>& prims)
{
  //this vector will contain indices to all primitives
  //we want to store inside the tree
  std::vector<int> prims_ids;
  for(int i = 0; i < prims.size(); ++i) prims_ids.push_back(i);

  AABB aabb; compute_aabb(prims, prims_ids, aabb);

  printf("AABB %s, %s\n", glm::to_string(aabb.min).c_str(),
                          glm::to_string(aabb.max).c_str());

  root = KdNode(prims, prims_ids, aabb);
}
