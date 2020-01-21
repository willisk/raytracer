#pragma once
using namespace std;

// #include <iostream>
#include <vector>
#include <cmath>

#include "Vector.h"

const float PI = 3.141592653589793f;
const float GAMMA_CORRECT = 0.45454545454f;

struct Camera
{
    Ray eye;
    float fov, z_near, aspect, f_dist;

    vec3 right, up;

    Camera(Ray eye = {{0, 0, 0}, {0, 0, -1}}, int fov = 90, float z_near = 0.1, float aspect = 1, float f_dist = 1, vec3 up = {0, 1, 0}) : eye(eye), fov(fov), aspect(aspect), z_near(z_near), f_dist(f_dist), up(up) { init(); };

    void init()
    {
        float size = f_dist * tan(PI * fov / (2 * 180));
        eye.dir.normalize();
        right = eye.dir.cross(up);
        if (right.length2() < 1e-8)
            right = {0, 0, 1};
        up = right.cross(eye.dir).normalize() * size;
        right = right.normalize() * (aspect * size);
    }

    Ray castRay(float xx, float yy)
    {
        Ray ray;
        ray.dir = eye.dir + right * xx + up * yy;
        ray.dir.normalize();
        ray.origin = eye.origin + ray.dir * z_near;
        return ray;
    }
};

class Display
{
private:
    int W_, H_;
    vector<unsigned char> pixels_;

public:
    const int &W, &H;

    Display(int W, int H) : W_(W), H_(H), pixels_(W * H * 3, 0), W(W_), H(H_){};

    vector<unsigned char> &pixels()
    {
        return pixels_;
    };

    inline void color_pixel(int i, int j, vec3 rgb)
    {
        pixels_[(i * W + j) * 3 + 0] = (unsigned char)(rgb.x * 255.f);
        pixels_[(i * W + j) * 3 + 1] = (unsigned char)(rgb.y * 255.f);
        pixels_[(i * W + j) * 3 + 2] = (unsigned char)(rgb.z * 255.f);
    }
    inline void color_pixel_postproc(int i, int j, vec3 rgb)
    {
        pixels_[(i * W + j) * 3 + 0] = (unsigned char)(postProc(rgb.x) * 255.f);
        pixels_[(i * W + j) * 3 + 1] = (unsigned char)(postProc(rgb.y) * 255.f);
        pixels_[(i * W + j) * 3 + 2] = (unsigned char)(postProc(rgb.z) * 255.f);
    }
    // GAMMA CORRECT
    inline float postProc(float f)
    {
        return pow(f, GAMMA_CORRECT);
    }
};

// DEBUG
ostream &operator<<(ostream &os, Display &disp)
{
    int H = disp.H;
    int W = disp.W;

    vector<unsigned char> &im = disp.pixels();

    os << disp.W << "x" << disp.H << endl;

    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            float gray = im[(i * W + j) * 3 + 0] + im[(i * W + j) * 3 + 1] + im[(i * W + j) * 3 + 2];
            os << int(gray / 3 * 7) << ' ';
        }
        os << endl;
    }
    return os;
}