// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module source_engine.vmf:polymesh;

import :poly;

export namespace source_engine::vmf {
	class BrushMesh;
	class PolyMesh {
	  public:
		friend std::ostream &operator<<(std::ostream &, const PolyMesh &);
		bool m_bHasDisplacements;
		PolyMesh();
		~PolyMesh();
	  private:
		std::vector<Poly *> m_polys;
		PolyMeshInfo m_compiledData;
		glm::vec3 m_min;
		glm::vec3 m_max;
		glm::vec3 m_center;
		glm::vec3 *m_centerLocalized;
		glm::vec3 m_centerOfMass;
		bool m_bValid;
		std::vector<glm::vec3 *> m_vertices;
		bool HasVertex(Vertex *v, glm::vec3 *vThis = nullptr);
		void Localize(const glm::vec3 &center);
		void Localize();

		void BuildDisplacement(Poly *poly, std::vector<glm::vec3> &verts, glm::vec3 &nu, glm::vec3 &nv, unsigned int w, unsigned int h, std::vector<glm::vec3> &outVertices, std::vector<glm::vec2> &outUvs, std::vector<unsigned int> &outTriangles, std::vector<glm::vec3> &faceNormals,
		  unsigned char &numAlpha, std::vector<glm::vec2> *outAlphas);
		void BuildDisplacementTriangles(std::vector<glm::vec3> &sideVerts, unsigned int start, glm::vec3 &nu, glm::vec3 &nv, float sw, float sh, float ou, float ov, float su, float sv, unsigned char power, std::vector<std::vector<glm::vec3>> &normals,
		  std::vector<std::vector<glm::vec3>> &offsets, std::vector<std::vector<float>> &distances, unsigned char numAlpha, std::vector<std::vector<glm::vec2>> &alphas, std::vector<glm::vec3> &outVertices, std::vector<glm::vec2> &outUvs, std::vector<unsigned int> &outTriangles,
		  std::vector<glm::vec3> &faceNormals, std::vector<glm::vec2> *outAlphas);
	  public:
		bool HasDisplacements();
		void SetHasDisplacements(bool b);
		void AddPoly(Poly *poly);
		int BuildPolyMesh();
		void Calculate();
		PolyMeshInfo &GetCompiledData();
		unsigned int GetPolyCount();
		void CenterPolys();
		void GetPolys(std::vector<Poly *> **polys);
		void debug_print();
		static void GenerateBrushMeshes(std::vector<BrushMesh *> *brushMeshes, std::vector<PolyMesh *> meshes);
		void GetBounds(glm::vec3 *min, glm::vec3 *max);
		glm::vec3 GetCenter();
		glm::vec3 *GetLocalizedCenter();
		glm::vec3 *GetWorldPosition();
		void Validate();
		bool IsValid();
		void BuildPolyMeshInfo();

		static void BuildDisplacementNormals(std::vector<std::shared_ptr<PolyMesh>> &meshes, PolyMesh *mesh, Poly *poly, PolyInfo &info, std::vector<glm::vec3> &outNormals);
	};
};

inline std::ostream &source_engine::vmf::operator<<(std::ostream &os, const PolyMesh &mesh)
{
	os << "Mesh[" << &mesh << "] [" << mesh.m_polys.size() << "] [" << mesh.m_vertices.size() << "]";
	return os;
}
