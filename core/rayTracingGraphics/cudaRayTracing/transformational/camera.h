#ifndef CAMERAH
#define CAMERAH

#include "math/ray.h"

namespace cuda {

    class camera {
    public:
        ray viewRay;
        vec4 horizontal;
        vec4 vertical;

        float aspect{ 1.0f };
        float matrixScale{ 0.04f };
        float matrixOffset{ 0.05f };
        float focus{ 0.049f };
        float apertura{ 0.005f };

        __host__ __device__ camera();

        __host__ __device__ camera(
            const ray viewRay,
            float aspect,
            float matrixScale,
            float matrixOffset,
            float focus,
            float apertura);

        __host__ __device__ camera(const ray viewRay, float aspect);

        __device__ ray getPixelRay(float u, float v, curandState* local_rand_state);
        __host__ __device__ ray getPixelRay(float u, float v);

        __host__ __device__ ray getViewRay();

        __host__ __device__ void setViewRay(const ray& viewRay);
        __host__ __device__ void setFocus(const float& focus);

        static camera* create(const ray& viewRay, float aspect);
        static void destroy(camera* cam);

        static void reset(camera* cam, const ray& viewRay, float aspect);
        static void setViewRay(camera* cam, const ray& viewRay);
        static void setFocus(camera* cam, const float& focus);

        static camera copyToHost(camera* pDevice);
    };
}
#endif
