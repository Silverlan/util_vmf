// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <cassert>

module source_engine.vmf;

import :polymesh;

void source_engine::vmf::PolyMesh::BuildPolyMeshInfo()
{
	static std::vector<std::string> matMissing;
	auto bDisp = HasDisplacements();
	std::vector<Poly *> *polys;
	GetPolys(&polys);
	auto &info = GetCompiledData();
	for(auto it = polys->begin(); it != polys->end(); ++it) {
		auto *poly = *it;
		auto &polyInfo = poly->GetCompiledData();
		if(bDisp == false || poly->IsDisplacement()) {
			std::vector<Vertex> &polyVerts = poly->GetVertices();
			unsigned int numPolyVerts = static_cast<unsigned int>(polyVerts.size());
			for(unsigned int k = 0; k < numPolyVerts; k++) {
				glm::vec3 &va = polyVerts[k].pos;
				bool bExists = false;
				for(unsigned int l = 0; l < info.vertexList.size(); l++) {
					glm::vec3 &vb = info.vertexList[l];
					if(fabsf(va.x - vb.x) <= EPSILON && fabsf(va.y - vb.y) <= EPSILON && fabsf(va.z - vb.z) <= EPSILON) {
						bExists = true;
						break;
					}
				}
				if(bExists == false)
					info.vertexList.push_back(va);
			}
		}
		auto *texData = poly->GetTextureData();
		auto *mat = poly->GetMaterial();
		auto *texInfo = (mat != nullptr) ? mat->GetDiffuseMap() : nullptr;
		if(texInfo == nullptr && mat != nullptr) {
			// Find first texture in material
			std::function<bool(const std::shared_ptr<ds::Block> &)> fFindTexture = nullptr;
			fFindTexture = [&texInfo, &fFindTexture](const std::shared_ptr<ds::Block> &block) -> bool {
				auto *data = block->GetData();
				if(data == nullptr)
					return false;
				for(auto &pair : *data) {
					if(pair.second->IsBlock()) {
						if(fFindTexture(std::static_pointer_cast<ds::Block>(pair.second)) == true)
							return true;
						continue;
					}
					if(std::dynamic_pointer_cast<ds::Texture>(pair.second) != nullptr) {
						texInfo = &static_cast<ds::Texture *>(pair.second.get())->GetValue();
						return true;
					}
				}
				return false;
			};
			if(mat->GetPropertyDataBlock())
				fFindTexture(mat->GetPropertyDataBlock());
		}
		if(texInfo == nullptr) {
			auto bExists = false;
			for(unsigned int i = 0; i < matMissing.size(); i++) {
				if(matMissing[i] == texData->texture) {
					bExists = true;
					break;
				}
			}
			if(bExists == false) {
				matMissing.push_back(texData->texture);
				std::cout << "WARNING: Unable to find material '" << texData->texture << "'! Any meshes with this material may be rendered incorrectly!" << std::endl;
			}
			polyInfo.nu = glm::vec3(0, 0, 0);
			polyInfo.nv = glm::vec3(0, 0, 0);
			polyInfo.width = 512.f;
			polyInfo.height = 512.f;
		}
		else {
			polyInfo.nu = texData->nu;
			polyInfo.nv = texData->nv;
			polyInfo.width = static_cast<float>(texInfo->width);
			polyInfo.height = static_cast<float>(texInfo->height);
		}

		// Displacements
		if(poly->IsDisplacement()) {
			auto *disp = poly->GetDisplacement();
			auto &vertices = poly->GetVertices();
			std::vector<glm::vec3> sideVerts;
			sideVerts.reserve(vertices.size());
			for(auto it = vertices.begin(); it != vertices.end(); ++it)
				sideVerts.push_back((*it).pos);
			auto &dispInfo = *(polyInfo.displacement = std::make_unique<PolyDispInfo>()).get();
			dispInfo.power = disp->power;
			BuildDisplacement(poly, sideVerts, polyInfo.nu, polyInfo.nv, polyInfo.width, polyInfo.height, dispInfo.vertices, dispInfo.uvs, dispInfo.triangles, dispInfo.faceNormals, dispInfo.numAlpha, &dispInfo.alphas);
		}
	}
}

