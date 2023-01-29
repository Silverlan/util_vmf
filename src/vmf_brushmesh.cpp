/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vmf_brushmesh.hpp"
#include "vmf_intersection.hpp"

std::ostream &vmf::operator<<(std::ostream &os, const vmf::BrushMesh &mesh)
{
	glm::vec3 min, max;
	mesh.GetBounds(&min, &max);
	os << "BrushMesh[" << mesh.m_sides.size() << "][" << min.x << "," << min.y << "," << min.z << "][" << max.x << "," << max.y << "," << max.z << "]";
	return os;
}

vmf::BrushMesh::BrushMesh()
{
	uvec::zero(&m_min);
	uvec::zero(&m_max);
}

vmf::BrushMesh::~BrushMesh()
{
	for(int i = 0; i < m_sides.size(); i++)
		delete m_sides[i];
}

void vmf::BrushMesh::AddSide(Side *side) { m_sides.push_back(side); }

void vmf::BrushMesh::GetSides(std::vector<Side *> **sides) { *sides = &m_sides; }

bool vmf::BrushMesh::IntersectAABB(glm::vec3 &pos, glm::vec3 &posNew, glm::vec3 &extents, glm::vec3 &posObj, float *entryTime, float *exitTime, glm::vec3 *hitnormal)
{
	glm::vec3 extentsThis = (m_max - m_min) * 0.5f;
	glm::vec3 posThis = posObj + m_min + extentsThis;
	return vmf::Sweep::AABBWithAABB(pos, posNew, extents, posThis, posThis, extentsThis, entryTime, exitTime, hitnormal);
}

bool vmf::BrushMesh::IntersectAABB(glm::vec3 *min, glm::vec3 *max)
{
	if(vmf::Intersect::AABBAABB(m_min, m_max, *min, *max) == IntersectResult::Outside)
		return false;
	return true;
}

void vmf::BrushMesh::Calculate()
{
	uvec::zero(&m_min);
	uvec::zero(&m_max);
	for(int i = 0; i < m_sides.size(); i++) {
		glm::vec3 min, max;
		m_sides[i]->GetBounds(&min, &max);
		if(i == 0) {
			m_min = min;
			m_max = max;
		}
		else {
			uvec::min(&m_min, min);
			uvec::max(&m_max, max);
		}
	}
}

void vmf::BrushMesh::GetBounds(glm::vec3 *min, glm::vec3 *max) const
{
	*min = m_min;
	*max = m_max;
}

bool vmf::BrushMesh::PointInside(glm::vec3 &p, double epsilon)
{
	for(int i = 0; i < m_sides.size(); i++) {
		Side *side = m_sides[i];
		glm::vec3 n = *side->GetNormal();
		glm::vec3 of = p - (n * -side->GetDistance());
		float d = glm::dot(of, n);
		if(d >= epsilon)
			return false;
	}
	return true;
}
