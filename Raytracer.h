using namespace std;

#include <iostream>
#include <vector>
#include <cmath>

#include "Vector.h"
#include "Objects.h"
#include "Camera.h"

const float SHADE_EPS = 1e-2f;

// DEBUG
float depthCurve(float z)
{
    float f = 10; // camera z far
    return 1.f - min(z / f, 1.f);
};

struct Scene
{
private:
    vector<Geometry *> geom;
    vector<PointLight *> lights;
    Sun *sun = nullptr;
    Sky *sky = nullptr;

public:
    void add(Geometry *g) { geom.push_back(g); }
    void add(PointLight *l) { lights.push_back(l); }
    void add(Sun *sun_ptr) { sun = sun_ptr; }
    void add(Sky *sky_ptr) { sky = sky_ptr; }

    Trace rayTrace(const Ray &ray, const Geometry *ignore_object = nullptr)
    {
        Trace trace(ray.dir);       // XXX sky needs ray direction, change ?
        float hit_z_min = INFINITY; // smallest found hit-depth

        for (Geometry *g : geom)
        {
            if (g == ignore_object)
                continue;

            Trace object_trace = g->intersect(ray, hit_z_min);
            if (object_trace.hit_object)
            {
                trace = object_trace;
                hit_z_min = trace.hit_z;
            }
        }
        return trace;
    }

    bool tracePass(const Ray &ray, const Geometry *ignore_object = nullptr, float z_MAX = INFINITY)
    {
        for (Geometry *g : geom)
        {
            if (g == ignore_object)
                continue;

            Trace object_trace = g->intersect(ray, z_MAX);
            if (object_trace.hit_object)
                return false;
        }
        return true;
    }

    void render(Camera &cam, Display &disp, bool postproc = false)
    {
        int W = disp.W;
        int H = disp.H;

        // NDC
        float xx, yy;

        vec3 rgb;

        // DEBUG
        bool DEPTHMAP = false;

        for (int i = 0; i < H; i++)
        {

            yy = (i + 0.5f - H / 2.f) / H * -1;

            for (int j = 0; j < W; j++)
            {
                xx = (j + 0.5f - W / 2.f) / W;

                Ray ray = cam.castRay(xx, yy);
                Trace trace = rayTrace(ray);

                if (DEPTHMAP) // DEBUG
                {
                    if (trace.hit_object)
                        disp.color_pixel(i, j, vec3{1, 1, 1} * depthCurve(trace.hit_z));
                    continue;
                }

                if (postproc)
                    disp.color_pixel_postproc(i, j, computeShade(trace));
                else
                    disp.color_pixel(i, j, computeShade(trace));
            }
        }
    }

    vec3 computeShade(const Trace &trace, int bounce_MAX = 1)
    {
        Geometry *obj = trace.hit_object;
        Geometry *ignore_object = obj;

        vec3 rgb{};

        if (!obj) // no object hit
        {
            if (sky)
                return sky->rgb(trace.dir);
            return rgb;
        }

        vec3 ambient_rgb{}, diffuse_rgb{}, specular_rgb{}, reflect_rgb{};
        float reflectivity = obj->shader.reflectivity;

        if (reflectivity > 0 && bounce_MAX > 0)
        {
            Ray reflect_ray = Ray({trace.hit_pos, trace.dir.bounce(trace.hit_normal)});
            Trace reflect_trace = rayTrace(reflect_ray, ignore_object);
            reflect_rgb = computeShade(reflect_trace, bounce_MAX - 1) * reflectivity;
            if (reflectivity == 1)
                return reflect_rgb;
        }

        for (PointLight *light : lights)
        {
            vec3 light_dir = (light->origin - trace.hit_pos);
            float light_dist = light_dir.length();
            light_dir /= light_dist;
            float diffuse_frac = light_dir.dot(trace.hit_normal);
            if (diffuse_frac > SHADE_EPS && tracePass(Ray(trace.hit_pos, light_dir), ignore_object, light_dist))
            {
                float diffuse_amt = light->intensity * diffuse_frac / (light_dist * light_dist);
                diffuse_rgb += light->rgb * diffuse_amt;
                float spec_frac = light_dir.dot(trace.dir.bounce(trace.hit_normal));
                if (spec_frac > SHADE_EPS)
                    specular_rgb += light->rgb * (light->intensity * pow(spec_frac, obj->shader.specular_exp));
            }
        }

        if (sun)
        {
            ambient_rgb += sun->rgb * sun->ambient;

            vec3 light_dir = sun->dir * -1;
            Ray light_ray = Ray(trace.hit_pos, light_dir);
            float diffuse_frac = -1 * sun->dir.dot(trace.hit_normal);
            if (diffuse_frac > SHADE_EPS && tracePass(light_ray, ignore_object))
            {
                float diffuse_amt = sun->intensity * diffuse_frac;
                diffuse_rgb += sun->rgb * diffuse_amt;
                float spec_frac = light_dir.dot(trace.dir.bounce(trace.hit_normal));
                if (spec_frac > SHADE_EPS)
                    specular_rgb += sun->rgb * (sun->intensity * pow(spec_frac, obj->shader.specular_exp));
            }
        }

        rgb += ambient_rgb;
        rgb += diffuse_rgb * obj->rgb /** obj->shader.diffuse*/;
        rgb = rgb * (1 - reflectivity) + reflect_rgb * reflectivity;
        rgb += specular_rgb * obj->shader.specular;
        return rgb.saturate();
    }
};