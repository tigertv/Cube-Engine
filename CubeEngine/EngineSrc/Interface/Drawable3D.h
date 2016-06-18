#ifndef TZW_DRAWABLE3D_H
#define TZW_DRAWABLE3D_H

#include "../Math/AABB.h"
#include "Drawable.h"
#include "DepthPolicy.h"
namespace tzw {
class Drawable3D : public Drawable
{
public:
    Drawable3D();
    virtual ~Drawable3D();
    virtual AABB localAABB() const;
    virtual void setLocalAABB(const AABB &localAABB);
    virtual AABB getAABB();
    virtual NodeType getNodeType();
    virtual bool intersectByAABB(const AABB & other, vec3 &overLap);
    virtual Drawable3D * intersectByRay(const Ray & ray,vec3 &hitPoint);
    virtual void reCache();
    void reCacheAABB();
    DepthPolicy& getDepthPolicy();
    void setDepthPolicy(const DepthPolicy &depthPolicy);
protected:
    AABB m_localAABB;
    AABB m_worldAABBCache;
    DepthPolicy m_depthPolicy;
};

class Drawable3DGroup
{
public:
    Drawable3DGroup(Drawable3D ** obj,int count);
    Drawable3D *hitByRay(const Ray& ray,vec3 & hitPoint);
    bool hitByAABB(AABB & aabb, vec3 &minmalOverLap);
private:
    std::vector<Drawable3D * >m_list;
};

} // namespace tzw

#endif // TZW_DRAWABLE3D_H
