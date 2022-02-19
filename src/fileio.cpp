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
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/python.hpp>

#include "fileio.hpp"

namespace cutsim
{

	FileIO::FileIO()
	{
		//std::cout << "FileIO object created" << std::endl;
	}

	FileIO::~FileIO()
	{
		//std::cout << "FileIO object destroyed" << std::endl;
		facets.clear();
	}

	std::vector<Facet *> FileIO::getFacets()
	{

		//TODO: Check the facets are valid before returning
		return facets;
	}

	bool FileIO::loadStl(boost::python::str fPath)
	{

		// Load mesh data from binary stl file
		std::cout << "Loading Data From STL File" << std::endl;
		std::string filePath = boost::python::extract<std::string>(fPath);

		// clear the facets
		facets.clear();

		/// Open file to check if its an ASCII file
		std::string line;
		std::ifstream stlFile(filePath);
		bool binaryFile = false;

		if (stlFile.is_open())
		{
			getline(stlFile, line);
			// ASCII stl files should begin with the word 'solid'
			// Check if the first line of the file contains 'solid'
			if (!boost::algorithm::contains(line, "solid"))
			{
				binaryFile = true;
				stlFile.close();
			}

			if (!binaryFile)
			{

				GLVertex v, n; // vertex, normal
				std::vector<GLVertex> vertices;
				int vertexTracker = 0;
				int vertexTotal = 0;
				int facetTotal = 0;
				std::string stlString;

				while (getline(stlFile, line))
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
					if (boost::algorithm::contains(line, stlString))
					{
						n = parseStlLine(line, stlString);
					}

					/// vertex
					stlString = "vertex";
					if (boost::algorithm::contains(line, stlString))
					{
						v = parseStlLine(line, stlString);
						vertices.push_back(v);
						vertexTotal++;
						vertexTracker++;
					}

					//endfacet
					stlString = "endfacet";
					if (boost::algorithm::contains(line, stlString))
					{
						if (vertices.size() == 3)
						{
							facets.push_back(new Facet(n, vertices[0], vertices[1], vertices[2]));
						}
						else
						{
							//TODO: handle processing errors
							std::cout << "Error processing vertex" << std::endl;
						}
						vertexTracker = 0;
						vertices.clear();
						facetTotal++;
					}

					//endsolid
					stlString = "endsolid";
					if (boost::algorithm::contains(line, stlString))
					{
						std::cout << "stl import complete - processed " << facetTotal << " facets, with " << vertexTotal << " vertices" << std::endl;
						return true;
					}
				}

				//reached the end of the file, possibly not an stl of formatting is incorrect.
				return false;
			}
		}
		else
		{
			//TODO: handle processing errors
			std::cout << "Error opening stl file" << std::endl;
			return false;
		}

		/// Binary
		if (binaryFile)
		{

			std::ifstream stlFile(filePath.c_str(), std::ios::in | std::ios::binary);

			if (!stlFile)
			{
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
			unsigned int triangleCount = static_cast<unsigned int>(*triangleData);

			std::cout << "Importing " << triangleCount << " Triangles" << std::endl;

			if (!triangleCount)
			{
				std::cout << "Error reading data" << std::endl;
				assert(!triangleCount);
				return false;
			}

			for (unsigned int i = 0; i < triangleCount; i++)
			{

				GLVertex normal = parseStlData(stlFile);
				GLVertex v1 = parseStlData(stlFile);
				GLVertex v2 = parseStlData(stlFile);
				GLVertex v3 = parseStlData(stlFile);

				facets.push_back(new Facet(normal, v1, v2, v3));

				char attribute[2];
				stlFile.read(attribute, 2);
			}
		}

		stlFile.close();

		// file loaded successfully, return true
		return true;
	}

	GLVertex FileIO::parseStlData(std::ifstream &stlFile)
	{
		// parse the binary stl data to a GLVertex and return
		// TODO: error checking required
		// valid numerical data?

		float x;
		stlFile.read(reinterpret_cast<char *>(&x), sizeof(float));
		float y;
		stlFile.read(reinterpret_cast<char *>(&y), sizeof(float));
		float z;
		stlFile.read(reinterpret_cast<char *>(&z), sizeof(float));

		return GLVertex(x, y, z);
	}

