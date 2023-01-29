/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __VMF_SIDE_HPP__
#define __VMF_SIDE_HPP__

#include <vector>
#include <mathutil/uvec.h>

namespace vmf {
	class Side {
	  public:
		Side(std::vector<glm::vec3> *vertexList, std::vector<glm::vec3> *verts, std::vector<glm::vec2> *uvs, std::vector<glm::vec3> *normals, std::string &material);
		~Side();
	  protected:
		std::vector<glm::vec3> m_vertices; // Vertices as triangles
		std::vector<glm::vec2> m_uvs;
		std::vector<glm::vec3> m_normals;
		std::vector<glm::vec3> m_vertexList;
		std::string m_material;
		glm::vec3 m_min;
		glm::vec3 m_max;
		glm::vec3 m_normal;
		float m_distance;
	  public:
		std::string &GetMaterial();
		unsigned int GetVertexCount();
		unsigned int GetTriangleVertexCount();
		void CalculateBounds();
		void GetBounds(glm::vec3 *min, glm::vec3 *max) const;
		void GetUVCoordinates(std::vector<glm::vec2> **uvs);
		void GetVertices(std::vector<glm::vec3> **vertices);
		void GetTriangles(std::vector<glm::vec3> **vertices);
		bool IsPointInside(glm::vec3 &p, double epsilon = 0);
		glm::vec3 *GetNormal();
		float GetDistance();
	};
};

std::ostream &operator<<(std::ostream &os, const vmf::Side &side);

#endif
