#include "stdafx.h"
#include "AABB.h"
//-----------------------------------------------------------------------------
AABB::AABB(const glm::vec3& inMin, const glm::vec3& inMax)
	: min(inMin), max(inMax)
{
}
//-----------------------------------------------------------------------------
AABB::AABB(const std::vector<glm::vec3>& points)
{
	for (size_t i = 0; i < points.size(); i++)
		Combine(points[i]);
}
//-----------------------------------------------------------------------------
float AABB::GetVolume() const
{
	glm::vec3 diagonal = max - min;
	return diagonal.x * diagonal.y * diagonal.z;
}
//-----------------------------------------------------------------------------
glm::vec3 AABB::GetCentre() const
{
	return (min + max) * 0.5f;
}
//-----------------------------------------------------------------------------
glm::vec3 AABB::GetHalfSize() const
{
	return (max - min) * 0.5f;
}
//-----------------------------------------------------------------------------
glm::vec3 AABB::GetDiagonal() const
{
	return max - min;
}
//-----------------------------------------------------------------------------
float AABB::GetSurfaceArea() const
{
	glm::vec3 diagonal = max - min;
	return 2.0f * (diagonal.x * diagonal.y + diagonal.y * diagonal.z + diagonal.z * diagonal.x);
}
//-----------------------------------------------------------------------------
void AABB::Combine(const AABB& anotherAABB)
{
	min = glm::min(min, anotherAABB.min);
	max = glm::max(max, anotherAABB.max);
}
//-----------------------------------------------------------------------------
void AABB::Combine(const glm::vec3& point)
{
	min = glm::min(min, point);
	max = glm::max(max, point);
}
//-----------------------------------------------------------------------------
bool AABB::Overlaps(const AABB& anotherAABB)
{
	return max.x > anotherAABB.min.x && min.x < anotherAABB.max.x
		&& max.y > anotherAABB.min.y && min.y < anotherAABB.max.y
		&& max.z > anotherAABB.min.z && min.z < anotherAABB.max.z;
}
//-----------------------------------------------------------------------------
bool AABB::Inside(const glm::vec3& point)
{
	return max.x > point.x && min.x < point.x
		&& max.y > point.y && min.y < point.y
		&& max.z > point.z && min.z < point.x;
}
//-----------------------------------------------------------------------------