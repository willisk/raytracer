#include <iostream>

#define _CRT_SECURE_NO_WARNINGS 1
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Raytracer.h"
// #include "Objects.h"
// #include "Vector.h"

int main()
{

    // -----------------------
    // Camera
    float z_near = 1;
    float fov = 90;

    vec3 cam_pos = {-2, 0.8, 2};
    vec3 cam_dir = {0.3, -0.1, -1};

    Ray view = {cam_pos, cam_dir};
    Camera cam(view, fov, z_near);

    // -----------------------
    // Geometry
    Sphere sp({0, 0, -5}, 1, {0.3, 0.5, 1});
    sp.shader.reflectivity = 1;

    Sphere sp2 = sp;
    sp2.origin.x += 3;
    sp2.shader.reflectivity = 0.5;

    InfinitePlane wall_down({0, 1, 0}, 2, {0.7, 0.8, 0.1});
    InfinitePlane wall_up({0, -1, 0}, 5, {0.6, 0.3, 0.1});
    InfinitePlane wall_right({-1, 0, 0}, 6, {0.9, 0.1, 0.1});
    InfinitePlane wall_left({1, 0, 0}, 6, {0.9, 0.6, 0.1});
    InfinitePlane wall_back({0, 0, 1}, 12, {0.2, 0.4, 1});

    // -----------------------
    // Lighting
    vec3 sun_dir = {0.6, -1, 0};
    Sun sun = Sun(sun_dir.normalize()); //, sun_rgb, sun_ambient);

    PointLight lamp({3, 3, -5}, 20);
    PointLight lamp2({1, 3, -8}, 20, {1, 0, 1});

    Sky sky({0.35, 0.55, 0.7}, {0.97, 0.95, 0.95}, {0, 0, 0.15});

    // -----------------------
    // Scene
    Scene scene;
    scene.add(&sky);
    // scene.add(&sun);
    scene.add(&lamp);
    // scene.add(&lamp2);

    scene.add(&sp);
    // scene.add(&sp2);

    scene.add(&wall_down);
    scene.add(&wall_right);
    scene.add(&wall_left);
    scene.add(&wall_up);
    scene.add(&wall_back);

    Tri t1({1, 0, -4},
           {-1.5, 1, -5},
           {-1, 4, -4});
    Tri t2({{-1, -2, -5},
            {-1.5, -3, -5},
            {3, -4, -7}});
    // scene.add(&t1);
    // scene.add(&t2);

    // -----------------------
    // Display
    Display disp(512, 512);

    // -----------------------
    // Camera 2
    cam_pos = {-4, 0.5, -4};
    cam_dir = {1, -0.2, -.25};
    view = {cam_pos, cam_dir};
    Camera cam_left(view, fov);
    Display disp_left(128, 128);

    // -----------------------
    // Camera 3
    cam_pos = {1, 4, -5};
    cam_dir = {0, -1, 0};
    view = {cam_pos, cam_dir};
    Camera cam_up(view, 90, 0, 1, 2);
    Display disp_up(128, 128);

    scene.render(cam, disp, false);
    scene.render(cam_left, disp_left);
    scene.render(cam_up, disp_up);
    stbi_write_png("render.png", disp.W, disp.H, 3, &disp.pixels()[0], 0);
    stbi_write_png("render_left.png", disp_left.W, disp_left.H, 3, &disp_left.pixels()[0], 0);
    stbi_write_png("render_up.png", disp_up.W, disp_up.H, 3, &disp_up.pixels()[0], 0);
}