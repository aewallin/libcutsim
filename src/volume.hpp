/*  
 *  Copyright 2012 Anders Wallin (anders.e.e.wallin "at" gmail.com)
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
#include "glvertex.hpp"
#include "gldata.hpp"

namespace cutsim {
    
/// base-class for defining implicit volumes from which to build octrees
/// an implicit volume is defined as a function dist(Point p)
/// which returns a positive value inside the volume and a negative value outside.
/// 
/// the "positive inside, negative outside" sign-convetion means that boolean operations can be done with:
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
/// Each cutter may also cut the material with a color of its own (new vertices have the color of the cutter).
class Volume {
public:
    /// default constructor
    Volume(){}
    /// return signed distance from volume surface to Point p
    /// Points p inside the volume should return positive values.
    /// Points p outside the volume should return negative values.
    virtual float dist(const GLVertex& p) const {return 0;}
    /// set the color
    void setColor(float r, float g, float b) {
        color.r=r; color.g=g; color.b=b;
    }
// DATA
    /// bounding-box. This holds the maximum(minimum) points along the X,Y, and Z-coordinates
    /// of the volume (i.e. the volume where dist(p) returns negative values)
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
        void setRadius(double r) {
            radius=r;
            calcBB();
        }
        /// set the centerpoint of the sphere
        void setCenter(float x, float y, float z) {
            center = GLVertex(x,y,z);
            calcBB();
        }
        /// update the Bbox
        void calcBB();
        virtual float dist(const GLVertex& p) const;

        /// center Point of sphere
        //GLVertex center;
        /// radius of sphere
        double radius;
};

/// box-volume
/// from corner, go out in the three directions v1,v2,v3
/// interior points = corner + a*v1 + b*v2 + c*v3  
/// where a, b, c are in [0,1]
/*
class RectVolume : public Volume {
    public:
        /// default constructor
        RectVolume();
        void set_corner(float x, float y, float z) {
            corner.x = x;
            corner.y = y;
            corner.z = z;
        }
        void set_dimensions(float l, float w, float h) {
            v1 = GLVertex(l,0,0); 
            v2 = GLVertex(0,w,0);
            v3 = GLVertex(0,0,h);
        }
        /// one corner of the box
        GLVertex corner;
        /// first vector from corner, to span the box
        GLVertex v1;
        /// second vector
        GLVertex v2;
        /// third vector
        GLVertex v3;
        /// update the bounding-box
        void calcBB();
        double dist(const GLVertex& p) const;
};*/


/// cube at center with side-length side
class CubeVolume: public Volume {
    public:
        /// default constructor
        CubeVolume();
        virtual float dist(const GLVertex& p) const;
        void setSide(double s) {
            side=s;
            calcBB();
        }
        /// set the centerpoint of the sphere
        void setCenter(float x, float y, float z) {
            center = GLVertex(x,y,z);
            calcBB();
        }
        /// center point of cube
        //GLVertex center;
        /// side length of cube
        double side;
       // bool isInside(GLVertex& p) const;
        /// update bounding-box
        void calcBB();
        double dist(GLVertex& p) const;
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

/*
/// ball-nose cutter volume
class BallCutterVolume: public OCTVolume {
    public:
        BallCutterVolume();
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