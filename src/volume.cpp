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

#include "volume.hpp"

#define TOLERANCE (1e-2)

namespace cutsim
{

	//************* Sphere **************/

	/// sphere at center
	SphereVolume::SphereVolume()
	{
		center = GLVertex(0, 0, 0);
		radius = 1.0;
		calcBB();
	}

	float SphereVolume::dist(const GLVertex &p) const
	{
		float d = (center - p).norm();
		return radius - d; // positive inside. negative outside.
	}

	/// set the bounding box values
	void SphereVolume::calcBB()
	{
		bb.clear();
		GLVertex maxpt = GLVertex(center.x + radius, center.y + radius, center.z + radius);
		GLVertex minpt = GLVertex(center.x - radius, center.y - radius, center.z - radius);
		bb.addPoint(maxpt);
		bb.addPoint(minpt);
	}

	//************* Cube **************/

	/// cube at center with side length side
	CubeVolume::CubeVolume()
	{
		center = GLVertex(0, 0, 0);
		side = 1.234;
	}

	/// set bbox values
	void CubeVolume::calcBB()
	{
		bb.clear();
		GLVertex maxpt = GLVertex(center.x + side / 2, center.y + side / 2, center.z + side / 2);
		GLVertex minpt = GLVertex(center.x - side / 2, center.y - side / 2, center.z - side / 2);
		bb.addPoint(maxpt);
		bb.addPoint(minpt);
	}

	float CubeVolume::dist(const GLVertex &p) const
	{
		float m;
		m = p.x - center.x;

		if (fabs(m) < fabs(p.y - center.y))
			m = p.y - center.y;
		if (fabs(m) < fabs(p.z - center.z))
			m = p.z - center.z;
		// m is now the maximum coordinate
		//bool sign = (invert ? -1.0 : 1.0 );
		return (side / 2.0 - fabs(m));
		// positive inside. negative outside.
	}

	//************* Cone **************/

	float ConeVolume::dist(const GLVertex &p) const
	{
		float h = p.z - center.z;

		if (h <= 0) // p is below cone
			return -1;
		else
		{
			float radius = h * tan(alfa);
			float dxy = sqrt((p.x - center.x) * (p.x - center.x) + (p.y - center.y) * (p.y - center.y));
			return radius - dxy;
		}
		//    return 0;
	}

	void ConeVolume::calcBB()
	{
		bb.clear();
		GLVertex maxpt = GLVertex(center.x + height * tan(alfa), center.y + height * tan(alfa), center.z + height);
		GLVertex minpt = GLVertex(center.x - height * tan(alfa), center.y - height * tan(alfa), center.z);
		bb.addPoint(maxpt);
		bb.addPoint(minpt);
	}

	//************* STL **************/

	MeshVolume::MeshVolume()
	{
		center = GLVertex(0, 0, 0); // center is treated as the origin's offset of STL
		rotationCenter = GLVertex(0, 0, 0);
		angle = GLVertex(0, 0, 0);
	}

	void MeshVolume::calcBB()
	{
		GLVertex maxpt;
		GLVertex minpt;
		GLVertex centreDiff = meshCenter - center;
		// std::cout << "MeshVolume::calcBB() Center x:" << center.x << " y: " << center.y << " z: " << center.z << std::endl;
		for (int i = 0; i < (int)facets.size(); i++)
		{
			facets[i]->v1 += centreDiff;
			facets[i]->v2 += centreDiff;
			facets[i]->v3 += centreDiff;
			facets[i]->normal = facets[i]->normal.rotateAC(angle.x, angle.z);
			GLVertex v1p = facets[i]->v1 - rotationCenter;
			facets[i]->v1 = v1p.rotateAC(angle.x, angle.z) + rotationCenter;
			GLVertex v2p = facets[i]->v2 - rotationCenter;
			facets[i]->v2 = v2p.rotateAC(angle.x, angle.z) + rotationCenter;
			GLVertex v3p = facets[i]->v3 - rotationCenter;
			facets[i]->v3 = v3p.rotateAC(angle.x, angle.z) + rotationCenter;
		}
		if (facets.size())
		{
			maxpt.x = fmax(fmax(facets[0]->v1.x, facets[0]->v2.x), facets[0]->v3.x);
			maxpt.y = fmax(fmax(facets[0]->v1.y, facets[0]->v2.y), facets[0]->v3.y);
			maxpt.z = fmax(fmax(facets[0]->v1.z, facets[0]->v2.z), facets[0]->v3.z);
			minpt.x = fmin(fmin(facets[0]->v1.x, facets[0]->v2.x), facets[0]->v3.x);
			minpt.y = fmin(fmin(facets[0]->v1.y, facets[0]->v2.y), facets[0]->v3.y);
			minpt.z = fmin(fmin(facets[0]->v1.z, facets[0]->v2.z), facets[0]->v3.z);
		}
		for (int i = 0; i < (int)facets.size(); i++)
		{
			maxpt.x = fmax(fmax(fmax(facets[i]->v1.x, facets[i]->v2.x), facets[i]->v3.x), maxpt.x);
			maxpt.y = fmax(fmax(fmax(facets[i]->v1.y, facets[i]->v2.y), facets[i]->v3.y), maxpt.y);
			maxpt.z = fmax(fmax(fmax(facets[i]->v1.z, facets[i]->v2.z), facets[i]->v3.z), maxpt.z);
			minpt.x = fmin(fmin(fmin(facets[i]->v1.x, facets[i]->v2.x), facets[i]->v3.x), minpt.x);
			minpt.y = fmin(fmin(fmin(facets[i]->v1.y, facets[i]->v2.y), facets[i]->v3.y), minpt.y);
			minpt.z = fmin(fmin(fmin(facets[i]->v1.z, facets[i]->v2.z), facets[i]->v3.z), minpt.z);
			V21.push_back(facets[i]->v2 - facets[i]->v1);
			invV21dotV21.push_back(1.0 / (facets[i]->v2 - facets[i]->v1).dot(facets[i]->v2 - facets[i]->v1));
			V32.push_back(facets[i]->v3 - facets[i]->v2);
			invV32dotV32.push_back(1.0 / (facets[i]->v3 - facets[i]->v2).dot(facets[i]->v3 - facets[i]->v2));
			V13.push_back(facets[i]->v1 - facets[i]->v3);
			invV13dotV13.push_back(1.0 / (facets[i]->v1 - facets[i]->v3).dot(facets[i]->v1 - facets[i]->v3));
		}
		bb.clear();
		maxpt += GLVertex(TOLERANCE, TOLERANCE, TOLERANCE);
		minpt -= GLVertex(TOLERANCE, TOLERANCE, TOLERANCE);
		// std::cout << "STL maxpt x:" << maxpt.x << " y: " << maxpt.y << " z:" << maxpt.z  << "\n";
		// std::cout << "STL minpt x:" << minpt.x << " y: " << minpt.y << " z:" << minpt.z  << "\n";
		center = meshCenter;
		bb.addPoint(maxpt);
		bb.addPoint(minpt);
	}

