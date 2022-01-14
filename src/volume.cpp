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

#include <cassert>
#include <cmath>
#include <fstream>
#include<boost/algorithm/string.hpp>

#include "volume.hpp"

#define TOLERANCE	(1e-2)

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

//************* STL **************/

MeshVolume::MeshVolume() {
    center = GLVertex(0, 0, 0);	// center is treated as the origin's offset of STL
    rotationCenter = GLVertex(0, 0, 0);
    angle  = GLVertex(0, 0, 0);
}

void MeshVolume::calcBB() {
    GLVertex maxpt;
    GLVertex minpt;
    GLVertex centreDiff =  meshCenter - center;
    // std::cout << "MeshVolume::calcBB() Center x:" << center.x << " y: " << center.y << " z: " << center.z << std::endl;
    for (int i=0; i < (int)facets.size(); i++) {
    	facets[i]->v1 += centreDiff; facets[i]->v2 += centreDiff; facets[i]->v3 += centreDiff;
    	facets[i]->normal = facets[i]->normal.rotateAC(angle.x, angle.z);
    	GLVertex v1p = facets[i]->v1 - rotationCenter;
    	facets[i]->v1 = v1p.rotateAC(angle.x, angle.z) + rotationCenter;
    	GLVertex v2p = facets[i]->v2 - rotationCenter;
    	facets[i]->v2 = v2p.rotateAC(angle.x, angle.z) + rotationCenter;
    	GLVertex v3p = facets[i]->v3 - rotationCenter;
    	facets[i]->v3 = v3p.rotateAC(angle.x, angle.z) + rotationCenter;
    }
    if (facets.size()) {
        maxpt.x = fmax(fmax(facets[0]->v1.x, facets[0]->v2.x),facets[0]->v3.x);
        maxpt.y = fmax(fmax(facets[0]->v1.y, facets[0]->v2.y),facets[0]->v3.y);
        maxpt.z = fmax(fmax(facets[0]->v1.z, facets[0]->v2.z),facets[0]->v3.z);
        minpt.x = fmin(fmin(facets[0]->v1.x, facets[0]->v2.x),facets[0]->v3.x);
        minpt.y = fmin(fmin(facets[0]->v1.y, facets[0]->v2.y),facets[0]->v3.y);
        minpt.z = fmin(fmin(facets[0]->v1.z, facets[0]->v2.z),facets[0]->v3.z);
    }
    for (int i=0; i < (int)facets.size(); i++) {
        maxpt.x = fmax(fmax(fmax(facets[i]->v1.x, facets[i]->v2.x),facets[i]->v3.x), maxpt.x);
        maxpt.y = fmax(fmax(fmax(facets[i]->v1.y, facets[i]->v2.y),facets[i]->v3.y), maxpt.y);
        maxpt.z = fmax(fmax(fmax(facets[i]->v1.z, facets[i]->v2.z),facets[i]->v3.z), maxpt.z);
        minpt.x = fmin(fmin(fmin(facets[i]->v1.x, facets[i]->v2.x),facets[i]->v3.x), minpt.x);
        minpt.y = fmin(fmin(fmin(facets[i]->v1.y, facets[i]->v2.y),facets[i]->v3.y), minpt.y);
        minpt.z = fmin(fmin(fmin(facets[i]->v1.z, facets[i]->v2.z),facets[i]->v3.z), minpt.z);
        V21.push_back(facets[i]->v2 - facets[i]->v1);
        invV21dotV21.push_back(1.0/(facets[i]->v2 - facets[i]->v1).dot(facets[i]->v2 - facets[i]->v1));
        V32.push_back(facets[i]->v3 - facets[i]->v2);
        invV32dotV32.push_back(1.0/(facets[i]->v3 - facets[i]->v2).dot(facets[i]->v3 - facets[i]->v2));
        V13.push_back(facets[i]->v1 - facets[i]->v3);
        invV13dotV13.push_back(1.0/(facets[i]->v1 - facets[i]->v3).dot(facets[i]->v1 - facets[i]->v3));
    }
    bb.clear();
    maxpt += GLVertex(TOLERANCE, TOLERANCE, TOLERANCE);
    minpt -= GLVertex(TOLERANCE, TOLERANCE, TOLERANCE);
    // std::cout << "STL maxpt x:" << maxpt.x << " y: " << maxpt.y << " z:" << maxpt.z  << "\n";
    // std::cout << "STL minpt x:" << minpt.x << " y: " << minpt.y << " z:" << minpt.z  << "\n";
    center = meshCenter;
    bb.addPoint( maxpt );
    bb.addPoint( minpt );
}

