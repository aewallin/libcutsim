/*  
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenCAMlib.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

#include <boost/bind.hpp>
#include <boost/timer.hpp>

#include "octree.hpp"
#include "octnode.hpp"
#include "volume.hpp"
#include "marching_cubes.hpp"
#include "cube_wireframe.hpp"
#include "gldata.hpp"

namespace cutsim {

/// a Cutsim stores an Octree stock model, uses an iso-surface extraction
/// algorithm to generate surface triangles, and communicates with
/// the corresponding GLData surface which is used for rendering
/// 
/// the stock is modified using boolean operations:
/// - sum / unioin
/// - subtract / difference
/// - 'product' / intersection
///
/// after a boolean operation, caliing updateGL() will update the graphics object
///
class Cutsim  {
    
public:
    /// create a cutting simulation
    /// \param octree_size side length of the depth=0 octree cube
    /// \param octree_max_depth maximum sub-division depth of the octree
    ///        each linear dimension (xyz) will potentially be divided into
    ///        smaller cubes, with a minimum side-length of octree_size/pow(2,max_depth)
    ///        in practice max_depth= 6 or 7 works for testing, and 9 or 10 looks very smooth (but is slower)
    /// \param gld the GLData used to draw the stock
    Cutsim(double octree_size, unsigned int octree_max_depth, GLData* gld, IsoSurfaceAlgorithm* iso);
    virtual ~Cutsim();
    /// subtract/diff given Volume
    void diff_volume( const Volume* vol );
    /// sum/union given Volume
    void sum_volume( const Volume* vol );
    /// intersect/"and" given Volume
    void intersect_volume( const Volume* vol );
    /// update the GL-data
    void updateGL(); 
    void init(unsigned int n);
    std::string str() const;
private:
    IsoSurfaceAlgorithm* iso_algo; // the isosurface-extraction algorithm to use
    Octree* tree; // this is the stock model
    GLData* g; // this is the graphics object representing the stock
};

} // end namespace
