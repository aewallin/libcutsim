/*  
 *  Copyright 2012 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  Copyright 2015      Kazuyasu Hamada (k-hamada@gifu-u.ac.jp)
 *  
 *  This file is part of libcutsim.
 *
 *  libcutsim is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libcutsim is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libcutsim.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cassert>
#include <cmath>

namespace cutsim
{

    /// color of a GL-vertex
    struct Color
    {
        float r; ///< red
        float g; ///< green
        float b; ///< blue
        /// set color
        void set(float ri, float gi, float bi)
        {
            r = ri;
            g = gi;
            b = bi;
        }
    };

    /// a vertex/point in 3D, with (x,y,z) coordinates of type float
    /// normal is (nx,ny,nz)
    /// color is (r,g,b)
    struct GLVertex
    {
        /// default (0,0,0) ctor
        GLVertex() : x(0), y(0), z(0), r(0), g(0), b(0) {}
        /// ctor with given (x,y,z)
        GLVertex(float x, float y, float z)
            : x(x), y(y), z(z), r(0), g(0), b(0) {}
        /// ctor with given (x,y,z) and (r,g,b)
        GLVertex(float x, float y, float z, float r, float g, float b)
            : x(x), y(y), z(z), r(r), g(g), b(b) {}
        /// ctor with given position (x,y,z) color (r,g,b) and normal (xn,yn,zn)
        GLVertex(float x, float y, float z, float red, float gre, float blu, float xn, float yn, float zn)
            : x(x), y(y), z(z), r(red), g(gre), b(blu), nx(xn), ny(yn), nz(zn) {}

        /// set normal
        void setNormal(float xn, float yn, float zn)
        {
            nx = xn;
            ny = yn;
            nz = zn;
            // normalize:
            float norm = sqrt(nx * nx + ny * ny + nz * nz);
            if (norm != 1.0)
            {
                nx /= norm;
                ny /= norm;
                nz /= norm;
            }
        }
        /// set the vertex color
        void setColor(Color c)
        {
            setColor(c.r, c.g, c.b);
        }
        /// set the vertex color
        void setColor(float red, float green, float blue)
        {
            r = red;
            g = green;
            b = blue;
        }

        /// assume p1-p2-p3 forms a triangle. set normals. set color.
        static void set_normal_and_color(GLVertex &p1, GLVertex &p2, GLVertex &p3, Color c)
        {
            GLVertex n = (p1 - p2).cross(p1 - p3);
            n.normalize();
            p1.setNormal(n.x, n.y, n.z);
            p2.setNormal(n.x, n.y, n.z);
            p3.setNormal(n.x, n.y, n.z);
            p1.setColor(c);
            p2.setColor(c);
            p3.setColor(c);
        }
        /// string output
        std::string str() { return "glvertex"; } //(%1, %2, %3 )").arg(x).arg(y).arg(z); }

        // DATA
        float x;  ///< x-coordinate
        float y;  ///< y-coordinate
        float z;  ///< z-coordinate
                  // color, 3*4=12-bytes offset from position data.
        float r;  ///< red
        float g;  ///< green
        float b;  ///< blue
                  // normal, 24-bytes offset
        float nx; ///< normal x-coordinate
        float ny; ///< normal y-coordinate
        float nz; ///< normal z-coordinate

        // Operators
        /// return length
        float norm() const
        {
            return sqrt(x * x + y * y + z * z);
        }
        /// set length to 1
        void normalize()
        {
            if (this->norm() != 0.0)
                *this *= (1 / this->norm());
        }
        /// multiplication by scalar
        GLVertex &operator*=(const double &a)
        {
            x *= a;
            y *= a;
            z *= a;
            return *this;
        }
        /// multiplication by scalar
        GLVertex operator*(const double &a) const
        {
            return GLVertex(*this) *= a;
        }
        /// vector addition
        GLVertex &operator+=(const GLVertex &p)
        {
            x += p.x;
            y += p.y;
            z += p.z;
            return *this;
        }
        /// vector addition
        const GLVertex operator+(const GLVertex &p) const
        {
            return GLVertex(*this) += p;
        }
        /// vector subtraction
        GLVertex &operator-=(const GLVertex &p)
        {
            x -= p.x;
            y -= p.y;
            z -= p.z;
            return *this;
        }
        /// vector subtraction
        const GLVertex operator-(const GLVertex &p) const
        {
            return GLVertex(*this) -= p;
        }
        /// cross product
        GLVertex cross(const GLVertex &p) const
        {
            float xc = y * p.z - z * p.y;
            float yc = z * p.x - x * p.z;
            float zc = x * p.y - y * p.x;
            return GLVertex(xc, yc, zc);
        }

        /// dot product
        float dot(const GLVertex &p) const
        {
            return x * p.x + y * p.y + z * p.z;
        }

        /// rotate vertex by amount alfa around o->v axis
        void rotate(const GLVertex &origin, const GLVertex &v, float alfa)
        {
            // rotate point p by alfa deg/rad around vector o->v
            // p = o + M*(p-o)
            float M[3][3];
            float c = cos(alfa);
            float D = 1.0 - c;
            float s = sin(alfa);
            M[0][0] = v.x * v.x * D + c;
            M[0][1] = v.y * v.x * D + v.z * s;
            M[0][2] = v.z * v.x * D - v.y * s;
            M[1][0] = v.x * v.y * D - v.z * s;
            M[1][1] = v.y * v.y * D + c;
            M[1][2] = v.z * v.y * D + v.x * s;
            M[2][0] = v.x * v.z * D + v.y * s;
            M[2][1] = v.y * v.z * D - v.x * s;
            M[2][2] = v.z * v.z * D + c;
            // matrix multiply
            float vector[3];
            vector[0] = x - origin.x;
            vector[1] = y - origin.y;
            vector[2] = z - origin.z;
            float result[3];
            for (int i = 0; i < 3; i++)
            {
                result[i] = 0;
                for (int j = 0; j < 3; j++)
                {
                    result[i] += vector[j] * M[i][j];
                }
            }
            x = origin.x + result[0];
            y = origin.y + result[1];
            z = origin.z + result[2];
        }
        /// rotate vertex around A and C axis
        GLVertex rotateAC(const float a, const float c)
        {
            float M[3][3];
            float zC = cos(c);
            float zS = sin(c);
            float xC = cos(a);
            float xS = sin(a);
            M[0][0] = zC;
            M[0][1] = -zS;
            M[0][2] = 0.0;
            M[1][0] = zS * xC;
            M[1][1] = zC * xC;
            M[1][2] = -xS;
            M[2][0] = zS * xS;
            M[2][1] = zC * xS;
            M[2][2] = xC;
            // matrix multiply
            GLVertex result;
            result.x = x * M[0][0] + y * M[0][1];
            result.y = x * M[1][0] + y * M[1][1] + z * M[1][2];
            result.z = x * M[2][0] + y * M[2][1] + z * M[2][2];
            return result;
        }
        /// rotate vertex by A and C rotation matrix
        GLVertex rotateAC(const float M[3][3])
        {
            // matrix multiply
            GLVertex result;
            result.x = x * M[0][0] + y * M[0][1];
            result.y = x * M[1][0] + y * M[1][1] + z * M[1][2];
            result.z = x * M[2][0] + y * M[2][1] + z * M[2][2];
            return result;
        }
    };

} // end namespace
