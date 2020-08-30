
// This code is **based on** from https://www.scratchapixel.com/code.php?id=10&origin=/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes&src=1
// ... which is made available under GNU license

//[header]
// This program illustrates how the concept of vector and matrix can be implemented
// in C++. This is a light version of the implementation. It contains the most
// essential methods to manipulate vectors and matrices. It should be enough
// for most projects. Vectors and matrices are really the alphabet as we said
// in the lesson of any graphics application. It's really important you feel
// confortable with these techniques especially with the concepts of
// normalizing vectors, computing their length, computing the dot and cross products
// of two vectors, and the point- and vector-matrix multiplication (and knowing
// the difference between the two).
//[/header]
//[compile]
// c++ geometry.cpp  -o geometry -std=c++11
//[/compile]
//[ignore]
// Copyright (C) 2012  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//[/ignore]

#ifndef LARCV_GEOMETRY_H
#define LARCV_GEOMETRY_H
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "TVector3.h"
#include "TLorentzVector.h"

namespace larcv {

//[comment]
// Implementation of a generic vector class - it will be used to deal with 3D points, vectors and normals.
// The class is implemented as a template. While it may complicate the code a bit, it gives us
// the flexibility later, to specialize the type of the coordinates into anything we want.
// For example: Vec3f if we want the coordinates to be floats or Vec3i if we want the coordinates to be integers.
//
// Vec3 is a standard/common way of naming vectors, points, etc. The OpenEXR and Autodesk libraries
// use this convention for instance.
//[/comment]
template<typename T>
    class Vec3
    {
    public:
        Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
        Vec3(T xx) : x(xx), y(xx), z(xx) {}
        Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
        Vec3(const TVector3& v) : x(v.X()), y(v.Y()), z(v.Z()) {}
        Vec3(const TLorentzVector& v) : x(v.X()), y(v.Y()), z(v.Z()) {}
        Vec3 operator + (const Vec3 &v) const
        { return Vec3(x + v.x, y + v.y, z + v.z); }
        Vec3 operator - (const Vec3 &v) const
        { return Vec3(x - v.x, y - v.y, z - v.z); }
        Vec3 operator - () const
        { return Vec3(-x, -y, -z); }
        Vec3 operator * (const T &r) const
        { return Vec3(x * r, y * r, z * r); }
        Vec3 operator * (const Vec3 &v) const
        { return Vec3(x * v.x, y * v.y, z * v.z); }
        T dotProduct(const Vec3<T> &v) const
        { return x * v.x + y * v.y + z * v.z; }
        Vec3& operator /= (const T &r)
        { x /= r, y /= r, z /= r; return *this; }
        Vec3& operator *= (const T &r)
        { x *= r, y *= r, z *= r; return *this; }
        Vec3 crossProduct(const Vec3<T> &v) const
        { return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
        T norm() const
        { return x * x + y * y + z * z; }
        T length() const
        { return sqrt(norm()); }
    //[comment]
    // The next two operators are sometimes called access operators or
    // accessors. The Vec coordinates can be accessed that way v[0], v[1], v[2],
    // rather than using the more traditional form v.x, v.y, v.z. This useful
    // when vectors are used in loops: the coordinates can be accessed with the
    // loop index (e.g. v[i]).
    //[/comment]
        const T& operator [] (uint8_t i) const { return (&x)[i]; }
        T& operator [] (uint8_t i) { return (&x)[i]; }
        Vec3& normalize()
        {
            T n = norm();
            if (n > 0) {
                T factor = 1 / sqrt(n);
                x *= factor, y *= factor, z *= factor;
            }

            return *this;
        }

        friend Vec3 operator * (const T &r, const Vec3 &v)
        { return Vec3<T>(v.x * r, v.y * r, v.z * r); }
        friend Vec3 operator / (const T &r, const Vec3 &v)
        { return Vec3<T>(r / v.x, r / v.y, r / v.z); }

        friend std::ostream& operator << (std::ostream &s, const Vec3<T> &v)
        {
            return s << '[' << v.x << ' ' << v.y << ' ' << v.z << ']';
        }

        T x, y, z;
    };

//[comment]
// Now you can specialize the class. We are just showing two examples here. In your code
// you can declare a vector either that way: Vec3<float> a, or that way: Vec3f a
//[/comment]
    typedef Vec3<float> Vec3f;
    typedef Vec3<int> Vec3i;

}

#endif