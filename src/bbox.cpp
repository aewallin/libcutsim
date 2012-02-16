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
#include <sstream>
#include <iostream>

#include "bbox.hpp"

namespace cutsim {


Bbox::Bbox() {
    minpt = GLVertex(0,0,0);
    maxpt = GLVertex(0,0,0);
    initialized = false;
}
//              minx       maxx        miny       maxy       minz       maxz
Bbox::Bbox(double b1, double b2, double b3, double b4, double b5, double b6) {
    minpt = GLVertex(b1,b3,b5);
    maxpt = GLVertex(b2,b4,b6);
    initialized = true;
}

void Bbox::clear() {
    initialized = false;
}

void Bbox::addPoint(const GLVertex &p) {
    if (!initialized) {
        maxpt = p;
        minpt = p;
        initialized = true;
    } else {
        if (p.x > maxpt.x)
            maxpt.x = p.x;
        if (p.x < minpt.x)
            minpt.x = p.x;
        
        if (p.y > maxpt.y)
            maxpt.y = p.y;
        if (p.y < minpt.y)
            minpt.y = p.y;
        
        if (p.z > maxpt.z)
            maxpt.z = p.z;
        if (p.z < minpt.z)
            minpt.z = p.z;
    }
}


/// does this Bbox overlap with b?
bool Bbox::overlaps(const Bbox& b) const {
    if  ( (this->maxpt.x < b.minpt.x) || (this->minpt.x > b.maxpt.x) )
        return false;
    else if ( (this->maxpt.y < b.minpt.y) || (this->minpt.y > b.maxpt.y) )
        return false;
    else if ( (this->maxpt.z < b.minpt.z) || (this->minpt.z > b.maxpt.z) )
        return false;
    else
        return true;
}

} // end namespace
// end of file
