/*  
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of Cutsim/OpenCAMlib.
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

#include <boost/python.hpp>

#include <iostream>
#include <set>
#include <cmath>
#include <vector>

#include <boost/foreach.hpp>

#include "glvertex.hpp"

namespace cutsim {

class Octnode;

/// additional vertex data not needed for OpenGL rendering
/// but required for the isosurface or cutting-simulation algorithm.
struct VertexData {
    /// string output
    void str() {
        BOOST_FOREACH( unsigned int pIdx, polygons ) {
            std::cout << pIdx << " ";
        }
    }
    /// insert polygon-id to polygon set
    inline void addPolygon( unsigned int idx ) { polygons.insert( idx ); }
    /// remove polygon-id from polygon set
    inline void removePolygon(unsigned int idx ) { polygons.erase( idx ); }
    /// is the polygon set empty?
    inline bool empty() { return polygons.empty(); }
// DATA
    /// The set of polygons. Each polygon has an uint index which is stored here.
    /// Note: we want to access polygons from highest index to lowest, thus compare with "greater"
    typedef std::set< unsigned int, std::greater<unsigned int> > PolygonSet;
    /// the polygons to which this vertex belongs. i.e. for each vertex we store in this set all the polygons to which it belongs.
    PolygonSet polygons;
    /// the Octnode that created this vertex. 
    /// when an Octnode is cut the corresponding vertex/vertices are deleted.
    /// when a vertex is deleted, the Octnode that generated it is notified
    Octnode* node;
    // (an alternative callback-mechanism would be to store a function-pointer or similar)
};


// the "secret sauce" paper suggests the following primitives
//   http://www.cs.berkeley.edu/~jrs/meshpapers/SchaeferWarren2.pdf
//   or
//   http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.13.2631
//
// - add vertex  
//   add vertex with empty polygon list and pointer to octree-node
//
// - remove vertex (also removes associated polygons)
//   process list of polygons, from highest to lowest. call remove_polygon on each poly.
//   overwrite with last vertex. shorten list. request each poly to re-number.
// 
// - add polygon DONE
//   append new polygon to end of list, request each vertex to add new polygon to list.
//
// - remove_polygon( polygonIndex ) 
//   i) for each vertex: request remove this polygons index from list
//   ii) then remove polygon from polygon-list: overwrite with last polygon, then shorten list.
//   iii) process each vertex in the moved polygon, request renumber on each vert for this poly
//
// data structure:
//  vertex-table: index, pos(x,y,z)  , polygons(id1,id2,...), Node-pointer to octree 
// polygon-table: index, vertex-list
//

/// a GLData object holds data which is drawn by OpenGL 
///
/// holds two arrays vertexArray and indexArray for OpenGL drawing
/// with 
class GLData {
public:
    GLData();
    unsigned int addVertex(float x, float y, float z, float r, float g, float b);
    unsigned int addVertex(GLVertex v, Octnode* n);
    unsigned int addVertex(float x, float y, float z, float r, float g, float b, Octnode* n);
    void setNormal(unsigned int vertexIdx, float nx, float ny, float nz);
    void modifyVertex( unsigned int id, float x, float y, float z, float r, float g, float b, float nx, float ny, float nz);
    void removeVertex( unsigned int vertexIdx );
    int addPolygon( std::vector<unsigned int>& verts);
    void removePolygon( unsigned int polygonIdx);
    std::string str();
    boost::python::list get_triangles() {
        boost::python::list out;
        for (unsigned int n=0;n<indexArray.size();n+=3) {
            GLVertex p1 = vertexArray[n ];
            GLVertex p2 = vertexArray[n+1];
            GLVertex p3 = vertexArray[n+2];
            boost::python::list tri;
            tri.append(p1);
            tri.append(p2);
            tri.append(p3);
            out.append(tri);
        }
        return out;
    }
// type of GLData
    void setTriangles() {
        triangles=true;
        lines=false;
        polyVerts=3;
    }
    void setLines() {
        lines=true;
        triangles=false;
        polyVerts=2;
    }

    /// byte-offset for coordinate data in the vertexArray
    static const unsigned int vertex_offset = 0;
    /// byte-offset for color data in the vertexArray
    static const unsigned int color_offset = 12;
    /// byte-offset for normal data in the vertexArray
    static const unsigned int normal_offset = 24;
    
    /// pointer to the vertex-array
    const GLVertex* getVertexArray() const { return vertexArray.data(); }
    /// pointer to the index-array
    const unsigned int* getIndexArray() const { return indexArray.data(); }
    /// number of vertices per polygon (usually 3 or 4)
    inline const int polygonVertices() const { return polyVerts; }

    /// length of indexArray
    inline const int indexCount() const { return indexArray.size(); }
    


protected:
    /// set type for GL-rendering, e.g. GL_TRIANGLES, GL_QUADS, etc.
    //void setType(GLenum t) { glp.type = t; }
// data. double buffered. rendering uses [renderIndex], worker-task uses [workIndex]
    /// vertex coordinates
    //QVarLengthArray<GLVertex>    vertexArray;
    std::vector<GLVertex>    vertexArray;
    /// non-OpenGL data associated with vertices. This correspoinds allways to the workIndex.
    /// only one array, since not needed for OpenGL drawing!
    //QVarLengthArray<VertexData>  vertexDataArray; 
    std::vector<VertexData>  vertexDataArray; 
    /// polygon indices
    std::vector<unsigned int>      indexArray;
    /// parameters for rendering this GLData
    //GLParameters glp;
    int polyVerts;
    bool triangles;
    bool lines;
};

} // end namespace
