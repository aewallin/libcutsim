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

namespace cutsim {
    
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
class Volume {
public:
    Volume(){}
    /// return signed distance from volume surface to Point p
    /// Points p inside the volume should return positive values.
    /// Points p outside the volume should return negative values.
    virtual float dist(const GLVertex& p) const {return 0;}
    /// set the color
    void setColor(float r, float g, float b) {
        color.r=r; color.g=g; color.b=b;
    }
    /// set the centerpoint of the sphere
    void setCenter(float x, float y, float z) {
        center = GLVertex(x,y,z);
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
class SphereVolume: public Volume {
    public:
        /// default constructor
        SphereVolume();
        /// set radius of sphere
        void setRadius(float r) {
            radius=r;
            calcBB();
        }
        /// update the Bbox
        virtual void calcBB();
        virtual float dist(const GLVertex& p) const;
        float radius;   ///< radius of sphere
};

/// cube at center with side-length side
class CubeVolume: public Volume {
    public:
        CubeVolume();
        virtual float dist(const GLVertex& p) const;
        void setSide(float s) {
            side=s;
            calcBB();
        }
        void calcBB();
// DATA
        float side;    ///< side length of cube
};

/// cone, for v-carving sim
class ConeVolume: public Volume {
    public:
        ConeVolume() {
            alfa = M_PI / 4;
            setHeight(10);
        }
        virtual float dist(const GLVertex& p) const;
        void setHeight(float h) {
            height=h;
            calcBB();
        }
        void calcBB();
// DATA
        float height;  ///< height of cone
        float alfa;    ///< half-angle of cone
};

/// STL volume
class MeshVolume: public Volume {

    public:
        MeshVolume();
        virtual ~MeshVolume() {
        	V21.resize(0); invV21dotV21.resize(0);
        	V32.resize(0); invV32dotV32.resize(0);
        	V13.resize(0); invV13dotV13.resize(0);
            facets.resize(0);
        }


        void addFacet(Facet *f)	{ facets.push_back(f); }

        /// set the center of STL
        void setCenter(GLVertex v) {
            center = v;
        }

        /// set the target center of STL
        void setMeshCenter(float x, float y, float z){
        // std::cout << "MeshVolume setMeshCenter" << std::endl;
        // setCenter(GLVertex(x, y, z));
        meshCenter = GLVertex(x, y, z);
        calcBB();

        }
        /// set the rotation center of STL
        void setRotationCenter(GLVertex c) {
            rotationCenter = c;
        }
        /// set the angle of STL
        void setAngle(GLVertex a) {
            angle = a;
        }
        /// update the bounding-box of STL
        void calcBB();

        virtual float dist(const GLVertex& p) const;

        // load mesh from facet data
        void loadMesh(boost::python::list);

       // int readStlFile(QString file) {  int retval = Stl::readStlFile(file); calcBB(); return retval; }

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
        std::vector<Facet*> facets;
};


/// cylinder volume
/*
class CylinderOCTVolume: public OCTVolume {
    public:
        /// default constructor
        CylinderOCTVolume();
        /// startpoint of cylinder
        GLVertex p1;
        /// endpoint of cylinder
        GLVertex p2;
        /// cylinder radius
        double radius;
        bool isInside(GLVertex& p) const;
        /// update the bounding box
        void calcBB();
        double dist(GLVertex& p) const {return -1;}
};*/

/// box-volume
/// from corner, go out in the three directions v1,v2,v3
/// interior points = corner + a*v1 + b*v2 + c*v3  
/// where a, b, c are in [0,1]
/*
class BoxOCTVolume: public OCTVolume {
    public:
        /// default constructor
        BoxOCTVolume();
        /// one corner of the box
        GLVertex corner;
        /// first vector from corner, to span the box
        GLVertex v1;
        /// second vector
        GLVertex v2;
        /// third vector
        GLVertex v3;
        bool isInside(GLVertex& p) const;
        /// update the bounding-box
        void calcBB();
        double dist(GLVertex& p) const {
            // translate to origo
            //GLVertex pt = p - corner;
            double max_x = corner.x + v1.x;
            double min_x = corner.x;
            double max_y = corner.y + v2.y;
            double min_y = corner.y;
            double max_z = corner.z + v3.z;
            double min_z = corner.z;
            
            double xdist;
            if (p.x < min_x) {
                xdist = min_x - p.x;
            } else if ( (min_x <= p.x) && (p.x <= max_x) ) {
                if ( (p.x-min_x) > (max_x-p.x) )
                    xdist = p.x-min_x;
                else
                    xdist = max_x-p.x;
            } else if ( p.x > max_x ) {
                xdist = p.x-max_x;
            } else {
                assert(0);
            }
            
            double ydist;
            if (p.y < min_y) {
                ydist = min_y - p.y;
            } else if ( (min_y <= p.y) && (p.y <= max_y) ) {
                if ( (p.y-min_y) > (max_y-p.y) )
                    ydist = p.y-min_y;
                else
                    ydist = max_y-p.y;
            } else if ( p.y > max_y ) {
                ydist = p.y-max_y;
            } else {
                assert(0);
            }
            
            double zdist;
            if (p.z < min_z) {
                zdist = min_z - p.z;
            } else if ( (min_z <= p.z) && (p.z <= max_z) ) {
                if ( (p.z-min_z) > (max_z-p.z) )
                    zdist = p.z-min_z;
                else
                    zdist = max_z-p.z;
            } else if ( p.z > max_z ) {
                zdist = p.z-max_z;
            } else {
                assert(0);
            }
            
            if ( xdist <= ydist && xdist <= zdist )
                return xdist;
            else if ( ydist < xdist && ydist < zdist )
                return ydist;
            else if ( zdist < xdist && zdist < xdist )
                return zdist;
            else {
                assert(0);
                return -1;
            }
            // box is = a*v1 + b*v2 + c*v3
            // where a,b,c are in [0,1]
            
            // v1 radial
            // v2 along move
            // v3 axial(z-dir)
            
            //GLVertex v1xy = v1;
            //v1xy.z = 0;
            
            //GLVertex v2xy = v2;
            //v2xy.z = 0;
            
            // projection along each vector, in turn
            // this only works if the vectors are orthogonal
            //double t1 = pt.dot(v1xy)/v1xy.dot(v1xy);
            //if ( (t1 < 0.0) || (t1>1.0) )
            //    return false;
            
            //return dist;
        }
};*/

/// elliptic tube volume
/*
class EtubeOCTVolume: public OCTVolume {
    public:
        /// default constructor
        EtubeOCTVolume();
        /// construct Etube with given parameters
        EtubeOCTVolume(GLVertex& p1in, GLVertex& p2in, GLVertex& ain, GLVertex& bin);
        /// start of tune
        GLVertex p1; // start of move
        /// end of tube
        GLVertex p2; // end of move
        /// ellipse a-axis
        GLVertex a; // a-axis of ellipse
        /// ellipse b-axis
        GLVertex b; // b-axis of ellipse
        bool isInside(GLVertex& p) const;
        double dist(GLVertex& p) const {return -1;}
};*/

/// cylindrical cutter volume
/*
class CylCutterVolume: public OCTVolume {
    public:
        CylCutterVolume();
        /// cutter radius
        double radius;
        /// cutter length
        double length;
        /// start CL-Point for this move
        void setPos(Point& p);
        /// cutter position
        Point pos;
        bool isInside(Point& p) const;
        /// update the Bbox
        void calcBB();
        double dist(Point& p) const;
};
*/


/// ball-nose cutter volume
/*
class BallCutterVolume : public Volume {
    public:
        BallCutterVolume();
        virtual float dist(const GLVertex& p) const;
        
        /// cutter radius
        double radius;
        /// cutter length
        double length;
        /// position cutter at p
        void setPos(Point& p);
        /// cutter position
        Point pos;
        bool isInside(Point& p) const {return false;};
        /// update bounding box
        void calcBB();
        double dist(Point& p) const;
};
*/

/// bull-nose cutter volume
/*
class BullCutterVolume: public OCTVolume {
    public:
        BullCutterVolume();
        /// cutter radius
        double radius;
        /// radius of cylinder-part
        double r1;
        /// radius of torus
        double r2;
        /// cutter length
        double length;
        /// position cutter
        void setPos(Point& p);
        /// position of cutter
        Point pos;
        bool isInside(Point& p) const {return false;};
        /// update bounding box
        void calcBB();
        double dist(Point& p) const;
};
*/

/*
/// plane-volume, useful for cutting stock to shape
class PlaneVolume: public OCTVolume {
    public:
        PlaneVolume() {};
        /// create a plane orthogonal to axis at position pos 
        PlaneVolume(bool sign, unsigned int axis, double pos);
        /// is +pos or -pos the positive side?
        bool sign;
        /// tje position of the plane
        double position;
        /// the axis of the plane
        unsigned int axis;
        bool isInside(GLVertex& p) const {return false;};
        /// update bounding box
        void calcBB();
        double dist(GLVertex& p) const;
};*/


/// cutter-swept volume of a CylCutter
/*
class CylMoveOCTVolume: public OCTVolume {
    public:
        /// default constructor
        CylMoveOCTVolume() {};
        /// create a CylMoveOCTVolume with the specified cutter and startpoint p1 and endpoint p2
        CylMoveOCTVolume(const CylCutter& c, const Point& p1, const Point& p2);
        /// start CL-Point for this move
        Point p1;
        /// end CL-point for this move
        Point p2;
        /// the CylCutter for this move
        CylCutter c;
        /// a cylindrical volume representing the cutter at p1
        CylinderOCTVolume c1;
        /// a cylindrical volume representing the cutter at p2
        CylinderOCTVolume c2;
        /// an elliptic tube of the swept-volube
        EtubeOCTVolume etube;
        /// the box-part of the swept-volume
        BoxOCTVolume box;
        bool isInside(Point& p) const;
        double dist(Point& p) const {return -1;}
};*/




} // end namespace

// end file volume.hpp
