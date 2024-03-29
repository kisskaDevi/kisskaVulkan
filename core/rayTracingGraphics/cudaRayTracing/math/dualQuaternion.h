#ifndef DUALQUATERNION_H
#define DUALQUATERNION_H

#include "quaternion.h"

namespace cuda {

template<typename type>
class dualQuaternion
{
private:
    quaternion<type> p;
    quaternion<type> q;
public:
    __host__ __device__ dualQuaternion();
    __host__ __device__ dualQuaternion(const dualQuaternion<type>& other);
    __host__ __device__ dualQuaternion(const quaternion<type>& p, const quaternion<type>& q);
    __host__ __device__ dualQuaternion<type>& operator=(const dualQuaternion<type>& other);
    __host__ __device__ ~dualQuaternion();

    __host__ __device__ quaternion<type>            rotation()const;
    __host__ __device__ quaternion<type>            translation()const;

    __host__ __device__ bool                        operator==(const dualQuaternion<type>& other)const;
    __host__ __device__ bool                        operator!=(const dualQuaternion<type>& other)const;
    __host__ __device__ dualQuaternion<type>        operator+ (const dualQuaternion<type>& other)const;
    __host__ __device__ dualQuaternion<type>        operator- (const dualQuaternion<type>& other)const;
    __host__ __device__ dualQuaternion<type>        operator* (const dualQuaternion<type>& other)const;
    __host__ __device__ dualQuaternion<type>&       operator+=(const dualQuaternion<type>& other);
    __host__ __device__ dualQuaternion<type>&       operator-=(const dualQuaternion<type>& other);
    __host__ __device__ dualQuaternion<type>&       operator*=(const dualQuaternion<type>& other);

    __host__ __device__ dualQuaternion<type>& normalize();
    __host__ __device__ dualQuaternion<type>& conjugate();
    __host__ __device__ dualQuaternion<type>& invert();

    template<typename T> __host__ __device__ friend dualQuaternion<T>   normalize(const dualQuaternion<T>& quat);
    template<typename T> __host__ __device__ friend dualQuaternion<T>   conjugate(const dualQuaternion<T>& quat);
    template<typename T> __host__ __device__ friend dualQuaternion<T>   invert(const dualQuaternion<T>& quat);

    template<typename T> __host__ __device__ friend dualQuaternion<T> operator* (const T& c, const dualQuaternion<T>& quat);
    template<typename T> friend std::ostream& operator<< (std::ostream& out, const dualQuaternion<T>& quat);

    template<typename T> __host__ __device__ friend dualQuaternion<T> convert(const quaternion<T>& rotation, const quaternion<T>& translation);

    //template<typename T> friend dualQuaternion<T> convert(const glm::mat<4, 4, T, glm::defaultp>& SE3);
    //template<typename T> friend glm::mat<4, 4, T, glm::defaultp> convert(const dualQuaternion<T>& quat);

