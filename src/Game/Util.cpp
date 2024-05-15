#include "stdafx.h"
#include "Util.h"

glm::vec3 orbit_axis(float angle, glm::vec3 const& axis, glm::vec3 const& spread) { return glm::angleAxis(angle, axis) * spread; }

float lerp(float a, float b, float f) { return a + f * (b - a); }