// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <vector>
#include <mathutil/uvec.h>

export module source_engine.vmf:side;

export namespace source_engine::vmf
{
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

export std::ostream &operator<<(std::ostream &os, const source_engine::vmf::Side &side);
