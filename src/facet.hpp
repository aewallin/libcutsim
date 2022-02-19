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

#ifndef FACET_H
#define FACET_H

#include <cassert>

#include "glvertex.hpp"

namespace cutsim
{

	class Facet
	{

	public:
		Facet(GLVertex n, GLVertex p1, GLVertex p2, GLVertex p3)
		{
			normal = n;
			v1 = p1;
			v2 = p2;
			v3 = p3;
		}
		GLVertex normal;
		GLVertex v1, v2, v3;
	};

}

#endif // FACET_H
