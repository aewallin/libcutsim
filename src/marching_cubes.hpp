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
#include <vector>

#include <boost/foreach.hpp>

#include "isosurface.hpp"
#include "bbox.hpp"
#include "octnode.hpp"
#include "gldata.hpp"

namespace cutsim {

/// Marching-cubes isosurface extraction from distance field stored in Octree
/// see http://en.wikipedia.org/wiki/Marching_cubes
///
class MarchingCubes : public IsoSurfaceAlgorithm {
public:
    /// create algorithm
    MarchingCubes() : IsoSurfaceAlgorithm() {}
    virtual void set_polyVerts(unsigned int ) { g->setTriangles(); }
    virtual ~MarchingCubes() { }
protected:
    void updateGL(Octnode* node);
    void mc_node(Octnode* node); 
    /// based on the f[] values, generate a list of interpolated vertices, all on the edges of the node.
    /// These vertices are later used for defining triangles.
    std::vector<GLVertex> interpolated_vertices(const Octnode* node, unsigned int edges) ;
    GLVertex interpolate(const Octnode* node, int idx1, int idx2);
// DATA
    /// get table-index based on the funcion values (positive or negative) at the corners
    unsigned int mc_edgeTableIndex(const Octnode* node);
    /// Marching-Cubes edge table
    static const unsigned int edgeTable[256];
    /// Marching-Cubes triangle table
    static const int triTable[256][16]; 
};

} // end namespace

// end file marching_cubes.hpp
