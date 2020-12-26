#pragma once

#include "Fwd.h"

namespace MeshPreset {

// location 0: vec3 pos
// location 1: vec2 texCoord
// location 2: vec3 normal
PMesh CreateUnitBox();

// location 0: vec3 pos
// location 1: vec2 texCoord
// location 2: vec3 normal
// location 3: vec3 tangent
PMesh CreateUnitShpere(uint32_t level);

// location 0: vec3 pos
// location 1: vec2 texCoord
// No normal
PMesh CreateQuad();

};  // namespace MeshPreset