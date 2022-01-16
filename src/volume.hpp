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

#include <iostream>
#include <list>
#include <cassert>

#include "bbox.hpp"
#include "facet.hpp"
#include "glvertex.hpp"
#include "gldata.hpp"
#include "fileio.hpp"

namespace cutsim
{

    /// Volume is a base-class for defining implicit volumes from which to build octrees
    /// an implicit volume is defined as a function dist(Point p)
    /// which returns a positive value inside the volume and a negative value outside.
    ///
    /// the "positive inside, negative outside" sign-convetion means that
    /// boolean operations between two Volumes A and B can be done with:
    ///
    ///  A U B ('union' or 'sum') =  max( d(A),  d(B) )
    ///  A \ B ('diff' or 'minus') = min( d(A), -d(B) )
    ///  A int B ('intersection') =  min( d(A),  d(B) )
    ///
    /// reference: Frisken et al. "Designing with Distance Fields"
    /// http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.69.9025
    ///
    /// iso-surface extraction using standard marching-cubes requires just the distance
    /// field to be stored at each corner vertex of an octree leaf-node.
    ///
    /// advanced iso-surface extraction using extended-marching-cubes or dual-contouring may require
    /// more information such as normals of the distance field or exact
    /// intersection points and normals. This is similar to a tri-dexel representation.
    /// In multi-material simulation a material-index can be stored.
    /// Each cutter may also cut the material with a color of its own
    /// (new vertices have the color of the cutter).
    class Volume
    {
    public:
        Volume() {}
        /// return signed distance from volume surface to Point p
        /// Points p inside the volume should return positive values.
        /// Points p outside the volume should return negative values.
        virtual float dist(const GLVertex &p) const { return 0; }
        /// set the color
        void setColor(float r, float g, float b)
        {
            color.r = r;
            color.g = g;
            color.b = b;
        }
        /// set the centerpoint of the sphere
        void setCenter(float x, float y, float z)
        {
            center = GLVertex(x, y, z);
            calcBB();
        }
        virtual void calcBB() {} ///< update bounding-box
                                 // DATA
        /// bounding-box. This holds the maximum and minimum points along the (x,y,z) coordinates
        /// of the volume
        /// dist(p) can return positive "inside" values only inside the bounding-box
        Bbox bb;
        /// the color of this Volume
        Color color;
        GLVertex center;
    };

    // sub-classes of Volume below:

    /// sphere centered at center
    class SphereVolume : public Volume
    {
    public:
        /// default constructor
        SphereVolume();
        /// set radius of sphere
        void setRadius(float r)
        {
            radius = r;
            calcBB();
        }
        /// update the Bbox
        virtual void calcBB();
        virtual float dist(const GLVertex &p) const;
        float radius; ///< radius of sphere
    };

    /// cube at center with side-length side
    class CubeVolume : public Volume
    {
    public:
        CubeVolume();
        virtual float dist(const GLVertex &p) const;
        void setSide(float s)
        {
            side = s;
            calcBB();
        }
        void calcBB();
        // DATA
        float side; ///< side length of cube
    };

    /// cone, for v-carving sim
    class ConeVolume : public Volume
    {
    public:
        ConeVolume()
        {
            alfa = M_PI / 4;
            setHeight(10);
        }
        virtual float dist(const GLVertex &p) const;
        void setHeight(float h)
        {
            height = h;
            calcBB();
        }
        void calcBB();
        // DATA
        float height; ///< height of cone
        float alfa;   ///< half-angle of cone
    };

    /// STL volume
    class MeshVolume : public Volume
    {

    public:
        MeshVolume();
        virtual ~MeshVolume()
        {
            V21.resize(0);
            invV21dotV21.resize(0);
            V32.resize(0);
            invV32dotV32.resize(0);
            V13.resize(0);
            invV13dotV13.resize(0);
            facets.resize(0);
        }

        void addFacet(Facet *f) { facets.push_back(f); }

        /// set the center of STL
        void setCenter(GLVertex v)
        {
            center = v;
        }

        /// set the target center of STL
        void setMeshCenter(float x, float y, float z)
        {
            meshCenter = GLVertex(x, y, z);
            calcBB();
        }

        /// set the rotation center of STL
        void setRotationCenter(GLVertex c)
        {
            rotationCenter = c;
        }

        /// set the angle of STL
        void setAngle(GLVertex a)
        {
            angle = a;
        }

        /// update the bounding-box of STL
        void calcBB();

        virtual float dist(const GLVertex &p) const;

        // load mesh from facet data
        bool loadMesh(boost::python::list);

        // load mesh from stl file
        bool loadStl(boost::python::str);

    private:
        // V21[i] = facets[i]->v2 - facets[i]->v1
        std::vector<GLVertex> V21;
        // 1/<V21[i], V21[i]>
        std::vector<double> invV21dotV21;
        // V32[i] = facets[i]->v3 - facets[i]->v2
        std::vector<GLVertex> V32;
        // 1/<V32[i], V32[i]>
        std::vector<double> invV32dotV32;
        // V13[i] = facets[i]->v1 - facets[i]->v3
        std::vector<GLVertex> V13;
        // 1/<V13[i], V13[i]>
        std::vector<double> invV13dotV13;
        /// STL center
        GLVertex center;
        /// STL Target Center
        GLVertex meshCenter;
        /// center of rotation
        GLVertex rotationCenter;
        /// STL angle
        GLVertex angle;
        // facets
        std::vector<Facet *> facets;
    };

} // end namespace
// end file volume.hpp
