#pragma once

class AABB final
{
public:
	AABB() = default;
	AABB(const glm::vec3& inMin, const glm::vec3& inMax);
	AABB(const std::vector<glm::vec3>& points);

	float GetVolume() const;

	glm::vec3 GetCentre() const;
	glm::vec3 GetHalfSize() const;
	glm::vec3 GetDiagonal() const;

	float GetSurfaceArea() const;

	void Combine(const AABB& anotherAABB);
	void Combine(const glm::vec3& point);
	bool Overlaps(const AABB& anotherAABB);
	bool Inside(const glm::vec3& point);

	glm::vec3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
	glm::vec3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
};