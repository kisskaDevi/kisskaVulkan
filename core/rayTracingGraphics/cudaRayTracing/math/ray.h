#ifndef RAYH
#define RAYH
#include "vec4.h"

namespace cuda {

class ray {
private:
    vec4f origin;
    vec4f direction;

public:
    __host__ __device__ ray() {}
    __host__ __device__ ray(const vec4f& origin, const vec4f& direction) : origin(origin), direction(normal(direction)){}
    __host__ __device__ vec4f getOrigin() const { return origin; }
    __host__ __device__ vec4f getDirection() const { return direction; }
    __host__ __device__ vec4f point(const float& t) const { return origin + t * direction; }
};

}

#endif