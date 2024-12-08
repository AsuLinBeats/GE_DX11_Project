#pragma once
#include "GraphicMath.h"

class AABB {
public:
    Vec3 maxDis;
    Vec3 minDis;
    AABB() { reset(); }
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
};
//
//class Ray
//    // for bullet
//{
//public:
//    Vec3 o;
//    Vec3 dir;
//    Vec3 invdir;
//    Ray() {}
//    Ray(const Vec3 _o, const Vec3 _dir) {
//        init(_o, _dir);
//    }
//    void init(const Vec3 _o, const Vec3 _dir) {
//        o = _o;
//        dir = _dir;
//        invdir = Vec3(1.0f, 1.0f, 1.0f) / dir;
//    }
//    Vec3 at(const float t) {
//        return (o + (dir * t));
//    }
//
//    bool rayAABB(const Ray& r, float& t)
//    {
//        Vec3 s = (min - r.o) * r.invdir;
//        Vec3 l = (max - r.o) * r.invdir;
//        Vec3 s1 = Min(s, l);
//        Vec3 l1 = Max(s, l);
//        float ts = max(s1.x, max(s1.y, s1.z));
//        float tl = min(l1.x, min(l1.y, l1.z));
//        t = min(ts, tl);
//        return (ts < tl);
//    }
//
//};