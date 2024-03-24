#include "triangle.h"
#include "operations.h"

namespace cuda {

namespace {
    __host__ __device__ float det3(const vec4& a, const vec4& b, const vec4& c) {
        return a.x() * b.y() * c.z() + b.x() * c.y() * a.z() + c.x() * a.y() * b.z() -
            (a.x() * c.y() * b.z() + b.x() * a.y() * c.z() + c.x() * b.y() * a.z());
    }

    __host__ __device__ vec4 max(const vec4& v1, const vec4& v2) {
        return vec4(v1.x() >= v2.x() ? v1.x() : v2.x(),
                    v1.y() >= v2.y() ? v1.y() : v2.y(),
                    v1.z() >= v2.z() ? v1.z() : v2.z(),
                    v1.w() >= v2.w() ? v1.w() : v2.w());
    }

    __host__ __device__ vec4 min(const vec4& v1, const vec4& v2) {
        return vec4(v1.x() < v2.x() ? v1.x() : v2.x(),
                    v1.y() < v2.y() ? v1.y() : v2.y(),
                    v1.z() < v2.z() ? v1.z() : v2.z(),
                    v1.w() < v2.w() ? v1.w() : v2.w());
    }
}

__host__ __device__ triangle::triangle(const size_t& i0, const size_t& i1, const size_t& i2, const vertex* vertexBuffer)
    : index0(i0), index1(i1), index2(i2), vertexBuffer(vertexBuffer) {
};

__host__ __device__ bool triangle::hit(const ray& r, float tMin, float tMax, hitCoords& coord) const {
    const vec4 a = vertexBuffer[index1].point - r.getOrigin();
    const vec4 b = vertexBuffer[index1].point - vertexBuffer[index2].point;
    const vec4 c = vertexBuffer[index1].point - vertexBuffer[index0].point;
    const vec4 d = r.getDirection();

    float det = det3(d, b, c);
    if (det == 0.0f) {
        return false;
    }
    det = 1.0f / det;

    const float t = det3(a, b, c) * det;
    if(t > tMax || t < tMin){
        return false;
    }

    const float u = det3(d, a, c) * det;
    const float v = det3(d, b, a) * det;
    const float s = 1.0f - u - v;

    if (u >= 0.0f && v >= 0.0f && s >= 0.0f) {
        coord = {t,u,v};
        return true;
    }

    return false;
}

__host__ __device__ void triangle::calcHitRecord(const ray& r, const hitCoords& coord, hitRecord& rec) const {
    const float s = 1.0f - coord.u - coord.v;
    rec.point = r.point(coord.t);
    rec.normal = normal(coord.v * vertexBuffer[index0].normal + coord.u * vertexBuffer[index2].normal + s * vertexBuffer[index1].normal);
    rec.color = coord.v * vertexBuffer[index0].color + coord.u * vertexBuffer[index2].color + s * vertexBuffer[index1].color;
    rec.props = {
        coord.v * vertexBuffer[index0].props.refractiveIndex + coord.u * vertexBuffer[index2].props.refractiveIndex + s * vertexBuffer[index1].props.refractiveIndex,
        coord.v * vertexBuffer[index0].props.refractProb + coord.u * vertexBuffer[index2].props.refractProb + s * vertexBuffer[index1].props.refractProb,
        coord.v * vertexBuffer[index0].props.fuzz + coord.u * vertexBuffer[index2].props.fuzz + s * vertexBuffer[index1].props.fuzz,
        coord.v * vertexBuffer[index0].props.angle + coord.u * vertexBuffer[index2].props.angle + s * vertexBuffer[index1].props.angle,
        coord.v * vertexBuffer[index0].props.emissionFactor + coord.u * vertexBuffer[index2].props.emissionFactor + s * vertexBuffer[index1].props.emissionFactor,
        coord.v * vertexBuffer[index0].props.absorptionFactor + coord.u * vertexBuffer[index2].props.absorptionFactor + s * vertexBuffer[index1].props.absorptionFactor
    };
}

__global__ void createKernel(triangle* tr, const size_t i0, const size_t i1, const size_t i2, const vertex* vertexBuffer) {
    tr = new (tr) triangle(i0, i1, i2, vertexBuffer);
}

void triangle::create(triangle* dpointer, const triangle& host){
    createKernel<<<1,1>>>(dpointer, host.index0, host.index1, host.index2, host.vertexBuffer);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
}

__global__ void destroyKernel(triangle* p) {
    p->~triangle();
}

void triangle::destroy(triangle* dpointer){
    destroyKernel<<<1,1>>>(dpointer);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
}

box triangle::calcBox() const {
    box bbox;
    bbox.min = min(vertexBuffer[index0].point, min(vertexBuffer[index1].point, vertexBuffer[index2].point));
    bbox.max = max(vertexBuffer[index0].point, max(vertexBuffer[index1].point, vertexBuffer[index2].point));
    return bbox;
}
}
