// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <vector>

export module source_engine.vmf:dispinfo;

export import pragma.math;

export namespace source_engine::vmf {
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
