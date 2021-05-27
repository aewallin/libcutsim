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

#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cassert>
#include <set>
#include <vector>


#include "gldata.hpp"
#include "octnode.hpp"

namespace cutsim {

GLData::GLData() {
    polyVerts = 3;
}

/// add a vertex with given position and color, return its index
unsigned int GLData::addVertex(float x, float y, float z, float r, float g, float b) {
    return addVertex( GLVertex(x,y,z,r,g,b), NULL );
}

/// add vertex, associate given Octnode with the vertex, and return index
unsigned int GLData::addVertex(GLVertex v, Octnode* n) {
    // add vertex with empty polygon-list.
    unsigned int idx = vertexArray.size();
    vertexArray.push_back(v);
    vertexDataArray.push_back( VertexData() );
    vertexDataArray[idx].node = n;
    assert( vertexArray.size() == vertexDataArray.size() );
    return idx; // return index of newly appended vertex
}

/// add vertex at given position
unsigned int GLData::addVertex(float x, float y, float z, float r, float g, float b, Octnode* n) {
    unsigned int id = addVertex(x,y,z,r,g,b);
    vertexDataArray[id].node = n;
    return id;
}

/// set vertex normal
void GLData::setNormal(unsigned int vertexIdx, float nx, float ny, float nz) {
    vertexArray[vertexIdx].setNormal(nx,ny,nz);
}

/// modify given vertex
void GLData::modifyVertex( unsigned int id, float x, float y, float z, float r, float g, float b, float nx, float ny, float nz) {
    GLVertex p = GLVertex(x,y,z,r,g,b,nx,ny,nz);
    vertexArray[id] = p;
}

/// remove vertex with given index
void GLData::removeVertex( unsigned int vertexIdx ) {
    // i) for each polygon of this vertex, call remove_polygon:
    typedef std::set< unsigned int, std::greater<unsigned int> > PolygonSet;
    PolygonSet pset = vertexDataArray[vertexIdx].polygons;
    BOOST_FOREACH( unsigned int polygonIdx, pset ) {
        removePolygon( polygonIdx );
    }
    // ii) overwrite with last vertex:
    unsigned int lastIdx = vertexArray.size()-1;
    if (vertexIdx != lastIdx) {
        vertexArray[vertexIdx] = vertexArray[lastIdx];
        vertexDataArray[vertexIdx] = vertexDataArray[lastIdx];
        // notify octree-node with new index here!
        // vertex that was at lastIdx is now at vertexIdx
        vertexDataArray[vertexIdx].node->swapIndex( lastIdx, vertexIdx );
        
        // request each polygon to re-number this vertex.
        BOOST_FOREACH( unsigned int polygonIdx, vertexDataArray[vertexIdx].polygons ) {
            unsigned int idx = polygonIdx*polygonVertices();
            for (int m=0;m<polygonVertices();++m) {
                if ( indexArray[ idx+m ] == lastIdx )
                    indexArray[ idx+m ] = vertexIdx;
            }
        }
    }
    // shorten array
    vertexArray.resize( vertexArray.size()-1 );
    vertexDataArray.resize( vertexDataArray.size()-1 );
    assert( vertexArray.size() == vertexDataArray.size() );
}

/// add a polygon, return its index
int GLData::addPolygon( std::vector<unsigned int>& verts) {
    // append to indexArray, then request each vertex to update
    unsigned int polygonIdx = indexArray.size()/polygonVertices();
    BOOST_FOREACH( unsigned int vertex, verts ) {
        indexArray.push_back(vertex);
        vertexDataArray[vertex].addPolygon(polygonIdx); // add index to vertex i1
    }
    return polygonIdx;
}

/// remove polygon at given index
void GLData::removePolygon( unsigned int polygonIdx) {
    unsigned int idx = polygonVertices()*polygonIdx; // start-index for polygon
    // i) request remove for each vertex in polygon:
    for (int m=0; m<polygonVertices() ; ++m) // this polygon has the following 3/4 vertices. we call removePolygon on them all
        vertexDataArray[ indexArray[idx+m]   ].removePolygon(polygonIdx);
    unsigned int last_index = (indexArray.size()-polygonVertices());
    // if deleted polygon is last on the list, do nothing??
    if (idx!=last_index) { 
        // ii) remove from polygon-list by overwriting with last element
        for (int m=0; m<polygonVertices(); ++m)
            indexArray[idx+m  ] = indexArray[ last_index+m   ];
        // iii) for the moved polygon, request that each vertex update the polygon number
        for (int m=0; m<polygonVertices() ; ++m) {
            vertexDataArray[ indexArray[idx+m   ] ].addPolygon( idx/polygonVertices() ); // this is the new polygon index
            vertexDataArray[ indexArray[idx+m   ] ].removePolygon( last_index/polygonVertices() ); // this polygon is no longer there!
        }
    }
    indexArray.resize( indexArray.size()-polygonVertices() ); // shorten array
} 

/// string output
std::string GLData::str() {
    std::string s;
    std::stringstream out;
    out << "GLData(" << polyVerts << ") " << vertexArray.size() << " vertices and " << indexArray.size() << " indices.";
    s = out.str();
    return s;
}

/// export stl file and return the path to python
boost::python::str GLData::get_stl(boost::python::str fPath, bool binary = true) {
    
    boost::python::str out;
    std::ofstream stlFile;

    std::string filePath = boost::python::extract<std::string>(fPath);

    /// make sure the last charater isn't a seperator
    std::string lastchar = filePath.substr(filePath.length() - 1);
    if (boost::algorithm::contains(lastchar,"\\") || boost::algorithm::contains(lastchar,"/") ){
        filePath = filePath + "libcutsim.stl";
    }

    /// check for a .stl file extension
    std::string ext = filePath.substr(filePath.length() - 4);
    boost::algorithm::to_lower(ext);

    if ( ext != ".stl") {
        filePath = filePath + ".stl";
    }

    /// split the filename from the path name and check the path exists
    std::size_t pos = filePath.find_last_of("/\\");
    std::string path = filePath.substr(0, pos);

    if (!std::filesystem::exists(path)){
        std::filesystem::create_directory(path);
    }

    if (binary){

        stlFile.open(filePath.c_str(),  std::ios::out | std::ios::binary);
        std::string header_info = "output";
        char head[81];
        std::strncpy(head, header_info.c_str(), 81);
        unsigned long len = indexArray.size() / 3;
        
        stlFile.write(head, 80);
        stlFile.write((char*)&len, 4);

    }else{
        stlFile.open(filePath.c_str());
        stlFile << "solid libcutsim" << std::endl;
    }

    for (unsigned int n=0;n<indexArray.size();n+=3) {

        GLVertex p1 = vertexArray[n];
        GLVertex p2 = vertexArray[n+1];
        GLVertex p3 = vertexArray[n+2];

        if(binary){
            //normal coordinates
            stlFile.write((char*)&p1.nx, sizeof(p1.nx));
            stlFile.write((char*)&p1.ny, sizeof(p1.ny));
            stlFile.write((char*)&p1.nz, sizeof(p1.nz));

            //p1 coordinates
            stlFile.write((char*)&p1.x, sizeof(p1.x));
            stlFile.write((char*)&p1.y, sizeof(p1.y));
            stlFile.write((char*)&p1.z, sizeof(p1.z));

            //p2 coordinates
            stlFile.write((char*)&p2.x, sizeof(p2.x));
            stlFile.write((char*)&p2.y, sizeof(p2.y));
            stlFile.write((char*)&p2.z, sizeof(p2.z));

            //p3 coordinates
            stlFile.write((char*)&p3.x, sizeof(p3.x));
            stlFile.write((char*)&p3.y, sizeof(p3.y));
            stlFile.write((char*)&p3.z, sizeof(p3.z));

            char attribute[2] = "0";
            stlFile.write(attribute,sizeof(attribute));

        }else{

            stlFile << "facet normal " << p1.nx << " " << p1.ny << " " << p1.nz << std::endl;
            stlFile << "  outer loop" << std::endl;
            stlFile << "      vertex " << p1.x << " " << p1.y << " " << p1.z << std::endl;
            stlFile << "      vertex " << p2.x << " " << p2.y << " " << p2.z << std::endl;
            stlFile << "      vertex " << p3.x << " " << p3.y << " " << p3.z << std::endl;
            stlFile << "  endloop" << std::endl;
            stlFile << "endfacet" << std::endl;
            
        }
    }

    if(!binary){
        stlFile << "endsolid libcutsim" << std::endl;
    }

    stlFile.close();
    return boost::python::str(filePath);
}

} // end cutsim namespace

