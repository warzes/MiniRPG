#pragma once

#include "GModel.h"

class GraphicsSystem;

// TODO: rename IGameObject?
class IGraphicsObject
{
public:
	IGraphicsObject(Systems& systems);
	IGraphicsObject(Systems& systems, const std::string& objectName, int executionOrder);
	virtual ~IGraphicsObject() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;

	void SetPosition(const glm::vec3& position);
	void SetRotationX(float rotationAngle);
	void SetRotationY(float rotationAngle);
	void SetRotationZ(float rotationAngle);
	void SetScale(const glm::vec3& scale);
	void SetExecutionOrder(int executionOrder) { m_executionOrder = executionOrder; }
	void SetGameObjectName(const std::string& name) { m_name = name; };
	void SetGeometry(GLGeometryRef geometry) { m_geometry = geometry; }
	void SetModel(const std::shared_ptr<GModel>& model) { m_model = model; }

	GLGeometryRef GetGeometry() const;
	int GetExecutationOrder() const { return m_executionOrder; }
	float GetRotationX() const;
	float GetRotationY() const;
	float GetRotationZ() const;
	const glm::vec3& GetPosition() const;
	const glm::vec3& GetScale() const;
	const glm::mat4& GetModelMatrix() const;
	const std::string& GetName() const { return m_name; }
	const std::shared_ptr<GModel>& GetModel() { return m_model; }

	void Translate(const glm::vec3& positionOffset);
	void RotateX(float rotationOffset);
	void RotateY(float rotationOffset);
	void RotateZ(float rotationOffset);
	void Scale(const glm::vec3& scaleOffset);

	void UpdateModel(const GLProgramPipelineRef& program) const;

	std::shared_ptr<AABB> GetAABB() const;
	std::vector<glm::vec3> GetTriangles();
	bool operator<(const IGraphicsObject& otherPass) const;

protected:
	Systems& m_systems;

private:
	void updateModelMatrix();
	void setRotation(float rotationAngleOffset, const glm::vec3& rotateAxis);

	GLGeometryRef m_geometry = nullptr;
	int m_executionOrder = -1;
	std::string m_name;
	glm::vec3 m_position;
	glm::vec3 m_rotationAngle;
	glm::vec3 m_scale = glm::vec3(1.0f);
	glm::mat4 m_modelMatrix;
	glm::mat4 m_translationMatrix;
	glm::mat4 m_rotationMatrix;
	glm::mat4 m_scaleMatrix;
	std::shared_ptr<GModel> m_model;
};