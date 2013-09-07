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
#include <sstream>

#include <list>
#include <set>
#include <vector>

#include "volume.hpp"
#include "bbox.hpp"
#include "glvertex.hpp"
#include "gldata.hpp"

namespace cutsim {

/// \class Octnode
/// Octnode represents a node in the octree.
///
/// each node in the octree is a cube with side length scale
/// the distance field at each corner vertex is stored.
class Octnode {
    public:
        /// node state, one of inside, outside, or undecided
        enum NodeState  {INSIDE, OUTSIDE, UNDECIDED };
        NodeState state;         ///< the current state of this node
        NodeState prev_state;    ///< previous state of this node
        Color color;         /// the color of this node

        /// create suboctant idx of parent with scale nodescale and depth nodedepth
        Octnode(Octnode* parent, unsigned int idx, float nodescale, unsigned int nodedepth, GLData* g);
        virtual ~Octnode();
        /// create all eight children of this node
        void subdivide(); 
        /// for subdivision even though state is not undecided. called/used from Octree::init()
        void force_subdivide() { // this is only called from octree-init..
            setUndecided();
            subdivide();
        }
    // BOOLEAN OPS
        void sum(const Volume* vol);         ///< sum Volume to this Octnode
        void diff(const Volume* vol);        ///< diff Volume from this Octnode
        void intersect(const Volume* vol);   ///< intersect this Octnode with given Volume
        
        bool is_inside()    { return (state==INSIDE); }
        bool is_outside()   { return (state==OUTSIDE); }
        bool is_undecided() { return (state==UNDECIDED); }
        
        /// return true if all children of this node in given state s
        bool all_child_state(NodeState s) const;
        /// delete all children of this node
        void delete_children();

    // manipulate the valid-flag
        /// set valid-flag true
        void setValid();
        /// set valid-flag false
        void setInvalid();
        /// true if the GLData for this node is valid
        bool valid() const;
        
        /// true if this node has child n
        inline bool hasChild(int n) { return (this->child[n] != NULL); }
        /// true if this node has no children
        inline bool isLeaf() {return (childcount==0);}
    // DATA
        /// pointers to child nodes
        Octnode* child[8];
        /// pointer to parent node
        Octnode* parent;
        /// number of children
        unsigned int childcount;
        /// The eight corners of this node
        GLVertex* vertex[8];
        /// value of distance-field at corner vertex
        float f[8]; 
        /// the center point of this node
        GLVertex* center; // the centerpoint of this node
        /// the tree-dept of this node
        unsigned int depth; // depth of node
        /// the index of this node [0,7]
        unsigned int idx; // index of node
        /// the scale of this node, i.e. distance from center out to corner vertices
        float scale; // distance from center to vertices
        /// bounding-box corresponding to this node
        Bbox bb;
    
    // for manipulating vertexSet
        /// add id to the vertex set
        void addIndex(unsigned int id);
        /// swap the id for an existing oldId to the given newId. 
        /// This is called from GLData when GLData needs to move around vertices
        void swapIndex(unsigned int oldId, unsigned int newId);
        /// remove given id from vertex set
        void removeIndex(unsigned int id);
        /// is the vertex set empty?
        bool vertexSetEmpty() {return vertexSet.empty(); }
        /// return begin() for vertex set
        unsigned int vertexSetTop() { return *(vertexSet.begin()); }
        /// remove all vertices associated with this node. calls GLData to also remove nodes
        void clearVertexSet();

        // string output
        friend std::ostream& operator<<(std::ostream &stream, const Octnode &o);
        std::string str() const;
        std::string printF();
        std::string spaces() const; // "tabs" for aligned printing
        std::string type() const;
        
        /// set node to undecided
        void setUndecided();
    protected: 
        /// based on the f[]-values at the corners of this node, set the state to one of inside, outside, or undecided.
        void set_state();
        /// set node to inside
        void setInside();
        /// set node to outside
        void setOutside();

        /// set given child valid
        void setChildValid( unsigned int id );
        /// set the given child to invalid
        inline void setChildInvalid( unsigned int id );

        /// the vertex indices that this node has produced. 
        /// These correspond to vertex id's in the GLData vertexArray.
        std::set<unsigned int> vertexSet;
        /// return center of child with index n
        GLVertex* childcenter(int n); // return position of child center
        /// The GLData, i.e. vertices and polygons, associated with this node
        /// when this node is deleted we notify the GLData that vertices should be removed
        GLData* g;
        /// flag for telling isosurface extraction is valid for this node
        /// if false, the node needs updating.
        bool isosurface_valid;
        /// bit-field indicating if children have valid gldata
        char childStatus; 
        
// STATIC
        /// the direction to the vertices, from the center 
        static const GLVertex direction[8];
        /// bit masks for the status
        static const int octant[8];         
    private:
        Octnode(){}
};

} // end namespace

// end file octnode.hpp
