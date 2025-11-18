// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module source_engine.vmf:plane;

export import pragma.math;

export namespace source_engine::vmf {
	class Plane {
	  public:
		Plane(glm::vec3 *a, glm::vec3 *b, glm::vec3 *c);
		Plane(glm::vec3 n, glm::vec3 *pos);
		Plane(glm::vec3 n, double d);
	  private:
		glm::vec3 m_normal;
		glm::vec3 m_pos;
		double m_distance;
		void MoveToPos(glm::vec3 *pos);
		void Rotate(EulerAngles &ang);
	  public:
		static bool GetPlaneIntersection(glm::vec3 *intersect, glm::vec3 *na, glm::vec3 *nb, glm::vec3 *nc, double da, double db, double dc);
		glm::vec3 *GetNormal();
	};
};
