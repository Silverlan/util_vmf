/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vmf_side.hpp"
#include "vmf_intersection.hpp"

const double EPSILON = 4.9406564584125e-2;
vmf::Side::Side(std::vector<glm::vec3> *vertexList, std::vector<glm::vec3> *verts, std::vector<glm::vec2> *uvs, std::vector<glm::vec3> *normals, std::string &material) : m_normal(0, 0, 0), m_distance(0)
{
	m_vertexList = *vertexList;
	m_vertices = *verts;
	m_uvs = *uvs;
	m_normals = *normals; // TODO: Remove these?
	m_material = material;

	if(m_vertexList.size() > 2) {
		m_normal = glm::cross(m_vertexList[1] - m_vertexList[0], m_vertexList[2] - m_vertexList[0]);
		uvec::normalize(&m_normal);
		m_distance = -glm::dot(m_normal, m_vertexList[0]);
	}

	CalculateBounds();
}

bool vmf::Side::IsPointInside(glm::vec3 &p, double epsilon)
{
	if(!Intersect::VectorInBounds(&p, &m_min, &m_max, static_cast<float>(EPSILON)))
		return false;
	auto numVerts = m_vertexList.size();
	if(numVerts < 3)
		return false;
	glm::vec3 l = m_vertexList[numVerts - 1];
	glm::vec3 f = m_vertexList[0];
	glm::vec3 q = glm::cross(l - p, f - p);
	for(size_t i = 0; i < numVerts; i++) {
		glm::vec3 v = m_vertexList[i];
		glm::vec3 vNext = m_vertexList[(i + 1) % numVerts];
		glm::vec3 w = glm::cross(vNext - v, p - v);
		if(glm::dot(q, w) < -epsilon)
			return false;
	}
	return true;
}

glm::vec3 *vmf::Side::GetNormal() { return &m_normal; }
float vmf::Side::GetDistance() { return m_distance; }

void vmf::Side::CalculateBounds()
{
	uvec::zero(&m_min);
	uvec::zero(&m_max);
	for(int i = 0; i < m_vertices.size(); i++) {
		if(i == 0) {
			m_min = m_vertices[i];
			m_max = m_vertices[i];
		}
		else {
			uvec::min(&m_min, m_vertices[i]);
			uvec::max(&m_max, m_vertices[i]);
		}
	}
}

void vmf::Side::GetBounds(glm::vec3 *min, glm::vec3 *max) const
{
	*min = m_min;
	*max = m_max;
}

vmf::Side::~Side() {}

unsigned int vmf::Side::GetVertexCount() { return static_cast<unsigned int>(m_vertexList.size()); }

unsigned int vmf::Side::GetTriangleVertexCount() { return static_cast<unsigned int>(m_vertices.size()); }

std::string &vmf::Side::GetMaterial() { return m_material; }

void vmf::Side::GetUVCoordinates(std::vector<glm::vec2> **uvs) { *uvs = &m_uvs; }
void vmf::Side::GetVertices(std::vector<glm::vec3> **vertices) { *vertices = &m_vertexList; }
void vmf::Side::GetTriangles(std::vector<glm::vec3> **vertices) { *vertices = &m_vertices; }

std::ostream &operator<<(std::ostream &os, const vmf::Side side)
{
	glm::vec3 min, max;
	side.GetBounds(&min, &max);
	os << "Side[" << min << "][" << max << "]";
	return os;
}
