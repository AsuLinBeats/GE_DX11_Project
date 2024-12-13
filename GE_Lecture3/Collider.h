#pragma once
#include "GraphicMath.h"
#include "anime.h"
class AABB {
public:
    Vec3 maxDis;
    Vec3 minDis;
    std::vector<ANIMATED_VERTEX> vertices;
    std::vector<Vec3> verticesStatic;
    AABB() {}
    void reset()
    {
        maxDis = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        minDis = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    }
    void extend(const Vec3& p)
    {
        maxDis = Vec3::Max(maxDis, p);
        minDis = Vec3::Min(minDis, p);
    }

    bool intersects(const AABB& other) const {
        // check if intersects with other AABB
        return (minDis.x <= other.maxDis.x && maxDis.x >= other.minDis.x) &&
            (minDis.y <= other.maxDis.y && maxDis.y >= other.minDis.y) &&
            (minDis.z <= other.maxDis.z && maxDis.z >= other.minDis.z);
    }


    void update(Vec3 scaling, Vec3 rotating, Vec3 translating) {
        reset();
        if (vertices.empty()) {

            return;
        }
        // Compute the transformation matrix once
        Matrix transform = Matrix::worldTrans(scaling, rotating, translating);

        for (auto& vertex : vertices) {
            Vec3 worldV = (transform * homoVec(vertex.pos)).ToVec3();
            extend(worldV);
        }
    }

    void updateStatic(Vec3 scaling, Vec3 rotating, Vec3 translating) {
        reset();
        // Compute the transformation matrix once
        if (verticesStatic.empty()) {
            return;
        }
        Matrix transform = Matrix::worldTrans(scaling, rotating, translating);

        for (auto& vertex : verticesStatic) {
            Vec3 worldV = (transform * homoVec(vertex)).ToVec3();
            extend(worldV);
        }
    }
};


class Ray
    // for bullet
{
public:
    Vec3 o;
    Vec3 dir;
    Vec3 invdir;
    Ray() {}
    Ray(const Vec3 _o, const Vec3 _dir) {
        init(_o, _dir);
    }
    void init(const Vec3 _o, const Vec3 _dir) {
        o = _o;
        dir = _dir;
        invdir = Vec3(1.0f, 1.0f, 1.0f) / dir; // dir should not be 0
    }
    Vec3 at(const float t) {
        return (o + (dir * t));
    }

    // the ray will apply for bullet and collide with AABB box collider
    bool rayAABB(const AABB& box, float& t)
    {

        Vec3 tMin = (box.minDis - o) * invdir;
        Vec3 tMax = (box.maxDis - o) * invdir;

        // Ensure tMin is the near intersection and tMax is the far
        Vec3 nearHit = Vec3::Min(tMin, tMax);
        Vec3 farHit = Vec3::Max(tMin, tMax);

        // The largest near intersection
        float tNear = max(nearHit.x, max(nearHit.y, nearHit.z));
        // The smallest far intersection
        float tFar = min(farHit.x, min(farHit.y, farHit.z));

        // If tNear is greater than tFar, no intersection.
        // Also ensure tFar is >= 0, so the intersection isn't "behind" the ray.
        if (tNear > tFar || tFar < 0.0f) {
            return false;
        }

        // The ray intersects. If tNear < 0, it means we start inside the box, so take tFar.
        t = (tNear >= 0.0f) ? tNear : tFar;
        return true;
    }

};