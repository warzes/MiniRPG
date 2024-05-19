template<typename T>
inline GLBufferRef RenderSystem::CreateBuffer(const std::vector<T>& buff, GLenum flags)
{
	auto resource = std::make_shared<GLBuffer>();
	if (!IsValid(resource)) return nullptr;
	glNamedBufferStorage(*resource, sizeof(typename std::vector<T>::value_type) * buff.size(), buff.data(), flags);
	return resource;
}

template<typename T>
inline GLVertexArrayRef RenderSystem::CreateVertexArray(const std::vector<T>& vertices, const std::vector<uint8_t>& indices, const std::vector<AttribFormat>& attribFormats)
{
	GLBufferRef vbo = CreateBuffer(vertices);
	GLBufferRef ibo = CreateBuffer(indices);
	return CreateVertexArray(vbo, sizeof(T), ibo, attribFormats);
}

template<typename T>
inline GLVertexArrayRef RenderSystem::CreateVertexArray(const std::vector<T>& vertices, const std::vector<uint16_t>& indices, const std::vector<AttribFormat>& attribFormats)
{
	GLBufferRef vbo = CreateBuffer(vertices);
	GLBufferRef ibo = CreateBuffer(indices);
	return CreateVertexArray(vbo, sizeof(T), ibo, attribFormats);
}
template<typename T>
inline GLVertexArrayRef RenderSystem::CreateVertexArray(const std::vector<T>& vertices, const std::vector<uint32_t>& indices, const std::vector<AttribFormat>& attribFormats)
{
	GLBufferRef vbo = CreateBuffer(vertices);
	GLBufferRef ibo = CreateBuffer(indices);
	return CreateVertexArray(vbo, sizeof(T), ibo, attribFormats);
}

template<typename T>
inline GLGeometryRef RenderSystem::CreateGeometry(const std::vector<T>& vertices, const std::vector<uint8_t>& indices, const std::vector<AttribFormat>& attribFormats)
{
	auto resource = std::make_shared<GLGeometry>();
	resource->vbo = CreateBuffer(vertices);
	resource->vertexSize = sizeof(T);
	resource->ibo = CreateBuffer(indices);
	resource->indexFormat = IndexFormat::UInt8;
	resource->vao = CreateVertexArray(resource->vbo, resource->vertexSize, resource->ibo, attribFormats);
	return resource;
}

template<typename T>
inline GLGeometryRef RenderSystem::CreateGeometry(const std::vector<T>& vertices, const std::vector<uint16_t>& indices, const std::vector<AttribFormat>& attribFormats)
{
	auto resource = std::make_shared<GLGeometry>();
	resource->vbo = CreateBuffer(vertices);
	resource->vertexSize = sizeof(T);
	resource->ibo = CreateBuffer(indices);
	resource->indexFormat = IndexFormat::UInt16;
	resource->vao = CreateVertexArray(resource->vbo, resource->vertexSize, resource->ibo, attribFormats);
	return resource;
}
template<typename T>
inline GLGeometryRef RenderSystem::CreateGeometry(const std::vector<T>& vertices, const std::vector<uint32_t>& indices, const std::vector<AttribFormat>& attribFormats)
{
	auto resource = std::make_shared<GLGeometry>();
	resource->vbo = CreateBuffer(vertices);
	resource->vertexSize = sizeof(T);
	resource->ibo = CreateBuffer(indices);
	resource->indexFormat = IndexFormat::UInt32;
	resource->vao = CreateVertexArray(resource->vbo, resource->vertexSize, resource->ibo, attribFormats);
	return resource;
}

template<typename T>
inline GLTextureRef RenderSystem::CreateTextureCube(GLenum internalFormat, GLenum format, GLsizei width, GLsizei height, std::array<T*, 6> const& data)
{
	auto resource = CreateTextureCube();
	glTextureStorage2D(*resource, 1, internalFormat, width, height);
	for (GLint i = 0; i < 6; ++i)
	{
		if (data[i])
		{
			glTextureSubImage3D(*resource, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, data[i]);
		}
	}
	// Fix cubemap seams
	glTextureParameteri(*resource, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(*resource, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return resource;
}

template<typename T>
inline void RenderSystem::SetUniform(GLSeparableShaderProgramRef shader, GLint location, const T& value)
{
	if (!IsValid(shader))
	{
		m_systems.log->Error("shader is null");
		return;
	}
	if constexpr (std::is_same_v<T, GLint>) glProgramUniform1i(*shader, location, value);
	else if constexpr (std::is_same_v<T, GLuint>) glProgramUniform1ui(*shader, location, value);
	else if constexpr (std::is_same_v<T, bool>) glProgramUniform1ui(*shader, location, value);
	else if constexpr (std::is_same_v<T, GLfloat>) glProgramUniform1f(*shader, location, value);
	else if constexpr (std::is_same_v<T, GLdouble>) glProgramUniform1d(*shader, location, value);
	else if constexpr (std::is_same_v<T, glm::vec2>) glProgramUniform2fv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::vec3>) glProgramUniform3fv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::vec4>) glProgramUniform4fv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::ivec2>) glProgramUniform2iv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::ivec3>) glProgramUniform3iv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::ivec4>) glProgramUniform4iv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::uvec2>) glProgramUniform2uiv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::uvec3>) glProgramUniform3uiv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::uvec4>) glProgramUniform4uiv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::quat>) glProgramUniform4fv(*shader, location, 1, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::mat3>) glProgramUniformMatrix3fv(*shader, location, 1, GL_FALSE, glm::value_ptr(value));
	else if constexpr (std::is_same_v<T, glm::mat4>) glProgramUniformMatrix4fv(*shader, location, 1, GL_FALSE, glm::value_ptr(value));
	else m_systems.log->Fatal("unsupported type");
}