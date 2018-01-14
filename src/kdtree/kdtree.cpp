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
bool KdNode::intersect(const Ray& r, const std::vector<Primitive::ptr>& prims,
                        Isect& target) const
{
  if( is_leaf() )
  {
    bool hit = false;
    for(auto id : prims_ids)
    {
      Primitive::ptr p = prims[id];
      Isect p_isect; p->intersect(r, p_isect);

      if( p_isect.is_valid() && p_isect.t < target.t)
      {
        target = p_isect;
        hit = true;
      }
    }
    return hit;
  }

  //not a leaf
  float tmin, tmax;
  if(!aabb.intersect(r, tmin, tmax)) return false;

  //if ray is parallel to the splitting plane, there's no problem:
  //tsplit will be +INF, so it will be >= tmax (and won't be <= tmin),
  //thus we'll explore the NEAR box only, which is correct.
  //Problems may occur if the ray's origin is exactly on the splitting
  //plane, in which case we simply shift it a bit to the right.
  float split = this->split; if(split == r.o[this->axis]) split += 0.000001f;
  float tsplit = (split - r.o[this->axis])/r.d[this->axis];

  //define NEAR and FAR boxes.
  //this is defined by checking at which side of the
  //splitting plane the origin is. If the ray is exactly
  //on the origin, the direction defines which plane is near
  KdNode *near, *far;
  if( r.o[this->axis] < split ||
      (r.o[this->axis] == split && r.d[this->axis] < 0) )
  {
    near = this->left;
    far = this->right;
  }
  else
  {
    near = this->right;
    far = this->left;
  }

  //decide whether we must traverse NEAR only, FAR only or both
  //The case where both boxes should be visited (tmin < tsplit
  //&& tsplit < tmax) is equivalent to !near_only and !far_only.
  //Technically we should require that tsplit > 0 for far_only
  //to be true, but adding this clause won't change anything.
  //
  //Border cases, where tmin = tmax, are correctly also correctly
  //covered by this logic
  //
  //There's a subtlety here: everytime tsplit < 0, both near_only
  //and far_only are TRUE. However, as near_only is the first condition
  //tested, it returns true and we test only the nearest box, which is
  //CORRECT
  bool near_only = tsplit >= tmax || tsplit <= 0;
  bool far_only = tsplit <= tmin;

  if( near_only ) return near->intersect(r, prims, target);
  else if( far_only ) return far->intersect(r, prims, target);
  else
  {
    //if we found an intersection in the near
    //box, stop looking
    if( near->intersect(r, prims, target) ) return true;
    else return far->intersect(r, prims, target);
  }
}

bool KdTree::intersect(const Ray& r, const std::vector<Primitive::ptr>& prims,
                        Isect& isect) const
{
  isect.t = FLT_MAX;
  return root.intersect(r, prims, isect);
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

  //split at median
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
    this->prims_ids = std::move( prims_ids );
    this->aabb = aabb;
    this->axis = -1;
    this->split = -1.0f;
  }
  else
  {
    //choose splitting point
    float split; int axis;
    split = split_at(prims, prims_ids, aabb, axis);

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
    //TODO: use Surface Area Heuristic!
    int overlap = left.size()+right.size()-prims_ids.size();
    float shared_left = (float)std::abs(overlap)/left.size();
    float shared_right = (float)std::abs(overlap)/right.size();

    if(shared_left < 0.5f && shared_right < 0.5f)
    {
      //compute new bounding boxes
      AABB left_aabb = aabb;
      left_aabb.max[axis] = split;

      AABB right_aabb = aabb;
      right_aabb.min[axis] = split;

      this->split = split;
      this->axis = axis;
      this->aabb = aabb;

      //recursively build children
      this->left = new KdNode(prims, left, left_aabb);
      this->right = new KdNode(prims, right, right_aabb);
    }
    else
    {
      this->left = this->right = NULL;
      this->prims_ids = std::move( prims_ids );
      this->aabb = aabb;
      this->axis = -1;
      this->split = -1.0f;
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
  root = KdNode(prims, prims_ids, aabb);
}
