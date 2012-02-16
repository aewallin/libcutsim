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

#include <cassert>
#include <vector>

#include "gldata.hpp"
#include "octree.hpp"
#include "octnode.hpp"

namespace cutsim {

/// abstract base class for isosurface extraction algorithms
/// 
/// isosurface algorithms produce vertices and polygons based on an Octree
/// vertices and polygons are added to a GLData using addVertex, addPolygon, etc.
///
class IsoSurfaceAlgorithm {
public:
    /// create algorithm wich writes to given GLData and reads from given Octree
    IsoSurfaceAlgorithm()  {}
    virtual ~IsoSurfaceAlgorithm() {}
    void set_gl(GLData* gl) { g = gl; }
    void set_tree(Octree* tr) {tree = tr;}
    virtual void set_polyVerts() {}
    // return polygons corresponding to the octree node
    /// update GLData
    virtual void updateGL() { 
        updateGL( tree->root );
    }
protected:
    /// update the GLData for the given Octnode. re-implement in sub-class
    virtual void updateGL( Octnode* node) {}
    
// DATA
    /// how many updateGL calls were made? for debug
    int update_calls;
    /// how many valid nodes? for debug
    int valid_count;
    /// the GLData to which we udpate vertices/polygons
    GLData* g;
    /// the Octree which we traverse to update GLData
    Octree* tree;
};


} // end namespace

// end file isosurface.hpp
