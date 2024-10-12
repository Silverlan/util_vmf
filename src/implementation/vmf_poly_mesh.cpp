/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

module;

#include <mathutil/uvec.h>

module source_engine.vmf;

import :polymesh;
import :brushmesh;
import :plane;

source_engine::vmf::PolyMesh::PolyMesh() : m_bHasDisplacements(false), m_bValid(true)
{
	m_min.x = 0;
	m_min.y = 0;
	m_min.z = 0;
	m_max.x = 0;
	m_max.y = 0;
	m_max.z = 0;
	m_center.x = 0;
	m_center.y = 0;
	m_center.z = 0;
	m_centerLocalized = nullptr;
	m_centerOfMass.x = 0;
	m_centerOfMass.y = 0;
	m_centerOfMass.z = 0;
}

source_engine::vmf::PolyMesh::~PolyMesh()
{
	for(int i = 0; i < m_polys.size(); i++)
		delete m_polys[i];
	if(m_centerLocalized != nullptr)
		delete m_centerLocalized;
	for(int i = 0; i < m_vertices.size(); i++)
		delete m_vertices[i];
}

void source_engine::vmf::PolyMesh::Validate()
{
	if(m_bValid == false || m_bHasDisplacements == false)
		return;
	auto bHasDisp = m_bHasDisplacements;
	auto bCheck = false;
	for(auto it = m_polys.begin(); it != m_polys.end(); ++it) {
		auto *poly = *it;
		if(poly->IsDisplacement()) {
			bCheck = true;
			break;
		}
	}
	if(bCheck == false)
		m_bValid = false;
}
bool source_engine::vmf::PolyMesh::IsValid() { return m_bValid; }

source_engine::vmf::PolyMeshInfo &source_engine::vmf::PolyMesh::GetCompiledData() { return m_compiledData; }

bool source_engine::vmf::PolyMesh::HasDisplacements() { return m_bHasDisplacements; }
void source_engine::vmf::PolyMesh::SetHasDisplacements(bool b) { m_bHasDisplacements = b; }

void source_engine::vmf::PolyMesh::GenerateBrushMeshes(std::vector<BrushMesh *> *brushMeshes, std::vector<PolyMesh *> meshes)
{
	for(int i = 0; i < meshes.size(); i++) {
		BrushMesh *mesh = new BrushMesh();
		std::vector<Poly *> *polys;
		meshes[i]->GetPolys(&polys);
		for(int j = 0; j < polys->size(); j++) {
			Poly *poly = (*polys)[j];
			std::vector<Vertex> &polyVerts = poly->GetVertices();
			std::vector<glm::vec3> *vertexList = new std::vector<glm::vec3>;
			for(int i = 0; i < polyVerts.size(); i++)
				vertexList->push_back(polyVerts[i].pos);
			std::vector<glm::vec3> *verts = new std::vector<glm::vec3>;
			std::vector<glm::vec2> *uvs = new std::vector<glm::vec2>;
			std::vector<glm::vec3> *normals = new std::vector<glm::vec3>;
			poly->GenerateTriangleMesh(verts, uvs, normals);
			TextureData *texture = poly->GetTextureData();
			Side *side = new Side(vertexList, verts, uvs, normals, texture->texture);
			mesh->AddSide(side);
		}
		mesh->Calculate();
		brushMeshes->push_back(mesh);
	}
}

glm::vec3 *source_engine::vmf::PolyMesh::GetLocalizedCenter() { return m_centerLocalized; }

void source_engine::vmf::PolyMesh::AddPoly(Poly *poly) { m_polys.push_back(poly); }

unsigned int source_engine::vmf::PolyMesh::GetPolyCount() { return static_cast<unsigned int>(m_polys.size()); }

int source_engine::vmf::PolyMesh::BuildPolyMesh()
{
	int numPolys = static_cast<int>(m_polys.size());
	for(int i = 0; i < numPolys - 2; i++) {
		for(int j = i + 1; j < numPolys - 1; j++) {
			for(int k = j + 1; k < numPolys; k++) {
				bool legal = true;
				Poly *a = m_polys[i];
				Poly *b = m_polys[j];
				Poly *c = m_polys[k];

				glm::vec3 intersect;
				auto na = a->GetNormal();
				auto nb = b->GetNormal();
				auto nc = c->GetNormal();
				bool bIntersect = source_engine::vmf::Plane::GetPlaneIntersection(&intersect, &na, &nb, &nc, a->GetDistance(), b->GetDistance(), c->GetDistance());
				if(bIntersect) {
					for(int m = 0; m < numPolys; m++) {
						Poly *poly = m_polys[m];
						float dotProd = -glm::dot(poly->GetNormal(), intersect);
						double d = poly->GetDistance();
						if(dotProd - d > EPSILON) {
							legal = false;
							break;
						}
					}
					if(legal) {
						glm::vec3 na = a->GetNormal();
						uvec::mul(&na, -1);
						a->AddUniqueVertex(intersect, na);

						glm::vec3 nb = b->GetNormal();
						uvec::mul(&nb, -1);
						b->AddUniqueVertex(intersect, nb);

						glm::vec3 nc = c->GetNormal();
						uvec::mul(&nc, -1);
						c->AddUniqueVertex(intersect, nc);
					}
				}
			}
		}
	}
	auto bHasDisplacement = false;
	for(int i = 0; i < numPolys; i++) {
		if(m_polys[i]->IsDisplacement()) {
			bHasDisplacement = true;
			unsigned int numPolys = m_polys[i]->GetVertexCount();
			if(numPolys != 4)
				return -1;
		}
		m_polys[i]->SortVertices();
		if(m_polys[i]->IsDisplacement()) {
			DispInfo *info = m_polys[i]->GetDisplacement();
			std::vector<Vertex> &vertices = m_polys[i]->GetVertices();
			for(unsigned int i = 0; i < vertices.size(); i++) {
				Vertex &v = vertices[i];
				if(fabsf(v.pos.x - info->startposition.x) <= EPSILON && fabsf(v.pos.y - info->startposition.y) <= EPSILON && fabsf(v.pos.z - info->startposition.z) <= EPSILON) {
					info->startpositionId = i;
					break;
				}
			}
			if(info->startpositionId == -1)
				return -2;
		}
	}
	return 0;
}

