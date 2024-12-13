#pragma once
#include "GraphicMath.h"
#include "anime.h"
// TODO NEED TO REFACTOR THESE RUBBISH CODE IN COLLIDER.H, CHARACTER.H AND GAME.H TO MAKE IT MORE LOGIC AND STRUCTURED
class AABB {
    // AABB focus on bounds only
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

    //bool intersects(const AABB& other) const {
    //    return (minDis.x <= other.maxDis.x && maxDis.x >= other.minDis.x) &&
    //        (minDis.y <= other.maxDis.y && maxDis.y >= other.minDis.y) &&
    //        (minDis.z <= other.maxDis.z && maxDis.z >= other.minDis.z);
    //}
};

class SphereBound
    // distinguish with sphere in mesh.h
{
public:
    Vec3 centre;
    float radius;
    void init(Vec3 _centre, float _radius) {
        centre = _centre;
        radius = _radius;
    }
};


class Ray
    // from slides
{
public:
    Vec3 o;
    Vec3 dir;
    Vec3 invdir; // 1/dir
    Ray() {
    }
    Ray(const Vec3 _o, const Vec3 _dir) {
        init(_o, _dir);
    }
    void init(const Vec3 _o, const Vec3 _dir) {
        o = _o;
        dir = _dir;
        invdir = Vec3(1.0f, 1.0f, 1.0f) / dir;
    }
    Vec3 at(const float t) {
        return (o + (dir * t));
    }

    //bool rayAABB(const Ray& r, float& t)
    //{
    //    Vec3 s = (min - r.o) * r.invdir;
    //    Vec3 l = (max - r.o) * r.invdir;
    //    Vec3 s1 = Min(s, l);
    //    Vec3 l1 = Max(s, l);
    //    float ts = max(s1.x, max(s1.y, s1.z));
    //    float tl = min(l1.x, min(l1.y, l1.z));
    //    t = min(ts, tl);
    //    return (ts < tl);
    //}

};


class Collider {
public:
    // use a collider to accepts objects and check collision rather than defined indiviually
    AABB aabb1;
    AABB aabb2;

    SphereBound sp1;
    SphereBound sp2;

    Ray ray;
    // setting all boundary

    // set AABB to one object:
    void SetAABB() {
        
    }
    // set Sphere to one object:
    void SetSphere1(Vec3 _centre, float _radius) {
        sp1.init(_centre, _radius);
    }
    void SetSphere2(Vec3 _centre, float _radius) {
        sp2.init(_centre, _radius);
    }
    void SetRay(const Vec3 _o, const Vec3 _dir) {
        ray.init(_o, _dir); // only bullet need use ray, so define 1 ray for now.
    }
    // checking for every possible situation
    
    bool AABBs(AABB& aabb1, AABB& aabb2) {
        // case for two AABB 
        return (aabb1.minDis.x <= aabb2.maxDis.x && aabb1.maxDis.x >= aabb2.minDis.x) &&
            (aabb1.minDis.y <= aabb2.maxDis.y && aabb1.maxDis.y >= aabb2.minDis.y) &&
            (aabb1.minDis.z <= aabb2.maxDis.z && aabb1.maxDis.z >= aabb2.minDis.z);
    };

    bool SphereS(SphereBound& sp1, SphereBound& sp2){
        // case for two sphere
        // the distance between two centre
        Vec3 diff = sp1.centre - sp2.centre;

        // squared distance between centers
        float distSq = diff.Dot(diff);

        // sum of radius
        float radiusSum = sp1.radius + sp2.radius;

        if (distSq <= (radiusSum * radiusSum)) {
            return true;
        } else { return false; }
    };

    bool AABBSphere(SphereBound& sp1, AABB& aabb1){
        // case for AABB and sphere
        float px = max(aabb1.minDis.x, min(sp1.centre.x, aabb1.maxDis.x));
        float py = max(aabb1.minDis.y, min(sp1.centre.y, aabb1.maxDis.y));
        float pz = max(aabb1.minDis.z, min(sp1.centre.z, aabb1.maxDis.z));
        Vec3 p(px, py, pz);
        float diffx = (sp1.centre.x - px) * (sp1.centre.x - px);
        float diffy = (sp1.centre.y - py) * (sp1.centre.y - py);
        float diffz = (sp1.centre.z - pz) * (sp1.centre.z - pz);
        float radS = sp1.radius * sp1.radius;
       // float PenetrationZ = (p - sp1.centre) - rad;
        float distSq = diffx + diffy + diffz;

        return (distSq <= radS);

    };

    //bool RayAABB(Ray& ray, AABB& aabb1){
    //    //case for AABB and ray
    //    auto checkAxis = [&](float start, float dir, float minVal, float maxVal) {
    //        if (std::fabs(dir)) {
    //            // TODO NEED TO SOLVE PARALLEL SITUATION
    //        }
    //        else {
    //            float ood = 1.0f / dir;
    //            float t1 = (minVal - start) * ood;
    //            float t2 = (maxVal - start) * ood;
    //            if (t1 > t2) std::swap(t1, t2);

    //            if (t1 > aabb1.tMin) tMin = t1;
    //            if (t2 < aabb1.tMax) tMax = t2;

    //            if (tMin > tMax) return false;
    //        }
    //        return true;
    //        };

    //    if (!checkAxis(r.origin.x, r.direction.x, box.minDis.x, box.maxDis.x)) return false;
    //    if (!checkAxis(r.origin.y, r.direction.y, box.minDis.y, box.maxDis.y)) return false;
    //    if (!checkAxis(r.origin.z, r.direction.z, box.minDis.z, box.maxDis.z)) return false;

    //    // Check if intersection interval is in front of the ray
    //    if (tMax < 0.0f) return false;

    //    return true;
    //};
    bool RaySphere(Ray& ray, SphereBound& sp1){
        // case for Sphere and Ray
        Vec3 oc = ray.o - sp1.centre;
        float a = ray.dir.Dot(ray.dir);
        float b = 2.0f * oc.Dot(ray.dir);
        float c = oc.Dot(oc) - sp1.radius * sp1.radius;

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) return false;

        float sqrtD = std::sqrt(discriminant);
        float t0 = (-b - sqrtD) / (2.0f * a);
        float t1 = (-b + sqrtD) / (2.0f * a);

        // If both intersections are behind the origin, no forward hit
        return (t0 >= 0.0f || t1 >= 0.0f);
    };
};