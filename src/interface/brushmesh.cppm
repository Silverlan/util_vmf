// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <vector>
#include <mathutil/uvec.h>

export module source_engine.vmf:brushmesh;

import :side;

export namespace source_engine::vmf
{
	class BrushMesh;
	std::ostream &operator<<(std::ostream &os, const vmf::BrushMesh &mesh);
	class BrushMesh {
		friend std::ostream &operator<<(std::ostream &, const BrushMesh &);
	  public:
		BrushMesh();
		~BrushMesh();
	  private:
		std::vector<Side *> m_sides;
		glm::vec3 m_min;
		glm::vec3 m_max;
	  public:
		virtual void AddSide(Side *side);
		void GetSides(std::vector<Side *> **sides);
		bool IntersectAABB(glm::vec3 *min, glm::vec3 *max);
		bool IntersectAABB(glm::vec3 &pos, glm::vec3 &posNew, glm::vec3 &extents, glm::vec3 &posObj, float *entryTime, float *exitTime, glm::vec3 *hitnormal = NULL);
		void Calculate();
		void GetBounds(glm::vec3 *min, glm::vec3 *max) const;
		bool PointInside(glm::vec3 &p, double epsilon = 0);
	};
};