float MeshVolume::dist(const GLVertex& p) const {
	GLVertex q, r;
	GLVertex n1, n2, n3;
	double s12, s23, s31;
	float min = 1.0e+3, d, ret = -1.0, u, abs_d;
	for (int i=0; i < (int)facets.size(); i++) {
		u = (p - facets[i]->v1).dot(V21[i]) * invV21dotV21[i];
		q = facets[i]->v1 + V21[i] * u;
		d = (q - p).dot(facets[i]->normal);
		if ((abs_d = fabs(d)) > min) continue;
		r = p + facets[i]->normal * d;
		n1 = (r - facets[i]->v1).cross(V13[i]);
		n2 = (r - facets[i]->v2).cross(V21[i]);
		n3 = (r - facets[i]->v3).cross(V32[i]);
		s12 = n1.dot(n2); s23 = n2.dot(n3); s31 = n3.dot(n1);

		if ((s12 * s31 > 0.0) && (s12 * s23 > 0.0) && (s23 * s31 > 0.0))
			if (abs_d < min) {
				min = abs_d;
				ret = d;
				continue;
			}

		if (s12 <= 0.0 && s31 >= 0.0) {
			if (u > 0.0 && u < 1.0)
				/*q21 = q*/;
			else if (u <= 0.0)
				q = facets[i]->v1;
			else
				q = facets[i]->v2;
			abs_d = (q - p).norm();
			if (abs_d < min) {
				d = (q - p).dot(facets[i]->normal);
				if (d > 0.0 + TOLERANCE) {
					min = abs_d + TOLERANCE;
					ret = abs_d;
				} else {
					min = abs_d;
					ret = -abs_d;
				}
			}
		} else if (s31 <= 0.0 && s23 >= 0.0) {
			u = (p - facets[i]->v3).dot(V13[i]) * invV13dotV13[i];
			if (u > 0.0 && u < 1.0)
				q = facets[i]->v3 + V13[i] * u;
			else if (u <= 0.0)
				q = facets[i]->v3;
			else
				q = facets[i]->v1;
			abs_d = (q - p).norm();
			if (abs_d < min) {
				d = (q - p).dot(facets[i]->normal);
				if (d > 0.0 + TOLERANCE) {
					min = abs_d + TOLERANCE;
					ret = abs_d;
				} else {
					min = abs_d;
					ret = -abs_d;
				}
			}
		} else if (s23 <= 0.0 && s12 >= 0.0) {
			u = (p - facets[i]->v2).dot(V32[i]) * invV32dotV32[i];
			if (u > 0.0 && u < 1.0)
				q = facets[i]->v2 + V32[i] * u;
			else if (u <= 0.0)
				q = facets[i]->v2;
			else
				q = facets[i]->v3;
			abs_d = (q - p).norm();
			if (abs_d < min) {
				d = (q - p).dot(facets[i]->normal);
				if (d > 0.0 + TOLERANCE) {
					min = abs_d + TOLERANCE;
					ret = abs_d;
				} else {
					min = abs_d;
					ret = -abs_d;
				}
			}
		}
	}
    //std::cout << " STL dist " << ret << std::endl;
	return ret;		// positive inside. negative outside.
}

bool MeshVolume::loadMesh(boost::python::list facets){
	// Load mesh data from python facets
	// expected input
	// [[(normal),(v1), (v2), (v3)],...]
	// TODO: check the face data structure is valid

    boost::python::ssize_t len = boost::python::len(facets);

    if (!len) {
        std::cout << "Mesh data invalid" << std::endl;
		return false;
    }

    std::cout << " Load Mesh Shape from " << len << " Facets" << std::endl;

	GLVertex vertexData [4] = {};

    for(int i=0; i<len; i++){
		boost::python::ssize_t dataLen = boost::python::len(facets[i]);
		if (dataLen == 4){
				// std::cout << " Loading Data from facet:" << i << std::endl;
				for(int j=0; j<dataLen; j++){
				
					float f0 = boost::python::extract<float>(facets[i][j][0]);
					float f1 = boost::python::extract<float>(facets[i][j][1]);
					float f2 = boost::python::extract<float>(facets[i][j][2]);

					// std::cout << "Facet " << i << " vertex " << j << " x " << f0 << " y " << f1 << " z " << f2 << std::endl;

					vertexData[j].x = f0;
					vertexData[j].y = f1;
					vertexData[j].z = f2;
			}

			addFacet(new Facet(vertexData[0], vertexData[1], vertexData[2], vertexData[3]));

		}
    }

	// calculate the bounding box of the mesh volume
	calcBB();
	// file loaded successfully, return true
	return true;

}

