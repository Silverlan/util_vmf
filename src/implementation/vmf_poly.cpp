/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

module;

#include <sharedutils/util_string.h>
#include <materialmanager.h>

module source_engine.vmf;

import :poly;

std::ostream &source_engine::vmf::operator<<(std::ostream &os, const source_engine::vmf::Poly &poly)
{
	os << "Poly[" << &poly << "] [" << poly.m_vertices.size() << "] [" << poly.m__normal.x << " " << poly.m__normal.y << " " << poly.m__normal.z << "] [" << poly.m_distance << "]";
	return os;
}

source_engine::vmf::Poly::Poly(const std::function<Material *(const std::string &)> &fLoadMaterial) : m_materialLoader(fLoadMaterial)
{
	m_distance = 0;
	m_normal.x = 0;
	m_normal.y = 0;
	m_normal.z = 0;
	m__normal.x = 0;
	m__normal.y = 0;
	m__normal.z = 0;
	m_min.x = 0;
	m_min.y = 0;
	m_min.z = 0;
	m_max.x = 0;
	m_max.y = 0;
	m_max.z = 0;
	m_centerOfMass.x = 0;
	m_centerOfMass.y = 0;
	m_centerOfMass.z = 0;
	m_texData = {};
}

void source_engine::vmf::Poly::RemoveDisplacement()
{
	if(m_displacement.has_value() == false)
		return;
	m_displacement = {};
	auto *mat = m_materialLoader("tools/toolsnodraw");
	m_material = mat ? mat->GetHandle() : msys::MaterialHandle {};
}

source_engine::vmf::PolyInfo &source_engine::vmf::Poly::GetCompiledData() { return m_compiledData; }

void source_engine::vmf::Poly::Merge(Poly *poly)
{
	/*if(!IsDisplacement() || !poly->IsDisplacement())
		return;
	auto *disp = GetDisplacement();
	auto *disp2 = poly->GetDisplacement();
	*/
}

void source_engine::vmf::Poly::SetDisplacement(const DispInfo &disp) { m_displacement = disp; }

source_engine::vmf::DispInfo *source_engine::vmf::Poly::GetDisplacement() { return m_displacement.has_value() ? &*m_displacement : nullptr; }
bool source_engine::vmf::Poly::IsDisplacement() { return m_displacement.has_value(); }

void source_engine::vmf::Poly::SetTextureData(std::string texture, glm::vec3 nu, glm::vec3 nv, float ou, float ov, float su, float sv, float rot)
{
	ustring::to_lower(texture);
	m_texData = TextureData {};
	m_texData->texture = texture;
	m_texData->nu = nu;
	m_texData->nv = nv;
	m_texData->ou = ou;
	m_texData->ov = ov;
	m_texData->su = su;
	m_texData->sv = sv;
	m_texData->rot = rot;

	auto *mat = m_materialLoader(texture.c_str());
	m_material = mat ? mat->GetHandle() : nullptr;
}

source_engine::vmf::TextureData *source_engine::vmf::Poly::GetTextureData() { return m_texData.has_value() ? &*m_texData : nullptr; }

Material *source_engine::vmf::Poly::GetMaterial() { return m_material.get(); }
void source_engine::vmf::Poly::SetMaterial(Material *material) { m_material = material ? material->GetHandle() : msys::MaterialHandle {}; }

void source_engine::vmf::Poly::SetMaterialId(uint32_t id) { m_materialId = id; }
uint32_t source_engine::vmf::Poly::GetMaterialId() const { return m_materialId; }

void source_engine::vmf::Poly::SetDistance(double d) { m_distance = d; }
void source_engine::vmf::Poly::SetNormal(glm::vec3 n)
{
	m__normal.x = n.x;
	m__normal.y = n.y;
	m__normal.z = n.z;
}

glm::vec3 source_engine::vmf::Poly::GetCalculatedNormal() { return m_normal; }

const short source_engine::vmf::Poly::ClassifyPoint(glm::vec3 *point)
{
	glm::vec3 pos = GetCalculatedNormal() * float(-GetDistance());
	glm::vec3 v = (pos - (*point));
	uvec::normalize(&v);
	float r = glm::dot(v, GetNormal());
	if(r > EPSILON)
		return CLASSIFY_FRONT;
	else if(r < EPSILON)
		return CLASSIFY_BACK;
	return CLASSIFY_ONPLANE;
}

bool source_engine::vmf::Poly::IsValid()
{
	if(IsDisplacement())
		return true;
	return GetVertexCount() >= 3;
}

double source_engine::vmf::Poly::GetDistance() { return m_distance; }
glm::vec3 source_engine::vmf::Poly::GetNormal() { return m__normal; }