static unsigned short POLYMESH_ERROR_LEVEL = 0;
void source_engine::vmf::PolyMesh::Calculate()
{
	unsigned int numPolys = GetPolyCount();
	glm::vec3 pos(0, 0, 0);
	uvec::max(&m_min);
	uvec::min(&m_max);
	uvec::zero(&m_centerOfMass);
	for(int i = numPolys - 1; i >= 0; i--) {
		if(!m_polys[i]->IsValid()) {
			if(POLYMESH_ERROR_LEVEL > 0)
				std::cout << "WARNING: Invalid polygon '(" << *m_polys[i] << ")' for mesh '" << *this << "': Less than 3 vertices. Removing..." << std::endl;
			delete m_polys[i];
			m_polys.erase(m_polys.begin() + i);
			numPolys--;
		}
		else {
			uvec::add(&pos, m_polys[i]->GetCenter());
			uvec::add(&m_centerOfMass, *m_polys[i]->GetWorldPosition());
			std::vector<Vertex> &vertices = m_polys[i]->GetVertices();
			for(int j = 0; j < vertices.size(); j++) {
				Vertex &v = vertices[j];
				glm::vec3 vThis;
				//glm::vec3 *vNew = new glm::vec3(v->pos);
				if(HasVertex(&v, &vThis))
					uvec::match(&v.pos, vThis); // Make sure the vertex in the polygon is the same as ours
				else
					m_vertices.push_back(new glm::vec3(v.pos));
				glm::vec3 min, max;
				m_polys[i]->GetBounds(&min, &max);
				uvec::min(&m_min, min);
				uvec::max(&m_max, max);
			}
		}
	}
	m_center = pos;
	m_center /= numPolys;
	m_centerOfMass /= numPolys;
	m_min -= m_centerOfMass;
	m_max -= m_centerOfMass;
}

glm::vec3 *source_engine::vmf::PolyMesh::GetWorldPosition() { return &m_centerOfMass; }

void source_engine::vmf::PolyMesh::debug_print()
{
	std::cout << "Mesh '" << *this << "':" << std::endl;
	for(int i = 0; i < m_vertices.size(); i++)
		std::cout << "\tVertex " << i << ": (" << m_vertices[i]->x << "," << m_vertices[i]->y << "," << m_vertices[i]->z << ")" << std::endl;
	std::cout << "Polys:" << std::endl;
	for(int i = 0; i < m_polys.size(); i++)
		m_polys[i]->debug_print();
}

void source_engine::vmf::PolyMesh::CenterPolys()
{
	glm::vec3 center(0, 0, 0);
	unsigned int numPolys = static_cast<unsigned int>(m_polys.size());
	for(unsigned int i = 0; i < numPolys; i++) {
		glm::vec3 centerPoly = m_polys[i]->GetCenter();
		center += centerPoly;
	}
	center /= numPolys;
	Localize(center);
}

void source_engine::vmf::PolyMesh::Localize(const glm::vec3 &center)
{
	if(m_centerLocalized == NULL)
		m_centerLocalized = new glm::vec3(center);
	else
		uvec::add(m_centerLocalized, center);
	for(int i = 0; i < m_polys.size(); i++)
		m_polys[i]->Localize(center);
	Calculate();
}
void source_engine::vmf::PolyMesh::Localize() { Localize(GetCenter()); }

glm::vec3 source_engine::vmf::PolyMesh::GetCenter() { return m_center; }

bool source_engine::vmf::PolyMesh::HasVertex(Vertex *v, glm::vec3 *vThis)
{
	glm::vec3 pos = v->pos;
	for(int i = 0; i < m_vertices.size(); i++) {
		glm::vec3 *posThis = m_vertices[i];
		if(uvec::cmp(pos, *posThis)) {
			vThis->x = posThis->x;
			vThis->y = posThis->y;
			vThis->z = posThis->z;
			return true;
		}
		else {
			glm::vec3 diff(*posThis);
			uvec::sub(&diff, pos);
			if(diff.x <= EPSILON && diff.x >= -EPSILON && diff.y <= EPSILON && diff.y >= -EPSILON && diff.z <= EPSILON && diff.z >= -EPSILON) {
				if(vThis != NULL) {
					vThis->x = posThis->x;
					vThis->y = posThis->y;
					vThis->z = posThis->z;
				}
				return true;
			}
		}
	}
	return false;
}

void source_engine::vmf::PolyMesh::GetPolys(std::vector<Poly *> **polys) { *polys = &m_polys; }

void source_engine::vmf::PolyMesh::GetBounds(glm::vec3 *min, glm::vec3 *max)
{
	*min = m_min;
	*max = m_max;
}
