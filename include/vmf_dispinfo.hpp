/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __VMF_DISPINFO_HPP__
#define __VMF_DISPINFO_HPP__

#include <mathutil/uvec.h>
#include <vector>

namespace vmf {
	struct DispInfo {
		unsigned int power;
		unsigned int flags;
		unsigned int elevation;
		unsigned int subdiv;
		unsigned int startpositionId;
		Vector3 startposition;
		std::vector<std::vector<Vector3>> normals;
		std::vector<std::vector<float>> distances;
		std::vector<std::vector<Vector3>> offsets;
		std::vector<std::vector<Vector2>> alphas;
	};
};

#endif
