#pragma once

#include <iostream>
#include <cmath>

using namespace std;

inline float clamp(float val)
{
    return val <= 0 ? 0 : val <= 1 ? val : 1;
}

struct vec3
{
    float x, y, z;

    float length() { return sqrt(x * x + y * y + z * z); };
    float length2() { return x * x + y * y + z * z; };

    float dot(const vec3 &obj) const { return x * obj.x + y * obj.y + z * obj.z; }
    vec3 cross(const vec3 &obj) const
    {
        return {y * obj.z - obj.y * z,
                z * obj.x - obj.z * x,
                x * obj.y - obj.x * y};
    }

    inline vec3 bounce(const vec3 &n) const
    {
        return *this + n * (n.dot(*this) * -2);
    }

    vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    vec3 operator/(float s) const { return {x / s, y / s, z / s}; }
    vec3 operator*(const vec3 &obj) const { return {x * obj.x, y * obj.y, z * obj.z}; }
    vec3 operator+(const vec3 &obj) const { return {x + obj.x, y + obj.y, z + obj.z}; }
    vec3 operator-(const vec3 &obj) const { return {x - obj.x, y - obj.y, z - obj.z}; }

    // --- In Place
    vec3 &operator+=(const vec3 &obj)
    {
        x += obj.x;
        y += obj.y;
        z += obj.z;
        return *this;
    }
    vec3 &operator*=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    vec3 &operator/=(float s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    vec3 &normalize() { return this->operator/=(length()); }
    vec3 &saturate()
    {
        x = clamp(x);
        y = clamp(y);
        z = clamp(z);
        return *this;
    }
};

struct Shader
{
    float diffuse, diffuse_exp, specular;
    int specular_exp;
    float reflectivity;
    Shader(float diffuse = 0.18, float diffuse_exp = 4, float specular = 0.05, int specular_exp = 24, float reflectivity = 0) : diffuse(diffuse), specular(specular), diffuse_exp(diffuse_exp), specular_exp(specular_exp), reflectivity(reflectivity) {}
};

struct Ray
{
    vec3 origin, dir;
    Ray(){};
    Ray(vec3 origin, vec3 dir) : origin(origin), dir(dir){};
};

ostream &operator<<(ostream &os, const vec3 &v)
{
    os << "x: " << v.x << "\ty: " << v.y << "\tz: " << v.z << endl;
    return os;
}