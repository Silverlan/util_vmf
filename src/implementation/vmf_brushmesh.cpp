// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <vector>

#include <iostream>

module source_engine.vmf;

import :brushmesh;
import :intersection;

std::ostream &source_engine::vmf::operator<<(std::ostream &os, const source_engine::vmf::BrushMesh &mesh)
{
	glm::vec3 min, max;
	mesh.GetBounds(&min, &max);
	os << "BrushMesh[" << mesh.m_sides.size() << "][" << min.x << "," << min.y << "," << min.z << "][" << max.x << "," << max.y << "," << max.z << "]";
	return os;
}

source_engine::vmf::BrushMesh::BrushMesh()
{
	uvec::zero(&m_min);
	uvec::zero(&m_max);
}

source_engine::vmf::BrushMesh::~BrushMesh()
{
	for(int i = 0; i < m_sides.size(); i++)
		delete m_sides[i];
}

void source_engine::vmf::BrushMesh::AddSide(Side *side) { m_sides.push_back(side); }

void source_engine::vmf::BrushMesh::GetSides(std::vector<Side *> **sides) { *sides = &m_sides; }

bool source_engine::vmf::BrushMesh::IntersectAABB(glm::vec3 &pos, glm::vec3 &posNew, glm::vec3 &extents, glm::vec3 &posObj, float *entryTime, float *exitTime, glm::vec3 *hitnormal)
{
	glm::vec3 extentsThis = (m_max - m_min) * 0.5f;
	glm::vec3 posThis = posObj + m_min + extentsThis;
	return source_engine::vmf::Sweep::AABBWithAABB(pos, posNew, extents, posThis, posThis, extentsThis, entryTime, exitTime, hitnormal);
}

bool source_engine::vmf::BrushMesh::IntersectAABB(glm::vec3 *min, glm::vec3 *max)
{
	if(source_engine::vmf::Intersect::AABBAABB(m_min, m_max, *min, *max) == IntersectResult::Outside)
		return false;
	return true;
}

void source_engine::vmf::BrushMesh::Calculate()
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

void source_engine::vmf::BrushMesh::GetBounds(glm::vec3 *min, glm::vec3 *max) const
{
	*min = m_min;
	*max = m_max;
}

bool source_engine::vmf::BrushMesh::PointInside(glm::vec3 &p, double epsilon)
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
