// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <cmath>

module source_engine.vmf;

import :intersection;

bool source_engine::vmf::Intersect::VectorInBounds(glm::vec3 *vec, glm::vec3 *min, glm::vec3 *max, float EPSILON)
{
	if(EPSILON == 0.f)
		return vec->x >= min->x && vec->y >= min->y && vec->z >= min->z && vec->x <= max->x && vec->y <= max->y && vec->z <= max->z;
	return vec->x >= min->x - EPSILON && vec->y >= min->y - EPSILON && vec->z >= min->z - EPSILON && vec->x <= max->x + EPSILON && vec->y <= max->y + EPSILON && vec->z <= max->z + EPSILON;
}

bool source_engine::vmf::Intersect::SphereSphere(glm::vec3 &originA, float rA, glm::vec3 &originB, float rB)
{
	float dist = glm::distance(originA, originB);
	return dist - (rA + rB) <= 0;
}

bool source_engine::vmf::Intersect::AABBSphere(glm::vec3 &min, glm::vec3 &max, glm::vec3 &origin, float r)
{
	glm::vec3 pClosest;
	source_engine::vmf::Geometry::ClosestPointOnAABBToPoint(min, max, origin, pClosest);
	float d = glm::distance(pClosest, origin);
	return d <= r;
}

bool source_engine::vmf::Intersect::AABBInAABB(const glm::vec3 &minA, const glm::vec3 &maxA, const glm::vec3 &minB, const glm::vec3 &maxB) { return (((minA.x >= minB.x && minA.y >= minB.y && minA.z >= minB.z) && (maxA.x <= maxB.x && maxA.y <= maxB.y && maxA.z <= maxB.z))) ? true : false; }

source_engine::vmf::IntersectResult source_engine::vmf::Intersect::AABBAABB(const glm::vec3 &minA, const glm::vec3 &maxA, const glm::vec3 &minB, const glm::vec3 &maxB)
{
	if((maxA.x < minB.x) || (minA.x > maxB.x) || (maxA.y < minB.y) || (minA.y > maxB.y) || (maxA.z < minB.z) || (minA.z > maxB.z))
		return IntersectResult::Outside;
	if(AABBInAABB(minA, maxA, minB, maxB) || AABBInAABB(minB, maxB, minA, maxA))
		return IntersectResult::Inside;
	return IntersectResult::Overlap;
}

bool source_engine::vmf::Intersect::AABBAABB(AABB *a, AABB *b)
{
	glm::vec3 t = b->pos - a->pos;
	return fabs(t.x) <= (a->extents.x + b->extents.x) && fabs(t.y) <= (a->extents.y + b->extents.y) && fabs(t.z) <= (a->extents.z + b->extents.z);
}

bool source_engine::vmf::Intersect::AABBTriangle(glm::vec3 min, glm::vec3 max, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 center = (min + max) * 0.5f;
	min -= center;
	max -= center;
	a -= center;
	b -= center;
	c -= center;
	glm::vec3 minTri = a;
	glm::vec3 maxTri = a;
	uvec::min(&minTri, b);
	uvec::min(&minTri, c);
	uvec::max(&maxTri, b);
	uvec::max(&maxTri, c);
	if(AABBAABB(min, max, minTri, maxTri) == IntersectResult::Outside)
		return false;
	// TODO
	return true;
}

bool source_engine::vmf::Intersect::LineAABB(glm::vec3 &o, glm::vec3 &d, glm::vec3 &min, glm::vec3 &max, float *tMinRes, float *tMaxRes)
{
	glm::vec3 dirInv(1 / d.x, 1 / d.y, 1 / d.z);
	const int sign[] = {dirInv.x < 0, dirInv.y < 0, dirInv.z < 0};
	glm::vec3 bounds[] = {min, max};
	float tMin = (bounds[sign[0]].x - o.x) * dirInv.x;
	float tMax = (bounds[1 - sign[0]].x - o.x) * dirInv.x;
	float tyMin = (bounds[sign[1]].y - o.y) * dirInv.y;
	float tyMax = (bounds[1 - sign[1]].y - o.y) * dirInv.y;
	if((tMin > tyMax) || (tyMin > tMax))
		return false;
	if(tyMin > tMin)
		tMin = tyMin;
	if(tyMax < tMax)
		tMax = tyMax;
	float tzMin = (bounds[sign[2]].z - o.z) * dirInv.z;
	float tzMax = (bounds[1 - sign[2]].z - o.z) * dirInv.z;
	if((tMin > tzMax) || (tzMin > tMax))
		return false;
	if(tzMin > tMin)
		tMin = tzMin;
	if(tzMax < tMax)
		tMax = tzMax;
	*tMinRes = tMin;
	if(tMaxRes != NULL)
		*tMaxRes = tMax;
	return true;
}