void source_engine::vmf::PolyMesh::BuildDisplacement(Poly *poly, std::vector<glm::vec3> &verts, glm::vec3 &nu, glm::vec3 &nv, unsigned int w, unsigned int h, std::vector<glm::vec3> &outVertices, std::vector<glm::vec2> &outUvs, std::vector<unsigned int> &outTriangles,
  std::vector<glm::vec3> &faceNormals, unsigned char &numAlpha, std::vector<glm::vec2> *outAlphas)
{
	// Texture coordinates
	auto *texData = poly->GetTextureData();
	float sw = 1.f / w;
	float sh = 1.f / h;
	float ou = texData->ou;
	float ov = texData->ov;
	float su = texData->su;
	float sv = texData->sv;
	//

	// Vertex Ids
	auto *disp = poly->GetDisplacement();
	auto numVerts = static_cast<unsigned int>(verts.size());
	auto &vertices = poly->GetVertices();
	auto numPolyVerts = static_cast<unsigned int>(vertices.size());
	std::vector<unsigned int> vertIds;
	for(unsigned int k = 0; k < numPolyVerts; k++) {
		auto &va = vertices[k].pos;
		for(unsigned int l = 0; l < numVerts; l++) {
			auto &vb = verts[l];
			if(fabsf(va.x - vb.x) <= EPSILON && fabsf(va.y - vb.y) <= EPSILON && fabsf(va.z - vb.z) <= EPSILON) {
				vertIds.push_back(l);
				break;
			}
		}
	}
	//

	auto rows = static_cast<unsigned int>(pow(2, disp->power)) + 1;
	// Alpha
	bool bHasAlphaA = false;
	bool bHasAlphaB = false;
	for(int i = (rows - 1); i >= 0; i--) {
		std::vector<glm::vec2> &vAlphas = disp->alphas[i];
		for(unsigned int j = 0; j < rows; j++) {
			if(vAlphas[j].x != 0.f) {
				bHasAlphaA = true;
				if(bHasAlphaB == true)
					break;
			}
			if(vAlphas[j].y != 0.f) {
				bHasAlphaB = true;
				if(bHasAlphaA == true)
					break;
			}
		}
		if(bHasAlphaA == true && bHasAlphaB == true)
			break;
	}
	if(bHasAlphaB == true && bHasAlphaA == false)
		bHasAlphaB = false;
	numAlpha = 0;
	if(bHasAlphaB == true)
		numAlpha = 2;
	else if(bHasAlphaA == true)
		numAlpha = 1;
	//
	BuildDisplacementTriangles(verts, disp->startpositionId, nu, nv, sw, sh, ou, ov, su, sv, disp->power, disp->normals, disp->offsets, disp->distances, numAlpha, disp->alphas, outVertices, outUvs, outTriangles, faceNormals, outAlphas);
}

#define NEIGHBOR_THIS 0
#define NEIGHBOR_TOP_LEFT 1
#define NEIGHBOR_TOP 2
#define NEIGHBOR_TOP_RIGHT 3
#define NEIGHBOR_LEFT 4
#define NEIGHBOR_RIGHT 5
#define NEIGHBOR_BOTTOM_LEFT 6
#define NEIGHBOR_BOTTOM 7
#define NEIGHBOR_BOTTOM_RIGHT 8