bool source_engine::vmf::Poly::HasVertex(glm::vec3 *vert)
{
	for(auto &vertex : m_vertices) {
		glm::vec3 v = vertex.pos;
		glm::vec3 sub(v);
		uvec::sub(&sub, *vert);
		if(sub.x <= EPSILON && sub.x >= -EPSILON && sub.y <= EPSILON && sub.y >= -EPSILON && sub.z <= EPSILON && sub.z >= -EPSILON)
			return true;
	}
	return false;
}

bool source_engine::vmf::Poly::AddUniqueVertex(glm::vec3 vert, glm::vec3 n)
{
	if(HasVertex(&vert))
		return false;
	AddVertex(vert, n);
	return true;
}

void source_engine::vmf::Poly::AddVertex(glm::vec3 vert, glm::vec3 n) { m_vertices.push_back(Vertex(vert, n)); }

unsigned int source_engine::vmf::Poly::GetVertexCount() { return static_cast<unsigned int>(m_vertices.size()); }

void source_engine::vmf::Poly::debug_print()
{
	std::cout << "Poly '" << *this << "':" << std::endl;
	for(int i = 0; i < m_vertices.size(); i++)
		std::cout << "\tVertex (" << m_vertices[i].pos.x << "," << m_vertices[i].pos.y << "," << m_vertices[i].pos.z << ")" << std::endl;
}

void source_engine::vmf::Poly::SortVertices()
{
	if(!IsValid())
		return;
	glm::vec3 a(m_vertices[1].pos);
	uvec::sub(&a, m_vertices[0].pos);
	glm::vec3 an(a);
	glm::vec3 bn;
	a = glm::normalize(a);
	unsigned int numVerts = GetVertexCount();
	double BiggestAngle;
	unsigned int tempvert1 = 0;
	unsigned int tempvert2;
	unsigned int biggestVert = NULL;
	for(unsigned int i = 0; i < ((numVerts > 0) ? (numVerts - 1) : 0); i++) {
		BiggestAngle = 1;
		tempvert2 = tempvert1 + 1;
		for(unsigned j = i + 1; j < numVerts; j++) {
			glm::vec3 b(m_vertices[tempvert2].pos);
			uvec::sub(&b, m_vertices[tempvert1].pos);
			bn = glm::vec3(b);
			b = glm::normalize(b);

			float dotProd = glm::dot(a, b);
			if(dotProd < BiggestAngle) {
				BiggestAngle = dotProd;
				biggestVert = tempvert2;
			}
			tempvert2++;
		}
		if(biggestVert == NULL)
			break;
		a = glm::vec3(m_vertices[tempvert1].pos);
		uvec::sub(&a, m_vertices[biggestVert].pos);
		an = glm::vec3(a);
		a = glm::normalize(a);

		if(biggestVert != tempvert1 + 1) {
			glm::vec3 v(m_vertices[tempvert1 + 1].pos);
			m_vertices[tempvert1 + 1].pos = m_vertices[biggestVert].pos;
			m_vertices[biggestVert].pos = v;
		}
		tempvert1++;
	}
	glm::vec3 n(m__normal);
	CalculatePlane();
	CalculateBounds();
	if(glm::dot(m_normal, n) > 0)
		ReverseVertexOrder();
	CalculateTextureAxes();
}

bool source_engine::vmf::Poly::CalculatePlane()
{
	if(!IsValid())
		return false;
	glm::vec3 centerOfMass(0, 0, 0);
	glm::vec3 n(0, 0, 0);
	unsigned int j;
	unsigned int numVerts = static_cast<unsigned int>(m_vertices.size());
	for(unsigned int i = 0; i < numVerts; i++) {
		j = i + 1;
		if(j >= numVerts)
			j = 0;
		n.x += (m_vertices[i].pos.y - m_vertices[j].pos.y) * (m_vertices[i].pos.z + m_vertices[j].pos.z);
		n.y += (m_vertices[i].pos.z - m_vertices[j].pos.z) * (m_vertices[i].pos.x + m_vertices[j].pos.x);
		n.z += (m_vertices[i].pos.x - m_vertices[j].pos.x) * (m_vertices[i].pos.y + m_vertices[j].pos.y);

		uvec::add(&centerOfMass, m_vertices[i].pos);
	}
	float magnitude = glm::length(n);
	n /= magnitude;
	centerOfMass /= numVerts;
	m_centerOfMass = centerOfMass;
	m_normal = n;
	glm::vec3 c = -centerOfMass;
	m_distance = glm::dot(c, n);
	return true;
}

glm::vec3 *source_engine::vmf::Poly::GetWorldPosition() { return &m_centerOfMass; }