    template<typename T> __host__ __device__ friend dualQuaternion<T> slerp(const dualQuaternion<T>& quat1, const dualQuaternion<T>& quat2, const T& t);
};

template<typename type>
__host__ __device__ dualQuaternion<type>::dualQuaternion()
{}

template<typename type>
__host__ __device__ dualQuaternion<type>::dualQuaternion(const quaternion<type>& p, const quaternion<type>& q) :
    p(p), q(q)
{}

template<typename type>
__host__ __device__ dualQuaternion<type>::~dualQuaternion()
{}

template<typename type>
__host__ __device__ dualQuaternion<type>::dualQuaternion(const dualQuaternion<type>& other) :
    p(other.p), q(other.q)
{}

template<typename type>
__host__ __device__ dualQuaternion<type>& dualQuaternion<type>::operator=(const dualQuaternion<type>& other)
{
    p = other.p;
    q = other.q;
    return *this;
}

template<typename type>
__host__ __device__ quaternion<type>          dualQuaternion<type>::rotation()const
{
    return p;
}

template<typename type>
__host__ __device__ quaternion<type>          dualQuaternion<type>::translation()const
{
    quaternion<type> copy(p);
    copy.conjugate();
    return type(2) * q * copy;
}

template<typename type>
__host__ __device__ bool                      dualQuaternion<type>::operator==(const dualQuaternion<type>& other)const
{
    return p == other.p && q == other.q;
}

template<typename type>
__host__ __device__ bool                      dualQuaternion<type>::operator!=(const dualQuaternion<type>& other)const
{
    return !(p == other.p && q == other.q);
}

template<typename type>
__host__ __device__ dualQuaternion<type>      dualQuaternion<type>::operator+ (const dualQuaternion<type>& other)const
{
    return dualQuaternion<type>(p + other.p, q + other.q);
}

template<typename type>
__host__ __device__ dualQuaternion<type>      dualQuaternion<type>::operator- (const dualQuaternion<type>& other)const
{
    return dualQuaternion<type>(p - other.p, q - other.q);
}

template<typename type>
__host__ __device__ dualQuaternion<type>      dualQuaternion<type>::operator* (const dualQuaternion<type>& other)const
{
    return dualQuaternion<type>(
        p * other.p,
        p * other.q + q * other.p
        );
}

template<typename type>
__host__ __device__ dualQuaternion<type>& dualQuaternion<type>::operator+=(const dualQuaternion<type>& other)
{
    p += other.p;
    q += other.q;
    return *this;
}

template<typename type>
__host__ __device__ dualQuaternion<type>& dualQuaternion<type>::operator-=(const dualQuaternion<type>& other)
{
    p -= other.p;
    q -= other.q;
    return *this;
}

template<typename type>
__host__ __device__ dualQuaternion<type>& dualQuaternion<type>::operator*=(const dualQuaternion<type>& other)
{
    dualQuaternion<type> copy(*this);
    *this = copy * other;

    return *this;
}

template<typename T>
std::ostream& operator<< (std::ostream& out, const dualQuaternion<T>& quat)
{
    out << quat.p << "\t\t" << quat.q;
    return out;
}

template<typename T>
__host__ __device__ dualQuaternion<T> operator* (const T& c, const dualQuaternion<T>& quat)
{
    return dualQuaternion<T>(c * quat.p, c * quat.q);
}

template<typename type>
__host__ __device__ dualQuaternion<type>& dualQuaternion<type>::normalize()
{
    if (p.scalar() * q.scalar() + p.vector().x * q.vector().x + p.vector().y * q.vector().y + p.vector().z * q.vector().z == 0) {
        type norma = p.scalar() * p.scalar() + p.vector().x * p.vector().x + p.vector().y * p.vector().y + p.vector().z * p.vector().z;
        norma = std::sqrt(norma);
        p = (type(1) / norma) * p;
        q = (type(1) / norma) * q;
    }
    return *this;
}

template<typename type>
__host__ __device__ dualQuaternion<type>& dualQuaternion<type>::conjugate()
{
    p.conjugate();
    type(-1)* q.conjugate();
    return *this;
}

template<typename type>
__host__ __device__ dualQuaternion<type>& dualQuaternion<type>::invert()
{
    p.invert();
    q = type(-1) * p * q * p;

    return *this;
}

template<typename T>
__host__ __device__ dualQuaternion<T>   normalize(const dualQuaternion<T>& quat)
{
    const dualQuaternion<T>& res(quat);
    if (quat.p.s * quat.q.s + quat.p.x * quat.q.x + quat.p.y * quat.q.y + quat.p.z * quat.q.z == 0) {
        T norma = quat.p.s * quat.p.s + quat.p.x * quat.p.x + quat.p.y * quat.p.y + quat.p.z * quat.p.z;
        norma = std::sqrt(norma);

        res.p.s = quat.p.s / norma;
        res.p.x = quat.p.x / norma;
        res.p.y = quat.p.y / norma;
        res.p.z = quat.p.z / norma;
        res.q.s = quat.q.s / norma;
        res.q.x = quat.q.x / norma;
        res.q.y = quat.q.y / norma;
        res.q.z = quat.q.z / norma;

    }
    return res;
}

template<typename T>
__host__ __device__ dualQuaternion<T>   conjugate(const dualQuaternion<T>& quat)
{
    return dualQuaternion<T>(conjugate(quat.p), T(-1) * conjugate(quat.q));
}

template<typename T>
__host__ __device__ dualQuaternion<T>   invert(const dualQuaternion<T>& quat)
{
    dualQuaternion<T> res(quat);
    res.p.invert();
    res.q = T(-1) * res.p * res.q * res.p;

    return res;
}

template<typename T>
__host__ __device__ dualQuaternion<T>   convert(const quaternion<T>& rotation, const quaternion<T>& translation)
{
    return dualQuaternion<T>(rotation, T(0.5) * translation * rotation);
}

// template<typename T>
// glm::mat<4, 4, T, glm::defaultp> convert(const dualQuaternion<T>& quat)
// {
//     quaternion<T> rotatrion = quat.rotation();
//     quaternion<T> translation = quat.translation();
//
//     glm::mat<3, 3, T, glm::defaultp> R = convert(rotatrion);
//
//     glm::mat<4, 4, T, glm::defaultp> SE3;
//
//     SE3[0][0] = R[0][0];    SE3[0][1] = R[0][1];    SE3[0][2] = R[0][2];    SE3[0][3] = translation.vector().x;
//     SE3[1][0] = R[1][0];    SE3[1][1] = R[1][1];    SE3[1][2] = R[1][2];    SE3[1][3] = translation.vector().y;
//     SE3[2][0] = R[2][0];    SE3[2][1] = R[2][1];    SE3[2][2] = R[2][2];    SE3[2][3] = translation.vector().z;
//     SE3[3][0] = T(0);       SE3[3][1] = T(0);       SE3[3][2] = T(0);       SE3[3][3] = T(1);
//
//     return SE3;
// }
//
// template<typename T>
// dualQuaternion<T> convert(const glm::mat<4, 4, T, glm::defaultp>& SE3)
// {
//     glm::mat<3, 3, T, glm::defaultp> R;
//
//     R[0][0] = SE3[0][0];    R[0][1] = SE3[0][1];    R[0][2] = SE3[0][2];
//     R[1][0] = SE3[1][0];    R[1][1] = SE3[1][1];    R[1][2] = SE3[1][2];
//     R[2][0] = SE3[2][0];    R[2][1] = SE3[2][1];    R[2][2] = SE3[2][2];
//
//     quaternion<T> rotatrion = convert(R);
//     quaternion<T> translation = quaternion<T>(T(0), SE3[0][3], SE3[1][3], SE3[2][3]);
//
//     return convert(rotatrion, translation);
// }


template<typename T>
__host__ __device__ dualQuaternion<T> slerp(const dualQuaternion<T>& quat1, const dualQuaternion<T>& quat2, const T& t)
{
    quaternion<T> r1 = quat1.rotation();
    quaternion<T> r2 = quat2.rotation();
    quaternion<T> t1 = quat1.translation();
    quaternion<T> t2 = quat2.translation();

    dualQuaternion<T> dQuat(conjugate(r1) * r2, T(0.5) * conjugate(r1) * (t2 - t1) * r2);
    T theta = T(2) * std::acos(dQuat.p.scalar());
    vec4f l = normal(dQuat.p.vector());

    vec4f tr = dQuat.translation().vector();
    T d = tr.x() * l.x() + tr.y() * l.y() + tr.z() * l.z();
    vec4f m = T(0.5) * (cross(tr, l) + (tr - d * l) / std::tan(T(0.5) * theta));

    theta *= t;
    d *= t;

    dualQuaternion<T> sigma(quaternion<T>(std::cos(T(0.5) * theta), std::sin(T(0.5) * theta) * l),
        quaternion<T>(-T(0.5) * d * std::sin(T(0.5) * theta), T(0.5) * d * std::cos(T(0.5) * theta) * l + std::sin(T(0.5) * theta) * m));

    return quat1 * sigma;
}

}
#endif // DUALQUATERNION_H
