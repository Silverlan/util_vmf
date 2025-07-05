// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <memory>
#include <mathutil/uvec.h>

export module source_engine.vmf:polyinfo;

export namespace source_engine::vmf
{
	class PolyDispInfo {
	  public:
		PolyDispInfo();
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned int> triangles;
		std::vector<glm::vec2> alphas;
		std::vector<glm::vec3> faceNormals;
		glm::vec3 center;
		unsigned char numAlpha;
		unsigned int power;
		std::vector<unsigned int> powersMerged;
	};

	class PolyInfo {
	  public:
		PolyInfo();
		glm::vec3 nu;
		glm::vec3 nv;
		float width;
		float height;
		std::unique_ptr<PolyDispInfo> displacement;
	};

	class Poly;
	class PolyMeshInfo {
	  public:
		std::vector<glm::vec3> vertexList;
	};
};
