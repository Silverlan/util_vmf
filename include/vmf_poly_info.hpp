/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __VMF_POLY_INFO_HPP__
#define __VMF_POLY_INFO_HPP__

#include <memory>
#include <mathutil/uvec.h>

namespace vmf {
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

#endif