bool MeshVolume::loadStl(boost::python::str fPath){
	// Load mesh data from binary stl file

	std::cout << "Loading Data From STL File" << std::endl;

    std::string filePath = boost::python::extract<std::string>(fPath);

	/// Open file to check if its an ASCII file

	std::string line;
	std::ifstream stlFile (filePath);
	bool binaryFile = false;
	

	if (stlFile.is_open())
	{
		getline(stlFile,line);
		// ASCII stl files should begin with the word 'solid'
		// Check if the first line of the file contains 'solid'
		if (!boost::algorithm::contains(line, "solid"))
		{
			binaryFile = true;
			stlFile.close();
		}

		if (!binaryFile){

			GLVertex v, n;
			std::vector<GLVertex> vertices;
			int vertexTracker = 0;
			int vertexTotal = 0;
			int facetTotal = 0;
			std::string stlString;

			while ( getline(stlFile,line) )
			{
				// Typical facet definition

				//  facet normal -0.000000 -1.000000 -0.000000
				//    outer loop
				//      vertex 100.000000 0.000000 10.000000
				//      vertex 0.000000 0.000000 10.000000
				//      vertex 100.000000 0.000000 0.000000
				//    endloop
				//  endfacet

				/// normal
				stlString = "facet normal";
				if (boost::algorithm::contains(line, stlString)){
					n = parseStlLine(line, stlString);
				}

				/// vertex
				stlString = "vertex";
				if (boost::algorithm::contains(line, stlString)){
					v = parseStlLine(line, stlString);
					vertices.push_back(v);
					vertexTotal++;
					vertexTracker++;
				}

				//endfacet
				stlString = "endfacet";
				if (boost::algorithm::contains(line, stlString)){
					if (vertices.size() == 3){
						addFacet(new Facet(n, vertices[0], vertices[1], vertices[2]));
					}else{
						//TODO: handle processing errors
						std::cout << "Error processing vertex" << std::endl;
					}
					vertexTracker = 0;
					vertices.clear();
					facetTotal++;
				}

				//endsolid
				stlString = "endsolid";
				if (boost::algorithm::contains(line, stlString)){
					std::cout << "stl import complete - processed " << facetTotal << " facets, with " << vertexTotal << " vertices" << std::endl;
				}
			}
		}
	}else{
		//TODO: handle processing errors
		std::cout << "Error opening stl file" << std::endl;
	}

	/// Binary 
	if (binaryFile){

		std::ifstream stlFile(filePath.c_str(), std::ios::in | std::ios::binary);

		if (!stlFile) {
			std::cout << "Error reading file" << std::endl;
			assert(!stlFile);
			return false;
		}

		char headerData[80] = "";
		char triangleData[4];
		stlFile.read(headerData, 80);
		stlFile.read(triangleData, 4);
		
		std::string headerString(headerData);
		//TODO: The header may contain ASCII string for units
		// see: https://en.wikipedia.org/wiki/STL_(file_format)#ASCII_STL
		std::cout << "STL Header: " << headerString << std::endl;

		// cast triangleData to an unsigned int
		unsigned int triangleCount = static_cast<unsigned int>( *triangleData);

		std::cout << "Importing " << triangleCount << " Triangles" << std::endl;

		if (!triangleCount) {
			std::cout << "Error reading data" << std::endl;
			assert(!triangleCount);
			return false;
		}

		for (unsigned int i = 0; i < triangleCount; i++) {
			
			GLVertex normal = parseStlData(stlFile);
			GLVertex v1 = parseStlData(stlFile);
			GLVertex v2 = parseStlData(stlFile);
			GLVertex v3 = parseStlData(stlFile);

			addFacet(new Facet(normal, v1, v2, v3));

			char attribute[2];
			stlFile.read(attribute, 2);
		}
	}

	stlFile.close();
	// calculate the bounding box of the mesh volume
	calcBB();
	// file loaded successfully, return true
	return true;
}

GLVertex MeshVolume::parseStlData(std::ifstream& stlFile){
	// parse the binary stl data to a GLVertex and return
	// TODO: error checking required
	// valid numerical data?

	float x;
	stlFile.read(reinterpret_cast<char*>(&x), sizeof(float));
	float y;
	stlFile.read(reinterpret_cast<char*>(&y), sizeof(float));
	float z;
	stlFile.read(reinterpret_cast<char*>(&z), sizeof(float));

	return GLVertex(x,y,z);
}

GLVertex MeshVolume::parseStlLine(std::string line, std::string stlString){
	// parse the ascii stl line to a GLVertex and return
	// TODO: error checking required
	// valid numerical data?

	boost::algorithm::replace_all(line, stlString, "");
	boost::algorithm::trim(line);
	std::vector<std::string> strVec;
	strVec = boost::algorithm::split(strVec, line, boost::algorithm::is_space());

	GLVertex vertex;
	if (strVec.size() == 3)
	{
		vertex.x = std::stof(strVec[0]);
		vertex.y = std::stof(strVec[1]);
		vertex.z = std::stof(strVec[2]);
	}

	return vertex;
}

} // end namespace
// end of file volume.cpp
