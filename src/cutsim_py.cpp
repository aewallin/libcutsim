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

// this is the default font used if not specified on commandline
// #define TTFONT "/usr/share/fonts/truetype/freefont/FreeSerifBoldItalic.ttf"
//#define TTFONT "/usr/share/fonts/truetype/freefont/FreeSerif.ttf"

 
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
    ;
    bp::class_< SphereVolume, bp::bases<Volume> >("SphereVolume")
        .def("setRadius", &SphereVolume::setRadius)
        .def("setCenter", &SphereVolume::setCenter)
    ;
    bp::class_<IsoSurfaceAlgorithm>("IsoSurfaceAlgorithm")
    ;
    bp::class_< MarchingCubes, bp::bases<IsoSurfaceAlgorithm> >("MarchingCubes")
    ;
    /*
    bp::class_<Writer>("Writer")
        .add_property("arc", &Writer::get_arc, &Writer::set_arc)
        .add_property("conic", &Writer::get_conic, &Writer::set_conic)
        .add_property("cubic", &Writer::get_cubic, &Writer::set_cubic)
        .add_property("scale", &Writer::get_scale, &Writer::set_scale)
        .add_property("conic_biarc_subdivision", &Writer::get_conic_biarc_subdiv, &Writer::set_conic_biarc_subdiv)
        .add_property("conic_line_subdivision", &Writer::get_conic_line_subdiv, &Writer::set_conic_line_subdiv)
        .add_property("cubic_biarc_subdivision", &Writer::get_cubic_biarc_subdiv, &Writer::set_cubic_biarc_subdiv)
        .add_property("cubic_line_subdivision", &Writer::get_cubic_line_subdiv, &Writer::set_cubic_line_subdiv)
        .add_property("extents", &Writer::get_extents)
        .def("setFont", &Writer::set_font_number)
    ;
    bp::class_< NGC_Writer, bp::bases<Writer> >("NGC_Writer")
        .add_property("blockdelete", &NGC_Writer::get_blockdelete, &NGC_Writer::set_blockdelete)
    ;
    bp::class_< SEG_Writer, bp::bases<Writer> >("SEG_Writer")
        .def( "get_segments", &SEG_Writer::get_segments)
    ;
    */
}