	GLVertex FileIO::parseStlLine(std::string line, std::string stlString)
	{
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

	bool FileIO::loadMesh(boost::python::list pyfacets)
	{
		// Load mesh data from python facets
		// expected input
		// [[(normal),(v1), (v2), (v3)],...]
		// TODO: check the face data structure is valid

		// clear the facets
		facets.clear();

		boost::python::ssize_t len = boost::python::len(pyfacets);

		if (!len)
		{
			std::cout << "Mesh data invalid" << std::endl;
			return false;
		}

		std::cout << " Load Mesh Shape from " << len << " Facets" << std::endl;

		GLVertex vertexData[4] = {};

		for (int i = 0; i < len; i++)
		{
			boost::python::ssize_t dataLen = boost::python::len(pyfacets[i]);
			if (dataLen == 4)
			{
				// std::cout << " Loading Data from facet:" << i << std::endl;
				for (int j = 0; j < dataLen; j++)
				{

					float f0 = boost::python::extract<float>(pyfacets[i][j][0]);
					float f1 = boost::python::extract<float>(pyfacets[i][j][1]);
					float f2 = boost::python::extract<float>(pyfacets[i][j][2]);

					// std::cout << "Facet " << i << " vertex " << j << " x " << f0 << " y " << f1 << " z " << f2 << std::endl;

					vertexData[j].x = f0;
					vertexData[j].y = f1;
					vertexData[j].z = f2;
				}

				facets.push_back(new Facet(vertexData[0], vertexData[1], vertexData[2], vertexData[3]));
			}
		}

		// file loaded successfully, return true
		return true;
	}

	/// export stl file and return the path to python
	std::string FileIO::writeStl(std::vector<unsigned int> indexArray, std::vector<GLVertex> vertexArray, boost::python::str fPath, bool binary)
	{

		boost::python::str out;
		std::ofstream stlFile;

		std::string filePath = boost::python::extract<std::string>(fPath);

		/// make sure the last charater isn't a seperator
		std::string lastchar = filePath.substr(filePath.length() - 1);
		if (boost::algorithm::contains(lastchar, "\\") || boost::algorithm::contains(lastchar, "/"))
		{
			filePath = filePath + "libcutsim.stl";
		}

		/// check for a .stl file extension
		std::string ext = filePath.substr(filePath.length() - 4);
		boost::algorithm::to_lower(ext);

		if (ext != ".stl")
		{
			filePath = filePath + ".stl";
		}

		/// split the filename from the path name and check the path exists
		std::size_t pos = filePath.find_last_of("/\\");
		std::string path = filePath.substr(0, pos);

		if (!std::filesystem::exists(path))
		{
			std::filesystem::create_directory(path);
		}

		if (binary)
		{

			stlFile.open(filePath.c_str(), std::ios::out | std::ios::binary);
			std::string header_info = "output";
			char head[81];
			std::strncpy(head, header_info.c_str(), 81);
			unsigned long len = indexArray.size() / 3;

			stlFile.write(head, 80);
			stlFile.write((char *)&len, 4);
		}
		else
		{
			stlFile.open(filePath.c_str());
			stlFile << "solid libcutsim" << std::endl;
		}

		for (unsigned int n = 0; n < indexArray.size(); n += 3)
		{

			GLVertex p1 = vertexArray[n];
			GLVertex p2 = vertexArray[n + 1];
			GLVertex p3 = vertexArray[n + 2];

			if (binary)
			{
				//normal coordinates
				stlFile.write((char *)&p1.nx, sizeof(p1.nx));
				stlFile.write((char *)&p1.ny, sizeof(p1.ny));
				stlFile.write((char *)&p1.nz, sizeof(p1.nz));

				//p1 coordinates
				stlFile.write((char *)&p1.x, sizeof(p1.x));
				stlFile.write((char *)&p1.y, sizeof(p1.y));
				stlFile.write((char *)&p1.z, sizeof(p1.z));

				//p2 coordinates
				stlFile.write((char *)&p2.x, sizeof(p2.x));
				stlFile.write((char *)&p2.y, sizeof(p2.y));
				stlFile.write((char *)&p2.z, sizeof(p2.z));

				//p3 coordinates
				stlFile.write((char *)&p3.x, sizeof(p3.x));
				stlFile.write((char *)&p3.y, sizeof(p3.y));
				stlFile.write((char *)&p3.z, sizeof(p3.z));

				char attribute[2] = "0";
				stlFile.write(attribute, sizeof(attribute));
			}
			else
			{

				stlFile << "facet normal " << p1.nx << " " << p1.ny << " " << p1.nz << std::endl;
				stlFile << "  outer loop" << std::endl;
				stlFile << "      vertex " << p1.x << " " << p1.y << " " << p1.z << std::endl;
				stlFile << "      vertex " << p2.x << " " << p2.y << " " << p2.z << std::endl;
				stlFile << "      vertex " << p3.x << " " << p3.y << " " << p3.z << std::endl;
				stlFile << "  endloop" << std::endl;
				stlFile << "endfacet" << std::endl;
			}
		}

		if (!binary)
		{
			stlFile << "endsolid libcutsim" << std::endl;
		}

		stlFile.close();
		//return boost::python::str(filePath);
		return filePath;
	}

} // end namespace
// end of file stl.cpp