void source_engine::vmf::Poly::CalculateBounds()
{
	glm::vec3 center = GetCenter();
	glm::vec3 min;
	glm::vec3 max;
	uvec::max(&min);
	uvec::min(&max);
	for(int i = 0; i < m_vertices.size(); i++) {
		glm::vec3 pos = m_vertices[i].pos;
		glm::vec3 diff(pos);
		uvec::sub(&diff, center);
		uvec::min(&min, diff);
		uvec::max(&max, diff);
	}
	//Con::cout<<"Poly Bounds: "<<Vector3::tostring(&min)<<" - "<<Vector3::tostring(&max)<<Con::endl;
	m_min = min;
	m_max = max;
}

glm::vec3 source_engine::vmf::Poly::GetCenter() { return m_center; }

void source_engine::vmf::Poly::ReverseVertexOrder()
{
	unsigned int numVerts = static_cast<unsigned int>(m_vertices.size());
	unsigned int h = static_cast<unsigned int>(numVerts * 0.5f);
	for(unsigned int i = 0; i < h; i++) {
		glm::vec3 pos = m_vertices[i].pos;
		m_vertices[i].pos = m_vertices[numVerts - i - 1].pos;
		m_vertices[numVerts - i - 1].pos = pos;
	}
}

std::vector<source_engine::vmf::Vertex> &source_engine::vmf::Poly::GetVertices() { return m_vertices; }

void source_engine::vmf::Poly::GetBounds(glm::vec3 *min, glm::vec3 *max)
{
	min->x = m_min.x;
	min->y = m_min.y;
	min->z = m_min.z;
	max->x = m_max.x;
	max->y = m_max.y;
	max->z = m_max.z;
}

void source_engine::vmf::Poly::Localize(const glm::vec3 &center)
{
	uvec::add(&m_centerLocalized, center);
	for(int i = 0; i < m_vertices.size(); i++)
		uvec::sub(&m_vertices[i].pos, center);
	Calculate();
}

void source_engine::vmf::Poly::CalculateTextureAxes()
{
	TextureData *texData = GetTextureData();
	if(texData == NULL)
		return;

	Material *mat = GetMaterial();
	TextureInfo *tex = NULL;
	if(mat != NULL)
		tex = mat->GetTextureInfo("diffusemap");
	int w, h;
	if(tex != NULL) {
		w = tex->width;
		h = tex->height;
	}
	else {
		w = 512;
		h = 512;
	}
	for(int i = 0; i < m_vertices.size(); i++) {
		glm::vec3 v = m_vertices[i].pos;
		uvec::add(&v, m_centerLocalized);
		float du = glm::dot(v, texData->nu);
		float tu = (du / w) / texData->su + texData->ou / w;
		float dv = glm::dot(v, texData->nv);
		float tv = (dv / h) / texData->sv + texData->ov / h;
		m_vertices[i].u = tu;
		m_vertices[i].v = tv;
	}
}

void source_engine::vmf::Poly::Calculate()
{
	CalculateNormal();
	CalculatePlane();
	CalculateBounds();
	// TODO: CalculateTextureAxes()
}

void source_engine::vmf::Poly::CalculateNormal()
{
	unsigned int numVerts = static_cast<unsigned int>(m_vertices.size());
	if(numVerts < 2)
		return;
	glm::vec3 a = m_vertices[0].pos - m_vertices[numVerts - 1].pos;
	glm::vec3 b = m_vertices[1].pos - m_vertices[0].pos;
	glm::vec3 n = glm::normalize(glm::cross(a, b));
	glm::vec3 nd = -n;
	float d = glm::dot(nd, m_vertices[numVerts - 1].pos);
	SetNormal(n);
	SetDistance(d);
}

bool source_engine::vmf::Poly::GenerateTriangleMesh(std::vector<glm::vec3> *verts, std::vector<glm::vec2> *uvs, std::vector<glm::vec3> *normals)
{
	if(!IsValid())
		return false;
	unsigned int pivot = 0;
	Vertex &a = m_vertices[pivot];
	unsigned int numVerts = static_cast<unsigned int>(m_vertices.size());
	for(unsigned int i = pivot + 2; i < numVerts; i++) {
		Vertex &b = m_vertices[i - 1];
		Vertex &c = m_vertices[i];
		verts->push_back(a.pos);
		verts->push_back(b.pos);
		verts->push_back(c.pos);

		uvs->push_back(glm::vec2(a.u, a.v));
		uvs->push_back(glm::vec2(b.u, b.v));
		uvs->push_back(glm::vec2(c.u, c.v));

		normals->push_back(a.normal);
		normals->push_back(b.normal);
		normals->push_back(c.normal);
	}
	return true;
}
