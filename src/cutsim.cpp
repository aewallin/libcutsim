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

#include "cutsim.hpp"

namespace cutsim {

Cutsim::Cutsim (double octree_size, unsigned int octree_max_depth, GLData* gld, IsoSurfaceAlgorithm* iso)
    : iso_algo(iso), g(gld) {
    GLVertex octree_center(0,0,0);
    tree = new Octree(octree_size, octree_max_depth, octree_center, g );
    //std::cout << "Cutsim() ctor: tree before init: " << tree->str() << "\n";
    //tree->init(2u);
    tree->debug=false;
    //std::cout << "Cutsim() ctor: tree after init: " << tree->str() << "\n";
    iso_algo->set_gl(g);
    iso_algo->set_tree(tree);
    iso_algo->set_polyVerts();
    // iso_algo = new MarchingCubes(g, tree);
    //iso_algo = new CubeWireFrame(g, tree);    
} 

Cutsim::~Cutsim() {
    //delete iso_algo;
    delete tree;
    //delete g;
}

void Cutsim::init(unsigned int n) {
    tree->init(n);
    //std::cout << "Cutsim::init() tree after init: " << tree->str() << "\n";
}

std::string Cutsim::str() const {
    std::string out = tree->str();
    return out;
}

void Cutsim::updateGL() {
    std::clock_t start, stop;
    start = std::clock();
    iso_algo->updateGL();
    //g->swap();
    stop = std::clock();
    std::cout << "cutsim.cpp updateGL() : " << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
}

void Cutsim::sum_volume( const Volume* volume ) {
    std::clock_t start, stop;
    start = std::clock();
    tree->sum( volume );
    stop = std::clock();
    std::cout << "Cutsim::sum_volume()  :" << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<' s\n';
}

void Cutsim::diff_volume( const Volume* volume ) {
    std::clock_t start, stop;
    start = std::clock();
    tree->diff( volume );
    stop = std::clock();
    std::cout << "cutsim.cpp diff_volume()  :" << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
}

void Cutsim::intersect_volume( const Volume* volume ) {
    std::clock_t start, stop;
    start = std::clock();
    tree->intersect( volume );
    stop = std::clock();
    std::cout << "cutsim.cpp intersect_volume()  :" << ( ( stop - start ) / (double)CLOCKS_PER_SEC ) <<'\n';
}

} // end namespace
