// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string>
#include <optional>
#include <mathutil/uvec.h>
#include <materialmanager.h>

export module source_engine.vmf:poly;

import :vertex;
import :dispinfo;
import :polyinfo;

export namespace source_engine::vmf
{
	struct TextureData {
		std::string texture;
		Vector3 nu;
		Vector3 nv;
		float ou, ov;
		float su, sv;
		float rot;
	};

	const short CLASSIFY_FRONT = 0;
	const short CLASSIFY_BACK = 1;
	const short CLASSIFY_ONPLANE = 2;
	const short CLASSIFY_SPANNING = 3;

	const double EPSILON = 4.9406564584125e-2; //4.9406564584125e-4;//4.94065645841247e-324;
	class Poly;
	std::ostream &operator<<(std::ostream &os, const vmf::Poly &poly);
	class Poly {
	  public:
		friend std::ostream &operator<<(std::ostream &, const Poly &);
	  public:
		Poly(const std::function<Material *(const std::string &)> &fLoadMaterial);
	  protected:
		std::vector<Vertex> m_vertices;
		glm::vec3 m_normal, m__normal;
		glm::vec3 m_center;
		glm::vec3 m_centerLocalized = {};
		glm::vec3 m_min;
		glm::vec3 m_max;
		glm::vec3 m_centerOfMass;
		msys::MaterialHandle m_material = {};
		std::optional<TextureData> m_texData = {};
		std::optional<DispInfo> m_displacement = {};
		PolyInfo m_compiledData;
		std::function<Material *(const std::string &)> m_materialLoader = nullptr;
		uint32_t m_materialId = std::numeric_limits<uint32_t>::max();

		double m_distance;
		bool CalculatePlane();
		void CalculateBounds();
		void ReverseVertexOrder();
		void Calculate();
		void CalculateNormal();
		void CalculateTextureAxes();
	  public:
		PolyInfo &GetCompiledData();
		void SetDisplacement(const DispInfo &disp);
		DispInfo *GetDisplacement();
		bool IsDisplacement();
		void SetDistance(double d);
		void SetNormal(glm::vec3 n);
		double GetDistance();
		glm::vec3 GetNormal();
		bool AddUniqueVertex(glm::vec3 vert, glm::vec3 n);
		void AddVertex(glm::vec3 vert, glm::vec3 n);
		bool HasVertex(glm::vec3 *vert);
		unsigned int GetVertexCount();
		bool IsValid();
		virtual void SortVertices();
		glm::vec3 GetCenter();
		std::vector<Vertex> &GetVertices();
		void GetBounds(glm::vec3 *min, glm::vec3 *max);
		void Localize(const glm::vec3 &center);
		void debug_print();
		bool GenerateTriangleMesh(std::vector<glm::vec3> *verts, std::vector<glm::vec2> *uvs, std::vector<glm::vec3> *normals);
		virtual void SetTextureData(std::string texture, glm::vec3 nu, glm::vec3 nv, float ou, float ov, float su, float sv, float rot = 0);
		TextureData *GetTextureData();
		virtual Material *GetMaterial();
		glm::vec3 *GetWorldPosition();
		const short ClassifyPoint(glm::vec3 *point);
		glm::vec3 GetCalculatedNormal();
		void ToTriangles(std::vector<glm::vec3> *vertices);
		void Merge(Poly *other);
		void RemoveDisplacement();
		virtual void SetMaterial(Material *material);

		void SetMaterialId(uint32_t id);
		uint32_t GetMaterialId() const;
	};
};
