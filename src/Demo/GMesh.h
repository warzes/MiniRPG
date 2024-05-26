#pragma once

// TODO: в систему рендера
template<typename T>
constexpr std::pair<GLint, GLenum> TypeToSizeEnum()
{
	if constexpr (std::is_same_v<T, float>) return std::make_pair(1, GL_FLOAT);
	if constexpr (std::is_same_v<T, int>) return std::make_pair(1, GL_INT);
	if constexpr (std::is_same_v<T, unsigned int>) return std::make_pair(1, GL_UNSIGNED_INT);
	if constexpr (std::is_same_v<T, glm::vec2>) return std::make_pair(2, GL_FLOAT);
	if constexpr (std::is_same_v<T, glm::vec3>) return std::make_pair(3, GL_FLOAT);
	if constexpr (std::is_same_v<T, glm::vec4>) return std::make_pair(4, GL_FLOAT);
	LogFatal("unsupported type");
}

// TODO: в систему рендера
template<typename T>
constexpr inline AttribFormat CreateAttribFormat(GLuint attribIndex, GLuint relativeOffset)
{
	const auto [compCount, type] = TypeToSizeEnum<T>();
	return AttribFormat{ attribIndex, compCount, type, relativeOffset };
}

// TODO: retype -> constexpr
#define MESH_ID        "u_MeshId"
#define SHININESS      "u_Shininess"
#define REFRACTI       "u_Refracti"
#define AMBIENT_COLOR  "u_AmbientColor"
#define DIFFUSE_COLOR  "u_DiffuseColor"
#define SPECULAR_COLOR "u_SpecularColor"
#define DIFFUSE_TEX    "u_DiffuseTexture"
#define SPECULAR_TEX   "u_SpecularTexture"
#define NORMAL_TEX     "u_NormalTexture"
#define ROUGHNESS_TEX  "u_RoughnessTexture"
#define METALLIC_TEX   "u_MetallicTexture"

struct GMeshVertex final
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
};

struct GMeshTexture final
{
	GLTextureRef texture = nullptr;
	std::string texturePath;
	std::string textureUniformName;
};

struct GMeshMatProperties final
{
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess = 0.0f;
	float refracti = 0.0f;
};

class GMesh final
{
public:
	GMesh(Systems& systems);
	GMesh(Systems& systems, const std::vector<GMeshVertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<GMeshTexture>& textures, const GMeshMatProperties& meshMatProperties, int meshId);

	void Update(const GLProgramPipelineRef& program) const;

	std::shared_ptr<AABB> GetBounding();
	std::vector<glm::vec3> GetTriangle();

private:
	void init();
	Systems& m_systems;

	std::vector<GMeshVertex> m_vertices;
	std::vector<GMeshTexture> m_textures;
	std::vector<uint32_t> m_indices;
	std::shared_ptr<AABB> m_bounding;
	GMeshMatProperties m_meshMatProperties;
	GLGeometryRef m_geometry = nullptr;
	int m_ambientColorLoc = -1;
	int m_diffuseColorLoc = -1;
	int m_specularColorLoc = -1;
	int m_meshId = -1;

};