void source_engine::vmf::PolyMesh::BuildDisplacementTriangles(std::vector<glm::vec3> &sideVerts, unsigned int start, glm::vec3 &nu, glm::vec3 &nv, float sw, float sh, float ou, float ov, float su, float sv, unsigned char power, std::vector<std::vector<glm::vec3>> &normals,
  std::vector<std::vector<glm::vec3>> &offsets, std::vector<std::vector<float>> &distances, unsigned char numAlpha, std::vector<std::vector<glm::vec2>> &alphas, std::vector<glm::vec3> &outVertices, std::vector<glm::vec2> &outUvs, std::vector<unsigned int> &outTriangles,
  std::vector<glm::vec3> &faceNormals, std::vector<glm::vec2> *outAlphas)
{
	int rows = umath::pow(2, CInt32(power)) + 1;
	unsigned int numVerts = rows * rows;
	outUvs.resize(numVerts);
	if(numAlpha > 0)
		outAlphas->resize(numVerts);
	glm::vec3 sortedSideVerts[4];
	char j = 0;
	for(auto i = start; i < CUInt32(sideVerts.size()); i++) {
		sortedSideVerts[j] = sideVerts[i];
		j++;
	}
	for(unsigned int i = 0; i < start; i++) {
		sortedSideVerts[j] = sideVerts[i];
		j++;
	}
	glm::vec3 &x1 = sortedSideVerts[0];
	glm::vec3 &x2 = sortedSideVerts[1];
	glm::vec3 &y1 = sortedSideVerts[3];
	glm::vec3 &y2 = sortedSideVerts[2];
	glm::vec3 xOffset1 = (x2 - x1) / float(rows - 1);
	glm::vec3 xOffset2 = (y2 - y1) / float(rows - 1);
	glm::vec3 yOffset = (y1 - x1) / float(rows - 1); // CHECKME
	outVertices.resize(numVerts);
	glm::vec3 cur = sortedSideVerts[0];
	for(int col = 0; col < rows; col++) {
		std::vector<glm::vec3> &cNormals = normals[col];
		std::vector<glm::vec2> *cAlphas = (numAlpha > 0) ? &alphas[col] : nullptr;
		std::vector<float> &cDistances = distances[col];
		std::vector<glm::vec3> &cOffsets = offsets[col];

		glm::vec3 rowPos = cur;
		float offsetScale = col / float(rows - 1);
		for(int row = 0; row < rows; row++) {
			unsigned int idx = col * rows + row;
			//outNormals[idx] = -cNormals[row]; // This is the offset normal, not the actual face normal!
			if(numAlpha > 0)
				(*outAlphas)[idx] = (*cAlphas)[row];

			glm::vec3 vA = rowPos + cNormals[row] * cDistances[row] + cOffsets[row];
			glm::vec2 uv;
			uv.x = (glm::dot(rowPos, nu) * sw) / su + ou * sw;
			uv.y = 1.f - ((glm::dot(rowPos, nv) * sh) / sv + ov * sh);
			outUvs[idx] = uv;
			outVertices[idx] = vA;

			glm::vec3 xOffset = (1.f - offsetScale) * xOffset1 + offsetScale * xOffset2;
			rowPos += xOffset;
		}
		cur = sortedSideVerts[0] + (yOffset * float(col + 1));
		//if(col < rows -1)
		//	cur += yOffset +normals[col +1][0] *distances[col +1][0]; // +cNormals[0] *cDistances[0]; TODO!! -> Next column!
	}
	outTriangles.resize((rows - 1) * (rows - 1) * 6);
	unsigned int idx = 0;
	for(int col = 0; col < rows - 1; col++) {
		for(int row = 0; row < rows - 1; row++) {
			int a = col * rows + row;
			int b = a + 1;
			int c = (col + 1) * rows + row;
			outTriangles[idx] = a;
			outTriangles[idx + 1] = b;
			outTriangles[idx + 2] = c;

			// Calculate Face Normals
			auto na = -uvec::cross(outVertices[c] - outVertices[a], outVertices[b] - outVertices[a]);
			uvec::normalize(&na);
			faceNormals.push_back(na);
			//

			idx += 3;
			int d = (col + 1) * rows + row + 1;
			outTriangles[idx] = b;
			outTriangles[idx + 1] = d;
			outTriangles[idx + 2] = c;

			// Calculate Face Normals
			auto nb = -uvec::cross(outVertices[c] - outVertices[b], outVertices[d] - outVertices[b]);
			uvec::normalize(&nb);
			faceNormals.push_back(nb);
			//

			idx += 3;
		}
	}
}

