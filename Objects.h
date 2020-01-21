#pragma once
#include "Vector.h"

const float EPS = 1e-8;

struct Trace;

class Geometry
{
public:
    Shader shader;
    vec3 rgb;
    Geometry(vec3 rgb = {1, 0, 0}) : rgb(rgb){};
    virtual Trace intersect(const Ray &ray, float z_MAX = INFINITY, float z_MIN = 0) = 0;
};

struct Trace
{
    Geometry *hit_object;
    float hit_z;
    vec3 hit_pos;
    vec3 hit_normal;
    vec3 dir;

    Trace() : hit_object(nullptr) {}
    Trace(vec3 dir) : hit_object(nullptr), dir(dir) {}
};

class Sphere : public Geometry
{
public:
    vec3 origin;
    float r;

    Sphere(vec3 origin, float r, vec3 rgb = {0, 0, 1}) : Geometry(rgb), origin(origin), r(r){};

    Trace intersect(const Ray &ray, float z_MAX = INFINITY, float z_MIN = 0) override
    {
        Trace trace;
        float q = (ray.origin - origin).dot(ray.dir);
        float D = r - (ray.origin - origin).length2() + q * q;
        if (D < 0)
            return trace;

        float d = sqrt(D);

        float t = -d - q;
        if (t < 0)
            t = d - q;
        if (t < z_MIN || t >= z_MAX)
            return trace;

        trace.hit_object = this;
        trace.hit_z = t;
        trace.hit_pos = ray.origin + ray.dir * t;
        trace.hit_normal = (trace.hit_pos - origin) / r;
        trace.dir = ray.dir;

        return trace;
    }
};

class Tri : public Geometry
{
public:
    vec3 a, b, c;
    Tri(vec3 a, vec3 b, vec3 c, vec3 rgb = {0.f, 1.f, 0.f}) : Geometry(rgb), a(a), b(b), c(c){};

    Trace intersect(const Ray &ray, float z_MAX = INFINITY, float z_MIN = 0) override
    {
        Trace trace;
        vec3 ab = b - a;
        vec3 ac = c - a;
        const vec3 pvec = ray.dir.cross(ab);
        float det = ab.dot(pvec);
        float det_inv = 1 / det;

        // if(culling && det < eps) return false;
        if (abs(det) < EPS)
            return trace;

        vec3 tvec = ray.origin - a;
        float u = tvec.dot(pvec) * det_inv;
        if (u < 0 || u > 1)
            return trace;

        vec3 qvec = tvec.cross(ab);
        float v = ray.dir.dot(qvec) * det_inv;
        if (v < 0 || v > 1)
            return trace;

        float t = ab.dot(qvec) * det_inv;

        return trace;
    }
};

class InfinitePlane : public Geometry
{
private:
    vec3 normal_;
    float dist_;

public:
    const vec3 &normal;
    const float &dist;
    InfinitePlane(vec3 normal, float dist, vec3 rgb) : Geometry(rgb), normal_(normal), dist_(dist), normal(normal_), dist(dist_)
    {
        normal_.normalize();
    }

    Trace intersect(const Ray &ray, float z_MAX = INFINITY, float z_MIN = 0) override
    {
        Trace trace;
        float d = normal.dot(ray.dir);
        if (d >= 0)
            return trace; // XXX backculling per default on, add z_MIN

        float t = (dist + normal.dot(ray.origin)) / -d;
        if (t >= z_MAX)
            return trace;

        trace.hit_object = this;
        trace.hit_z = t;
        trace.hit_pos = ray.origin + ray.dir * t;
        trace.hit_normal = normal;
        trace.dir = ray.dir;

        return trace;
    }
};

struct Light
{
    vec3 origin, rgb;
    float intensity;
    Light(vec3 origin, float intensity = 100, vec3 rgb = {1, 1, 1}) : origin(origin), intensity(intensity), rgb(rgb) {}

    // vec3 lightDir(const vec3 &obj_point)
    // {
    // }
    // float distance()
    // {
    // }

    // float computeIntensity()
    // {
    // }
};

class PointLight
{
private:
    vec3 origin_;
    vec3 rgb_;
    float intensity_;

public:
    const vec3 &origin;
    const vec3 &rgb;
    const float &intensity;
    PointLight(vec3 origin, float intensity = 100, vec3 rgb = {1, 1, 1}) : origin_(origin), intensity_(intensity), rgb_(rgb), origin(origin_), intensity(intensity_), rgb(rgb_) {}
};

class Sun
{
private:
    vec3 dir_;
    vec3 rgb_;
    float ambient_;
    float intensity_;

public:
    const vec3 &dir;
    const vec3 &rgb;
    const float &ambient;
    const float &intensity;

    Sun(vec3 dir = {0, -1, 0}, float intensity = 0.8, vec3 rgb = {1, 0.99, 0.95}, float ambient = 0.1) : dir_(dir), intensity_(intensity), rgb_(rgb), ambient_(ambient), dir(dir_), intensity(intensity_), rgb(rgb_), ambient(ambient_)
    {
        dir_.normalize();
    }

    // Light(vec3 origin = {0, 100, 0}, float r = 10, vec3 = {1, 1, 1}) : Sphere(origin, 10, {1, 1, 1}) {}
    //Light(Sphere bulb, vec3 rgb = {1.f, 1.f, 1.f}, float luminosity = 1) : Sphere(bulb) { bulb.luminosity = luminosity; };
};

class Sky
{
private:
    vec3 rgb_top, rgb_mid, rgb_bottom;

public:
    Sky(vec3 rgb_top, vec3 rgb_mid, vec3 rgb_bottom) : rgb_top(rgb_top), rgb_mid(rgb_mid), rgb_bottom(rgb_bottom) {}

    vec3 rgb(vec3 ray_dir)
    {
        float cos = ray_dir.dot({0, 1, 0});
        if (cos >= 0)
            return rgb_top * cos + rgb_mid * (1 - cos);
        else
            return rgb_bottom * -cos + rgb_mid * (1 + cos);
    }
};