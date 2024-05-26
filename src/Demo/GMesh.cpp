#include "stdafx.h"
#include "GMesh.h"
//-----------------------------------------------------------------------------
GMesh::GMesh(Systems& systems)
	: m_systems(systems)
{
}
//-----------------------------------------------------------------------------
GMesh::GMesh(Systems& systems, const std::vector<GMeshVertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<GMeshTexture>& textures, const GMeshMatProperties& meshMatProperties, int meshId)
	: m_systems(systems)
	, m_vertices(vertices)
	, m_indices(indices)
	, m_textures(textures)
	, m_meshMatProperties(meshMatProperties)
	, m_meshId(meshId)
{
	init();
}
//-----------------------------------------------------------------------------
void GMesh::Update(const GLProgramPipelineRef& program) const
{
	if (m_textures.size() > 0 && m_textures[0].texture)
	{
		for (size_t i = 0; i < m_textures.size(); i++)
		{
			if (m_textures[i].texture)
				m_systems.render->BindSlot(m_textures[i].texture, i);
		}
	}
	else
	{
		int ambientColorLoc = glGetUniformLocation(*program->GetFragmentShader(), AMBIENT_COLOR);
		if (ambientColorLoc >= 0)
			m_systems.render->SetUniform(program->GetFragmentShader(), ambientColorLoc, m_meshMatProperties.ambientColor);

		int diffuseColorLoc = glGetUniformLocation(*program->GetFragmentShader(), DIFFUSE_COLOR);
		if (diffuseColorLoc >= 0)
			m_systems.render->SetUniform(program->GetFragmentShader(), diffuseColorLoc, m_meshMatProperties.diffuseColor);

		int specularColorLoc = glGetUniformLocation(*program->GetFragmentShader(), SPECULAR_COLOR);
		if (specularColorLoc >= 0)
			m_systems.render->SetUniform(program->GetFragmentShader(), specularColorLoc, m_meshMatProperties.specularColor);

		int shininessLoc = glGetUniformLocation(*program->GetFragmentShader(), SHININESS);
		if (shininessLoc >= 0)
			m_systems.render->SetUniform(program->GetFragmentShader(), shininessLoc, m_meshMatProperties.shininess);

		int refractiLoc = glGetUniformLocation(*program->GetFragmentShader(), REFRACTI);
		if (refractiLoc >= 0)
			m_systems.render->SetUniform(program->GetFragmentShader(), refractiLoc, m_meshMatProperties.refracti);
	}

	int meshIdLoc = glGetUniformLocation(*program->GetVertexShader(), MESH_ID);
	if (meshIdLoc)
		m_systems.render->SetUniform(program->GetFragmentShader(), meshIdLoc, m_meshId);

	m_systems.render->Bind(m_geometry);
	glDrawElements(GL_TRIANGLES, static_cast<int>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
}
//-----------------------------------------------------------------------------
std::shared_ptr<AABB> GMesh::GetBounding()
{
	if (!m_bounding)
	{
		std::vector<glm::vec3> points;
		for (size_t i = 0; i < m_vertices.size(); i++)
		{
			points.push_back(m_vertices[i].position);
		}
		m_bounding = std::make_shared<AABB>(points);
	}
	return m_bounding;
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> GMesh::GetTriangle()
{
	std::vector<glm::vec3> triangles;
	for (size_t i = 0; i < m_vertices.size(); i++)
		triangles.push_back(m_vertices[i].position);
	return triangles;
}
//-----------------------------------------------------------------------------
void GMesh::init()
{
	const std::vector<AttribFormat> vertexFormat =
	{
		CreateAttribFormat<glm::vec3>(0, offsetof(GMeshVertex, position)),
		CreateAttribFormat<glm::vec3>(1, offsetof(GMeshVertex, color)),
		CreateAttribFormat<glm::vec3>(2, offsetof(GMeshVertex, normal)),
		CreateAttribFormat<glm::vec3>(3, offsetof(GMeshVertex, texCoords)),
		CreateAttribFormat<glm::vec2>(4, offsetof(GMeshVertex, tangent))
	};
	m_geometry = m_systems.render->CreateGeometry(m_vertices, m_indices, vertexFormat);
}
//-----------------------------------------------------------------------------