static constexpr double EPSILON = 4.9406564584125e-2;
static void FindNeighborNormals(glm::vec3 &v, int rows, std::vector<glm::vec3> &verts, std::vector<unsigned int> &triangles, std::vector<glm::vec3> &faceNormals, std::vector<glm::vec3> &neighborNormals)
{
	for(auto col = 0; col < (rows - 1); col++) {
		for(auto row = 0; row < (rows - 1); row += ((col > 0 && col < (rows - 2)) ? (rows - 2) : 1)) {
			unsigned int idx = col * (rows - 1) * 6 + row * 6;
			unsigned int faceId = col * (rows - 1) * 2 + row * 2;
			auto vertAId1 = triangles[idx];
			auto vertAId2 = triangles[idx + 1];
			auto vertAId3 = triangles[idx + 2];
			auto &nA = faceNormals[faceId];
			idx += 3;
			faceId++;

			auto &va1 = verts[vertAId1];
			auto &va2 = verts[vertAId2];
			auto &va3 = verts[vertAId3];
			auto hit = 0;
			if((fabsf(va1.x - v.x) <= EPSILON && fabsf(va1.y - v.y) <= EPSILON && fabsf(va1.z - v.z) <= EPSILON) || (fabsf(va2.x - v.x) <= EPSILON && fabsf(va2.y - v.y) <= EPSILON && fabsf(va2.z - v.z) <= EPSILON)
			  || (fabsf(va3.x - v.x) <= EPSILON && fabsf(va3.y - v.y) <= EPSILON && fabsf(va3.z - v.z) <= EPSILON))
				hit |= 1;
			auto vertBId1 = triangles[idx];
			auto vertBId2 = triangles[idx + 1];
			auto vertBId3 = triangles[idx + 2];
			auto &nB = faceNormals[faceId];

			auto &vb1 = verts[vertBId1];
			auto &vb2 = verts[vertBId2];
			auto &vb3 = verts[vertBId3];
			if((fabsf(vb1.x - v.x) <= EPSILON && fabsf(vb1.y - v.y) <= EPSILON && fabsf(vb1.z - v.z) <= EPSILON) || (fabsf(vb2.x - v.x) <= EPSILON && fabsf(vb2.y - v.y) <= EPSILON && fabsf(vb2.z - v.z) <= EPSILON)
			  || (fabsf(vb3.x - v.x) <= EPSILON && fabsf(vb3.y - v.y) <= EPSILON && fabsf(vb3.z - v.z) <= EPSILON))
				hit |= 2;
			if(hit > 0) {
				if(hit & 1)
					neighborNormals.push_back(nA);
				if(hit & 2)
					neighborNormals.push_back(nB);
				if(hit < 3)
					neighborNormals.push_back(neighborNormals.back()); // Weigh it double
			}
		}
	}
}

static std::vector<glm::vec3> GetSortedVertices(source_engine::vmf::Poly *poly)
{
	auto &polyVerts = poly->GetVertices();
	auto &disp = *poly->GetDisplacement();
	auto &startPos = disp.startposition;
	std::vector<glm::vec3> sortedPolyVerts;
	sortedPolyVerts.reserve(polyVerts.size());
	auto start = -1;
	auto idx = 0;
	auto dMin = std::numeric_limits<float>::max();
	auto vertIdx = 0u;
	for(auto &v : polyVerts) {
		auto d = uvec::distance(startPos, v.pos);
		if(d < dMin) {
			dMin = d;
			start = vertIdx;
		}
		++vertIdx;
	}
	assert(start != -1);
	for(auto it = polyVerts.begin() + start; it != polyVerts.end(); ++it)
		sortedPolyVerts.push_back((*it).pos);
	for(auto it = polyVerts.begin(); it != polyVerts.begin() + start; ++it)
		sortedPolyVerts.push_back((*it).pos);
	return sortedPolyVerts;
}