	float MeshVolume::dist(const GLVertex &p) const
	{
		GLVertex q, r;
		GLVertex n1, n2, n3;
		double s12, s23, s31;
		float min = 1.0e+3, d, ret = -1.0, u, abs_d;
		for (int i = 0; i < (int)facets.size(); i++)
		{
			u = (p - facets[i]->v1).dot(V21[i]) * invV21dotV21[i];
			q = facets[i]->v1 + V21[i] * u;
			d = (q - p).dot(facets[i]->normal);
			if ((abs_d = fabs(d)) > min)
				continue;
			r = p + facets[i]->normal * d;
			n1 = (r - facets[i]->v1).cross(V13[i]);
			n2 = (r - facets[i]->v2).cross(V21[i]);
			n3 = (r - facets[i]->v3).cross(V32[i]);
			s12 = n1.dot(n2);
			s23 = n2.dot(n3);
			s31 = n3.dot(n1);

			if ((s12 * s31 > 0.0) && (s12 * s23 > 0.0) && (s23 * s31 > 0.0))
				if (abs_d < min)
				{
					min = abs_d;
					ret = d;
					continue;
				}

			if (s12 <= 0.0 && s31 >= 0.0)
			{
				if (u > 0.0 && u < 1.0)
					/*q21 = q*/;
				else if (u <= 0.0)
					q = facets[i]->v1;
				else
					q = facets[i]->v2;
				abs_d = (q - p).norm();
				if (abs_d < min)
				{
					d = (q - p).dot(facets[i]->normal);
					if (d > 0.0 + TOLERANCE)
					{
						min = abs_d + TOLERANCE;
						ret = abs_d;
					}
					else
					{
						min = abs_d;
						ret = -abs_d;
					}
				}
			}
			else if (s31 <= 0.0 && s23 >= 0.0)
			{
				u = (p - facets[i]->v3).dot(V13[i]) * invV13dotV13[i];
				if (u > 0.0 && u < 1.0)
					q = facets[i]->v3 + V13[i] * u;
				else if (u <= 0.0)
					q = facets[i]->v3;
				else
					q = facets[i]->v1;
				abs_d = (q - p).norm();
				if (abs_d < min)
				{
					d = (q - p).dot(facets[i]->normal);
					if (d > 0.0 + TOLERANCE)
					{
						min = abs_d + TOLERANCE;
						ret = abs_d;
					}
					else
					{
						min = abs_d;
						ret = -abs_d;
					}
				}
			}
			else if (s23 <= 0.0 && s12 >= 0.0)
			{
				u = (p - facets[i]->v2).dot(V32[i]) * invV32dotV32[i];
				if (u > 0.0 && u < 1.0)
					q = facets[i]->v2 + V32[i] * u;
				else if (u <= 0.0)
					q = facets[i]->v2;
				else
					q = facets[i]->v3;
				abs_d = (q - p).norm();
				if (abs_d < min)
				{
					d = (q - p).dot(facets[i]->normal);
					if (d > 0.0 + TOLERANCE)
					{
						min = abs_d + TOLERANCE;
						ret = abs_d;
					}
					else
					{
						min = abs_d;
						ret = -abs_d;
					}
				}
			}
		}
		//std::cout << " STL dist " << ret << std::endl;
		return ret; // positive inside. negative outside.
	}

	bool MeshVolume::loadMesh(boost::python::list pyfacets)
	{

		FileIO mesh;
		facets.clear();
		bool processed = false;
		processed = mesh.loadMesh(pyfacets);

		if (processed)
		{
			facets = mesh.getFacets();
			// calculate the bounding box of the mesh volume
			calcBB();
		}

		return processed;
	}

	bool MeshVolume::loadStl(boost::python::str fPath)
	{

		FileIO stl;
		facets.clear();
		bool processed = false;
		processed = stl.loadStl(fPath);

		if (processed)
		{
			facets = stl.getFacets();
			// calculate the bounding box of the mesh volume
			calcBB();
		}

		return processed;
	}

} // end namespace
// end of file volume.cpp