bool source_engine::vmf::Intersect::LinePlane(glm::vec3 &o, glm::vec3 &dir, glm::vec3 &nPlane, float distPlane, float *t)
{
	float f = glm::dot(nPlane, dir);
	if(f == 0)
		return false;
	float hit = (glm::dot(nPlane, (nPlane * -distPlane) - o)) / f;
	if(t != NULL)
		*t = hit;
	return true;
}

////////////////////////////////////

void source_engine::vmf::Geometry::ClosestPointOnAABBToPoint(glm::vec3 &min, glm::vec3 &max, glm::vec3 &point, glm::vec3 &res)
{
	for(int i = 0; i < 3; i++) {
		float v = point[i];
		if(v < min[i])
			v = min[i];
		if(v > max[i])
			v = max[i];
		res[i] = v;
	}
}

void source_engine::vmf::Geometry::ClosestPointOnPlaneToPoint(glm::vec3 &n, float d, glm::vec3 &p, glm::vec3 &res)
{
	//float t = glm::dot(n,p);// -d;
	float t = glm::dot(n, p) - d;
	res = p - t * n;
}

void source_engine::vmf::Geometry::ClosestPointOnTriangleToPoint(glm::vec3 &a, glm::vec3 &b, glm::vec3 &c, glm::vec3 &p, glm::vec3 &res)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ap = p - a;
	float dA = glm::dot(ab, ap);
	float dB = glm::dot(ac, ap);
	if(dA <= 0.0f && dB <= 0.0f) {
		res = a;
		return;
	}
	glm::vec3 bp = p - b;
	float dC = glm::dot(ab, bp);
	float dD = glm::dot(ac, bp);
	if(dC >= 0.0f && dD <= dC) {
		res = b;
		return;
	}
	float vc = dA * dD - dC * dB;
	if(vc <= 0.0f && dA >= 0.0f && dC <= 0.0f) {
		float v = dA / (dA - dC);
		res = a + v * ab;
		return;
	}
	glm::vec3 cp = p - c;
	float dE = glm::dot(ab, cp);
	float dF = glm::dot(ac, cp);
	if(dF >= 0.0f && dE <= dF) {
		res = c;
		return;
	}
	float vb = dE * dB - dA * dF;
	if(vb <= 0.0f && dB >= 0.0f && dF <= 0.0f) {
		float w = dB / (dB - dF);
		res = a + w * ac;
		return;
	}
	float va = dC * dF - dE * dD;
	if(va <= 0.0f && (dD - dC) >= 0.0f && (dE - dF) >= 0.0f) {
		float w = (dD - dC) / ((dD - dC) + (dE - dF));
		res = b + w * (c - b);
		return;
	}
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	res = a + ab * v + ac * w;
}

