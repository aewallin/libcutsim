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

#ifndef FILEIO_H
#define FILEIO_H

#include "facet.hpp"
#include "glvertex.hpp"

#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace cutsim {

class FileIO {

   public:
    FileIO();
    ~FileIO();
    bool loadStl(boost::python::str);
    bool loadMesh(boost::python::list);
    std::vector<Facet*> getFacets();
    std::string writeStl(std::vector<unsigned int> indexArray, std::vector<GLVertex> vertexArray, boost::python::str fPath, bool binary);


   private:
    GLVertex parseStlData(std::ifstream&);
    GLVertex parseStlLine(std::string, std::string);

    std::vector<Facet*> facets;

};

}// end namespace
// end file volume.hpp

#endif // FILEIO_H