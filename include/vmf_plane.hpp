/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __PLANE_HPP__
#define __PLANE_HPP__

#include <mathutil/uvec.h>
#include <mathutil/eulerangles.h>

namespace vmf
{
	class Plane
	{
	public:
		Plane(glm::vec3 *a,glm::vec3 *b,glm::vec3 *c);
		Plane(glm::vec3 n,glm::vec3 *pos);
		Plane(glm::vec3 n,double d);
	private:
		glm::vec3 m_normal;
		glm::vec3 m_pos;
		double m_distance;
		void MoveToPos(glm::vec3 *pos);
		void Rotate(EulerAngles &ang);
	public:
		static bool GetPlaneIntersection(glm::vec3 *intersect,glm::vec3 *na,glm::vec3 *nb,glm::vec3 *nc,double da,double db,double dc);
		glm::vec3 *GetNormal();
	};
};

#endif
