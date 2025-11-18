// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module source_engine.vmf;

import :plane;

source_engine::vmf::Plane::Plane(glm::vec3 *a, glm::vec3 *b, glm::vec3 *c)
{
	glm::vec3 n = glm::cross(*b - *a, *c - *a);
	uvec::normalize(&n);
	double d = glm::dot(-n, *a);
	Plane(n, d);
}

source_engine::vmf::Plane::Plane(glm::vec3 n, glm::vec3 *pos)
{
	m_normal = n;
	MoveToPos(pos);
}

source_engine::vmf::Plane::Plane(glm::vec3 n, double d)
{
	m_normal = n;
	m_distance = d;
	m_pos.x = n.x * -static_cast<float>(d);
	m_pos.y = n.y * -static_cast<float>(d);
	m_pos.z = n.z * -static_cast<float>(d);
}

glm::vec3 *source_engine::vmf::Plane::GetNormal() { return &m_normal; }

void source_engine::vmf::Plane::MoveToPos(glm::vec3 *pos)
{
	glm::vec3 n = *GetNormal();
	double dNew = glm::dot(n, *pos);
	m_distance = -dNew;
	m_pos = n * float(dNew);
}

void source_engine::vmf::Plane::Rotate(EulerAngles &ang)
{
	glm::vec3 n = *GetNormal();
	uvec::rotate(&n, EulerAngles(ang));
	m_normal = n;
}

bool source_engine::vmf::Plane::GetPlaneIntersection(glm::vec3 *intersect, glm::vec3 *na, glm::vec3 *nb, glm::vec3 *nc, double da, double db, double dc)
{
	float denom = glm::dot(*na, glm::cross(*nb, *nc));
	if(denom == 0)
		return false;
	glm::vec3 crossbc = glm::cross(*nb, *nc);
	glm::vec3 crossca = glm::cross(*nc, *na);
	glm::vec3 crossab = glm::cross(*na, *nb);
	uvec::mul(&crossbc, -da);
	uvec::mul(&crossca, -db);
	uvec::mul(&crossab, -dc);
	*intersect = crossbc + crossca + crossab;
	uvec::div(intersect, denom);
	return true;
}
