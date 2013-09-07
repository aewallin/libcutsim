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

#include <cassert>
#include <cmath>

#include "volume.hpp"

namespace cutsim {

//************* Sphere **************/

/// sphere at center
SphereVolume::SphereVolume() {
    center = GLVertex(0,0,0);
    radius = 1.0;
    calcBB();
}

float SphereVolume::dist(const GLVertex& p ) const {
    float d = (center-p).norm();
    return radius-d; // positive inside. negative outside.
}

/// set the bounding box values
void SphereVolume::calcBB() {
    bb.clear();
    GLVertex maxpt = GLVertex(center.x + radius, center.y + radius, center.z + radius);
    GLVertex minpt = GLVertex(center.x - radius, center.y - radius, center.z - radius);
    bb.addPoint( maxpt );
    bb.addPoint( minpt );
}

//************* Cube **************/


/// cube at center with side length side
CubeVolume::CubeVolume() {
    center = GLVertex(0,0,0);
    side = 1.234;
}

/// set bbox values
void CubeVolume::calcBB() {
    bb.clear();
    GLVertex maxpt = GLVertex(center.x + side/2, center.y + side/2, center.z + side/2);
    GLVertex minpt = GLVertex(center.x - side/2, center.y - side/2, center.z - side/2);
    bb.addPoint( maxpt );
    bb.addPoint( minpt );
}

float CubeVolume::dist(const GLVertex& p ) const {
    float m;
    m = p.x-center.x;
    
    if ( fabs(m) < fabs(p.y-center.y) ) 
        m = p.y-center.y;
    if (fabs(m) < fabs(p.z-center.z) )
        m = p.z-center.z;
    // m is now the maximum coordinate
    //bool sign = (invert ? -1.0 : 1.0 );
    return ( side/2.0 - fabs(m) );
    // positive inside. negative outside.
}

//************* Cone **************/

float ConeVolume::dist(const GLVertex& p) const {
    float h = p.z - center.z ;
    
    if (h<=0) // p is below cone
        return -1;
    else {    
        float radius = h*tan(alfa);
        float dxy = sqrt( (p.x -center.x)*(p.x -center.x) + (p.y -center.y)*(p.y -center.y) );
        return radius - dxy;
    }
//    return 0;
}

void ConeVolume::calcBB()  {
    bb.clear();
    GLVertex maxpt = GLVertex(center.x + height*tan(alfa), center.y + height*tan(alfa), center.z + height);
    GLVertex minpt = GLVertex(center.x - height*tan(alfa), center.y - height*tan(alfa), center.z );
    bb.addPoint( maxpt );
    bb.addPoint( minpt );
}

} // end namespace
// end of file volume.cpp
