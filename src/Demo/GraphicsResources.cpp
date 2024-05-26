#include "stdafx.h"
#include "GraphicsResources.h"
namespace
{
	bool floatEqual(float vFloatNum1, float vFloatNum2) // TODO: delete
	{
		return std::abs(vFloatNum1 - vFloatNum2) < 0.00001;
	}
}
//-----------------------------------------------------------------------------
IGraphicsObject::IGraphicsObject(Systems& systems)
	: m_systems(systems)
{
}
//-----------------------------------------------------------------------------
IGraphicsObject::IGraphicsObject(Systems& systems, const std::string& objectName, int executionOrder)
	: m_systems(systems)
	, m_name(objectName)
	, m_executionOrder(executionOrder)
{
}
//-----------------------------------------------------------------------------
void IGraphicsObject::SetPosition(const glm::vec3& position)
{
	if (position != m_position)
	{
		m_translationMatrix = glm::mat4();
		m_translationMatrix = glm::translate(m_translationMatrix, position);
		updateModelMatrix();
		m_position = position;
	}
}
//-----------------------------------------------------------------------------
void IGraphicsObject::SetRotationX(float rotationAngle)
{
	setRotation(rotationAngle - m_rotationAngle[0], glm::vec3(1, 0, 0));
	m_rotationAngle[0] = rotationAngle;
}
//-----------------------------------------------------------------------------
void IGraphicsObject::SetRotationY(float rotationAngle)
{
	setRotation(rotationAngle - m_rotationAngle[1], glm::vec3(0, 1, 0));
	m_rotationAngle[1] = rotationAngle;
}
//-----------------------------------------------------------------------------
void IGraphicsObject::SetRotationZ(float rotationAngle)
{
	setRotation(rotationAngle - m_rotationAngle[2], glm::vec3(0, 0, 1));
	m_rotationAngle[2] = rotationAngle;
}
//-----------------------------------------------------------------------------
void IGraphicsObject::SetScale(const glm::vec3& scale)
{
	if (scale != m_scale)
	{
		m_scaleMatrix = glm::mat4();
		m_scaleMatrix = glm::scale(m_scaleMatrix, scale);
		updateModelMatrix();
		m_scale = scale;
	}
}
//-----------------------------------------------------------------------------
GLGeometryRef IGraphicsObject::GetGeometry() const
{
	return m_geometry;
}
//-----------------------------------------------------------------------------
float IGraphicsObject::GetRotationX() const
{
	return m_rotationAngle[0];
}
//-----------------------------------------------------------------------------
float IGraphicsObject::GetRotationY() const
{
	return m_rotationAngle[1];
}
//-----------------------------------------------------------------------------
float IGraphicsObject::GetRotationZ() const
{
	return m_rotationAngle[2];
}
//-----------------------------------------------------------------------------
const glm::vec3& IGraphicsObject::GetPosition() const
{
	return m_position;
}
//-----------------------------------------------------------------------------
const glm::vec3& IGraphicsObject::GetScale() const
{
	return m_scale;
}
//-----------------------------------------------------------------------------
const glm::mat4& IGraphicsObject::GetModelMatrix() const
{
	return m_modelMatrix;
}
//-----------------------------------------------------------------------------
void IGraphicsObject::Translate(const glm::vec3& positionOffset)
{
	if (positionOffset != glm::vec3(0))
	{
		m_translationMatrix = glm::translate(m_translationMatrix, positionOffset);
		updateModelMatrix();
		m_position += positionOffset;
	}
}
//-----------------------------------------------------------------------------
void IGraphicsObject::RotateX(float rotationOffset)
{
	setRotation(rotationOffset, glm::vec3(1, 0, 0));
	m_rotationAngle[0] += rotationOffset;
}
//-----------------------------------------------------------------------------
void IGraphicsObject::RotateY(float rotationOffset)
{
	setRotation(rotationOffset, glm::vec3(0, 1, 0));
	m_rotationAngle[1] += rotationOffset;
}
//-----------------------------------------------------------------------------
void IGraphicsObject::RotateZ(float rotationOffset)
{
	setRotation(rotationOffset, glm::vec3(0, 0, 1));
	m_rotationAngle[2] += rotationOffset;
}
//-----------------------------------------------------------------------------
void IGraphicsObject::Scale(const glm::vec3& scaleOffset)
{
	if (scaleOffset != glm::vec3(0))
	{
		m_scaleMatrix = glm::scale(m_scaleMatrix, scaleOffset);
		updateModelMatrix();
		m_scale *= scaleOffset;
	}
}
//-----------------------------------------------------------------------------
void IGraphicsObject::UpdateModel(const GLProgramPipelineRef& program) const
{
	m_model->Update(program);
}
//-----------------------------------------------------------------------------
std::shared_ptr<AABB> IGraphicsObject::GetAABB() const
{
	return m_model->GetBounding();
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> IGraphicsObject::GetTriangles()
{
	return m_model->GetTriangles();
}
//-----------------------------------------------------------------------------
bool IGraphicsObject::operator<(const IGraphicsObject& otherPass) const
{
	return m_executionOrder < otherPass.GetExecutationOrder();
}
//-----------------------------------------------------------------------------
void IGraphicsObject::updateModelMatrix()
{
	m_modelMatrix = m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
}
//-----------------------------------------------------------------------------
void IGraphicsObject::setRotation(float rotationAngleOffset, const glm::vec3& rotateAxis)
{
	if (!floatEqual(rotationAngleOffset, 0.0))
	{
		glm::mat4 TempRotationMatrix;
		TempRotationMatrix = glm::rotate(TempRotationMatrix, glm::radians(rotationAngleOffset), rotateAxis);
		m_rotationMatrix = TempRotationMatrix * m_rotationMatrix;
		updateModelMatrix();
	}
}
//-----------------------------------------------------------------------------