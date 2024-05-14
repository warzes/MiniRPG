#pragma once

struct vertex_t
{
	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 color = glm::vec3(0.f);
	glm::vec3 normal = glm::vec3(0.f);
	glm::vec2 texcoord = glm::vec2(0.f);

	vertex_t(glm::vec3 const& position, glm::vec3 const& color, glm::vec3 const& normal, glm::vec2 const& texcoord)
		: position(position), color(color), normal(normal), texcoord(texcoord) {}
};

template<typename T>
constexpr std::pair<GLint, GLenum> type_to_size_enum()
{
	if constexpr (std::is_same_v<T, float>)
		return std::make_pair(1, GL_FLOAT);
	if constexpr (std::is_same_v<T, int>)
		return std::make_pair(1, GL_INT);
	if constexpr (std::is_same_v<T, unsigned int>)
		return std::make_pair(1, GL_UNSIGNED_INT);
	if constexpr (std::is_same_v<T, glm::vec2>)
		return std::make_pair(2, GL_FLOAT);
	if constexpr (std::is_same_v<T, glm::vec3>)
		return std::make_pair(3, GL_FLOAT);
	if constexpr (std::is_same_v<T, glm::vec4>)
		return std::make_pair(4, GL_FLOAT);
	throw std::runtime_error("unsupported type");
}

template<typename T>
inline AttribFormat create_attrib_format(GLuint attrib_index, GLuint relative_offset)
{
	auto const [comp_count, type] = type_to_size_enum<T>();
	return AttribFormat{ attrib_index, comp_count, type, relative_offset };
}

std::vector<glm::vec3> calc_tangents(std::vector<vertex_t> const& vertices, std::vector<uint8_t> const& indices);