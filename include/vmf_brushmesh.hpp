/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UTIL_BRUSHMESH_HPP__
#define __UTIL_BRUSHMESH_HPP__

#include <vector>
#include <mathutil/uvec.h>
#include "vmf_side.hpp"

namespace vmf
{
	class BrushMesh
	{
		friend std::ostream& operator<<(std::ostream&,const BrushMesh&);
	public:
		BrushMesh();
		~BrushMesh();
	private:
		std::vector<Side*> m_sides;
		glm::vec3 m_min;
		glm::vec3 m_max;
	public:
		virtual void AddSide(Side *side);
		void GetSides(std::vector<Side*> **sides);
		bool IntersectAABB(glm::vec3 *min,glm::vec3 *max);
		bool IntersectAABB(glm::vec3 &pos,glm::vec3 &posNew,glm::vec3 &extents,glm::vec3 &posObj,float *entryTime,float *exitTime,glm::vec3 *hitnormal=NULL);
		void Calculate();
		void GetBounds(glm::vec3 *min,glm::vec3 *max) const;
		bool PointInside(glm::vec3 &p,double epsilon=0);
	};
};

#endif