const float EPSILON = 1.19209e-005f;
float source_engine::vmf::Geometry::ClosestPointsBetweenLines(glm::vec3 &pA, glm::vec3 &qA, glm::vec3 &pB, glm::vec3 &qB, float &s, float &t, glm::vec3 &cA, glm::vec3 &cB)
{
	glm::vec3 dA = qA - pA;
	glm::vec3 dB = qB - pB;
	glm::vec3 r = pA - pB;
	float a = glm::dot(dA, dA);
	float e = glm::dot(dB, dB);
	float f = glm::dot(dB, r);
	if(a <= EPSILON && e <= EPSILON) {
		s = t = 0.0f;
		cA = pA;
		cB = pB;
		return glm::dot(cA - cB, cA - cB);
	}
	if(a <= EPSILON) {
		s = 0.0f;
		t = f / e;
		t = glm::clamp(t, 0.0f, 1.0f);
	}
	else {
		float c = glm::dot(dA, r);
		if(e <= EPSILON) {
			t = 0.0f;
			s = glm::clamp(-c / a, 0.0f, 1.0f);
		}
		else {
			float b = glm::dot(dA, dB);
			float denom = a * e - b * b;
			if(denom != 0.0f)
				s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			else
				s = 0.0f;
			t = (b * s + f) / e;
			if(t < 0.0f) {
				t = 0.0f;
				s = glm::clamp(-c / a, 0.0f, 1.0f);
			}
			else if(t > 1.0f) {
				t = 1.0f;
				s = glm::clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}
	cA = pA + dA * s;
	cB = pB + dB * t;
	return glm::dot(cA - cB, cA - cB);
}

////////////////////////////////////

bool source_engine::vmf::Sweep::AABBWithAABB(glm::vec3 aa, glm::vec3 &ab, glm::vec3 &extA, glm::vec3 ba, glm::vec3 &bb, glm::vec3 &extB, float *entryTime, float *exitTime, glm::vec3 *normal)
{
	::source_engine::vmf::AABB a(aa, extA);
	::source_engine::vmf::AABB b(ba, extB);
	*entryTime = 0;
	*exitTime = 0;
	if(normal != NULL) {
		normal->x = 0;
		normal->y = 0;
		normal->z = 0;
	}
	if(a.Intersect(&b))
		return true;
	glm::vec3 va = ab - aa;
	glm::vec3 vb = bb - ba;
	glm::vec3 invEntry(0, 0, 0);
	glm::vec3 invExit(0, 0, 0);
	glm::vec3 entry(0, 0, 0);
	glm::vec3 exit(0, 0, 0);
	glm::vec3 aMax = aa + extA;
	glm::vec3 bMax = ba + extB;
	aa -= extA;
	ba -= extB;
	for(int i = 0; i < 3; i++) {
		if(va[i] > 0.0f) {
			invEntry[i] = ba[i] - aMax[i];
			invExit[i] = bMax[i] - aa[i];
		}
		else {
			invEntry[i] = bMax[i] - aa[i];
			invExit[i] = ba[i] - aMax[i];
		}
		if(va[i] == 0.0f) {
			entry[i] = -std::numeric_limits<float>::infinity();
			exit[i] = std::numeric_limits<float>::infinity();
		}
		else {
			entry[i] = invEntry[i] / va[i];
			exit[i] = invExit[i] / va[i];
		}
	}
	float tEntry = std::max(entry.x, std::max(entry.y, entry.z));
	float tExit = std::min(exit.x, std::min(exit.y, exit.z));
	if(tEntry > tExit || entry.x < 0 && entry.y < 0 && entry.z < 0 || entry.x > 1 || entry.y > 1 || entry.z > 1)
		return false;
	if(entry.x > entry.y && entry.x > entry.z) {
		if(invEntry.x < 0) {
			normal->x = 1;
			normal->y = 0;
			normal->z = 0;
		}
		else {
			normal->x = -1;
			normal->y = 0;
			normal->z = 0;
		}
	}
	else if(entry.y > entry.x && entry.y > entry.z) {
		if(invEntry.y < 0) {
			normal->x = 0;
			normal->y = 1;
			normal->z = 0;
		}
		else {
			normal->x = 0;
			normal->y = -1;
			normal->z = 0;
		}
	}
	else {
		if(invEntry.z < 0) {
			normal->x = 0;
			normal->y = 0;
			normal->z = 1;
		}
		else {
			normal->x = 0;
			normal->y = 0;
			normal->z = -1;
		}
	}
	*entryTime = tEntry;
	*exitTime = tExit;
	return true;
}

bool source_engine::vmf::Sweep::AABBWithPlane(glm::vec3 &origin, glm::vec3 &dir, glm::vec3 &ext, glm::vec3 &planeNormal, float planeDistance, float *t)
{
	float r = ext.x * fabs(planeNormal.x) + ext.y * fabs(planeNormal.y) + ext.z * fabs(planeNormal.z);
	*t = (r - planeDistance - (glm::dot(planeNormal, origin))) / glm::dot(planeNormal, dir);
	if(*t < 0 || *t > 1)
		return false;
	return true;
}
