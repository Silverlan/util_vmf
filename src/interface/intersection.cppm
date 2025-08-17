// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <mathutil/uvec.h>

export module source_engine.vmf:intersection;

export namespace source_engine::vmf {
	enum class IntersectResult : uint8_t { Outside = 0, Inside, Overlap };

	struct AABB;
	namespace Intersect {
		bool AABBInAABB(const glm::vec3 &minA, const glm::vec3 &maxA, const glm::vec3 &minB, const glm::vec3 &maxB);
		bool VectorInBounds(glm::vec3 *vec, glm::vec3 *min, glm::vec3 *max, float EPSILON = 0.f);
		IntersectResult AABBAABB(const glm::vec3 &minA, const glm::vec3 &maxA, const glm::vec3 &minB, const glm::vec3 &maxB);
		bool AABBAABB(AABB *a, AABB *b);
		bool AABBTriangle(glm::vec3 min, glm::vec3 max, glm::vec3 a, glm::vec3 b, glm::vec3 c);
		bool LineAABB(glm::vec3 &o, glm::vec3 &d, glm::vec3 &min, glm::vec3 &max, float *tMinRes, float *tMaxRes = NULL);
		bool LinePlane(glm::vec3 &o, glm::vec3 &d, glm::vec3 &nPlane, float distPlane, float *t = NULL);
		bool SphereSphere(glm::vec3 &originA, float rA, glm::vec3 &originB, float rB);
		bool AABBSphere(glm::vec3 &min, glm::vec3 &max, glm::vec3 &origin, float r);
	};

	namespace Geometry {
		void ClosestPointOnAABBToPoint(glm::vec3 &min, glm::vec3 &max, glm::vec3 &point, glm::vec3 &res);
		void ClosestPointOnPlaneToPoint(glm::vec3 &n, float d, glm::vec3 &p, glm::vec3 &res);
		void ClosestPointOnTriangleToPoint(glm::vec3 &a, glm::vec3 &b, glm::vec3 &c, glm::vec3 &p, glm::vec3 &res);
		float ClosestPointsBetweenLines(glm::vec3 &pA, glm::vec3 &qA, glm::vec3 &pB, glm::vec3 &qB, float &s, float &t, glm::vec3 &cA, glm::vec3 &cB);
	};

	struct AABB {
	  public:
		AABB(glm::vec3 ppos, glm::vec3 pextents) : pos(ppos), extents(pextents) {}
		glm::vec3 pos;
		glm::vec3 extents;
		bool Intersect(AABB *b) { return Intersect::AABBAABB(this, b); }
		float min(int i) { return pos[i] - extents[i]; }
		float max(int i) { return pos[i] + extents[i]; }
		static void GetRotatedBounds(glm::vec3 &min, glm::vec3 &max, glm::mat4 rot, glm::vec3 *rmin, glm::vec3 *rmax)
		{
			rot = glm::inverse(rot);
			uvec::zero(rmin);
			uvec::zero(rmax);
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					float e = rot[i][j] * min[j];
					float f = rot[i][j] * max[j];
					if(e < f) {
						(*rmin)[i] += e;
						(*rmax)[i] += f;
					}
					else {
						(*rmin)[i] += f;
						(*rmax)[i] += e;
					}
				}
			}
		}
	};

	namespace Sweep {
		bool AABBWithAABB(glm::vec3 aa, glm::vec3 &ab, glm::vec3 &extA, glm::vec3 ba, glm::vec3 &bb, glm::vec3 &extB, float *entryTime, float *exitTime, glm::vec3 *normal);
		bool AABBWithPlane(glm::vec3 &origin, glm::vec3 &dir, glm::vec3 &ext, glm::vec3 &planeNormal, float planeDistance, float *t);
	};
};
