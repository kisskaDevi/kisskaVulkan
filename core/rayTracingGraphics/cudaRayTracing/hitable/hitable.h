#ifndef HITABLEH
#define HITABLEH

#include "math/box.h"
#include "materials/material.h"

namespace cuda::rayTracing {

struct HitRecord{
    vec4f point{0.0f, 0.0f, 0.0f, 1.0f };
    vec4f normal{0.0f, 0.0f, 0.0f, 0.0f };
    vec4f color{1.0f, 1.0f, 1.0f, 1.0f };
    Properties props;
    ray scattering;
    uint32_t rayDepth{0};
    float lightIntensity{1.0};
};

class Hitable;

struct HitCoords{
    float tmin{0.01f};
    float tmax{std::numeric_limits<float>::max()};
    float u{0.0f};
    float v{0.0f};
    Hitable* obj{nullptr};

    __host__ __device__ bool check() const {
        return obj && tmax != std::numeric_limits<float>::max();
    }
};

class Hitable {
public:
    __host__ __device__ virtual ~Hitable() {};
    __host__ __device__ virtual bool hit(const ray& r, HitCoords& coords) const = 0;
    __host__ __device__ virtual void calcHitRecord(const ray& r, const HitCoords& coords, HitRecord& rec) const = 0;
    __host__ __device__ virtual box getBox() const = 0;

    static void destroy(Hitable* dpointer);
};

}
#endif
