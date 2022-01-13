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

#include <boost/python.hpp>
namespace bp = boost::python;

#include "cutsim.hpp"
#include "gldata.hpp"
#include "volume.hpp"
#include "isosurface.hpp"

// python wrapper for libcutsim classes & functions

BOOST_PYTHON_MODULE(libcutsim) {
    using namespace cutsim;
    
    bp::class_<Cutsim>("Cutsim", bp::no_init)
        .def(bp::init<double, unsigned int, GLData*, IsoSurfaceAlgorithm* >())
        .def("init", &Cutsim::init)
        .def("diff_volume", &Cutsim::diff_volume)
        .def("sum_volume", &Cutsim::sum_volume)
        .def("updateGL", &Cutsim::updateGL)
        .def("__str__", &Cutsim::str)
    ;
    bp::class_<GLData>("GLData")
        .def("get_triangles", &GLData::get_triangles)
        .def("get_lines", &GLData::get_lines)
        .def("get_stl", &GLData::get_stl)

        .def("__str__", &GLData::str)
    ;
    bp::class_<GLVertex>("GLVertex")
        .add_property("x", &GLVertex::x)
        .add_property("y", &GLVertex::y)
        .add_property("z", &GLVertex::z)
        .add_property("r", &GLVertex::r)
        .add_property("g", &GLVertex::g)
        .add_property("b", &GLVertex::b)
    ;
    bp::class_<Volume>("Volume")
        .def("setCenter", &Volume::setCenter)
    ;
    bp::class_< SphereVolume, bp::bases<Volume> >("SphereVolume")
        .def("setRadius", &SphereVolume::setRadius)
    ;
    bp::class_< CubeVolume, bp::bases<Volume> >("CubeVolume")
        .def("setSide", &CubeVolume::setSide)
    ;
    bp::class_< ConeVolume, bp::bases<Volume> >("ConeVolume")
        .def("setHeight", &ConeVolume::setHeight)
    ;
    bp::class_< MeshVolume, bp::bases<Volume> >("MeshVolume")
        .def("loadMesh", &MeshVolume::loadMesh)
        .def("loadStl", &MeshVolume::loadStl)
        .def("setMeshCenter", &MeshVolume::setMeshCenter)
    ;
    bp::class_<IsoSurfaceAlgorithm>("IsoSurfaceAlgorithm")
    ;
    bp::class_< MarchingCubes, bp::bases<IsoSurfaceAlgorithm> >("MarchingCubes")
    ;
    bp::class_< CubeWireFrame, bp::bases<IsoSurfaceAlgorithm> >("CubeWireFrame")
    ;

}
