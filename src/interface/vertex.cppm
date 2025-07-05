// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <mathutil/uvec.h>

export module source_engine.vmf:vertex;

export namespace source_engine::vmf {
	struct Vertex {
		Vector3 pos;
		Vector3 normal;
		double u, v;
		Vertex()
		{
			u = 0;
			v = 0;
		}
		Vertex(const Vector3 &_pos, const Vector3 &_normal)
		{
			Vertex();
			pos = _pos;
			normal = _normal;
		}
	};
};