void source_engine::vmf::PolyMesh::BuildDisplacementNormals(std::vector<std::shared_ptr<PolyMesh>> &meshes, PolyMesh *mesh, Poly *poly, PolyInfo &info, std::vector<glm::vec3> &outNormals)
{
	auto &displacement = *info.displacement.get();
	auto &faceNormals = displacement.faceNormals;
	auto rows = umath::pow(2, CInt32(displacement.power)) + 1;
	auto numVerts = rows * rows;
	outNormals.resize(numVerts);

	std::array<PolyDispInfo *, 9> neighbors = {&displacement, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	auto &polyVerts = poly->GetVertices();
	assert(polyVerts.size() == 4);

	// Debug
	auto sortedPolyVerts = GetSortedVertices(poly);
	auto &disp = *poly->GetDisplacement();
	for(auto it = meshes.begin(); it != meshes.end(); ++it) {
		auto &polyMesh = *it;
		if(polyMesh->HasDisplacements()) {
			auto &meshInfo = polyMesh->GetCompiledData();
			std::vector<Poly *> *polys;
			polyMesh->GetPolys(&polys);
			for(auto it = polys->begin(); it != polys->end(); ++it) {
				auto *polyOther = *it;
				if(polyOther->IsDisplacement() && polyOther != poly) {
					auto &dispOther = *polyOther->GetDisplacement();
					if(dispOther.power == disp.power) {
						auto &otherPolyVerts = polyOther->GetVertices();
						assert(otherPolyVerts.size() == 4);
						auto touching = 0;
						auto numTouching = 0;
						for(auto it = otherPolyVerts.begin(); it != otherPolyVerts.end(); ++it) {
							auto &vOther = (*it).pos;
							auto idx = 0;
							for(auto it = sortedPolyVerts.begin(); it != sortedPolyVerts.end(); ++it) {
								auto &v = *it;
								if(fabsf(vOther.x - v.x) <= EPSILON && fabsf(vOther.y - v.y) <= EPSILON && fabsf(vOther.z - v.z) <= EPSILON) {
									numTouching++;
									touching |= 1 << idx;
								}
								idx++;
							}
						}
						if(touching > 0) {
							if(numTouching < 3) // Else displacement is overlapping; No smoothing possible
							{
								auto &polyInfo = polyOther->GetCompiledData();
								auto &dispInfo = *polyInfo.displacement.get();
								if(touching == 1)
									neighbors[NEIGHBOR_TOP_RIGHT] = &dispInfo;
								else if(touching == 3)
									neighbors[NEIGHBOR_TOP] = &dispInfo;
								else if(touching == 2)
									neighbors[NEIGHBOR_TOP_LEFT] = &dispInfo;
								else if(touching == 6)
									neighbors[NEIGHBOR_LEFT] = &dispInfo;
								else if(touching == 4)
									neighbors[NEIGHBOR_BOTTOM_LEFT] = &dispInfo;
								else if(touching == 12)
									neighbors[NEIGHBOR_BOTTOM] = &dispInfo;
								else if(touching == 8)
									neighbors[NEIGHBOR_BOTTOM_RIGHT] = &dispInfo;
								else if(touching == 9)
									neighbors[NEIGHBOR_RIGHT] = &dispInfo;
							}
							break;
						}
					}
				}
			}
		}
	}

	// Calculate Vertex Normals
	for(auto col = 0; col < rows; col++) {
		for(auto row = 0; row < rows; row++) {
			auto vertId = col * rows + row;
			std::vector<glm::vec3> neighborNormals;
			neighborNormals.reserve(8);
			if(col > 0 && row > 0 && col < (rows - 1) && row < (rows - 1)) {
				auto a = (col - 1) * (rows - 1) + (row - 1);
				auto b = col * (rows - 1) + (row - 1);
				auto c = (col - 1) * (rows - 1) + row;
				auto d = col * (rows - 1) + row;
				neighborNormals.push_back(faceNormals[a * 2 + 1]);
				neighborNormals.push_back(neighborNormals.back());

				neighborNormals.push_back(faceNormals[b * 2]);
				neighborNormals.push_back(faceNormals[b * 2 + 1]);

				neighborNormals.push_back(faceNormals[c * 2]);
				neighborNormals.push_back(faceNormals[c * 2 + 1]);

				neighborNormals.push_back(faceNormals[d * 2]);
				neighborNormals.push_back(neighborNormals.back());
			}
			else {
				auto &v = displacement.vertices[vertId];
				for(auto it = neighbors.begin(); it != neighbors.end(); ++it) {
					auto *neighbor = *it;
					if(neighbor != nullptr) {
						auto &nbVerts = neighbor->vertices;
						auto &nbTriangles = neighbor->triangles;
						auto &nbFaceNormals = neighbor->faceNormals;
						FindNeighborNormals(v, rows, nbVerts, nbTriangles, nbFaceNormals, neighborNormals);
					}
				}
			}
			auto &n = outNormals[vertId] = glm::vec3(0.f, 0.f, 0.f);
			for(auto it = neighborNormals.begin(); it != neighborNormals.end(); ++it)
				n += *it;
			if(!neighborNormals.empty())
				n /= neighborNormals.size();
			uvec::normalize(&n);
			// TODO: Fill up with "up" vectors when not touching other displacements?
		}
	}
	//
}
