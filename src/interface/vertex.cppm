/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
