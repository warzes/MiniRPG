#pragma once

#include "Systems.h"
#include "Image.h"
#include "RenderResources.h"

// TODO: сейчас сеттеры пишутся так ResourceSetMode. Может переделать так: SetMode?

constexpr int MaxBindingTextures = 16;

struct RenderSystemCreateInfo final
{
	glm::vec3 clearColor = glm::vec3{ 0.2f, 0.4f, 0.9f };
};

class RenderSystem final
{
public:
	RenderSystem(Systems& systems);
	~RenderSystem();

	bool Create(const RenderSystemCreateInfo& createInfo);
	void Destroy();

	void BeginFrame();
	void EndFrame();

	//-------------------------------------------------------------------------
	// Create Resource
	//-------------------------------------------------------------------------

	GLSeparableShaderProgramRef CreateSeparableShaderProgram(GLenum shaderType, std::string_view sourceCode);

	GLProgramPipelineRef CreateProgramPipeline();
	GLProgramPipelineRef CreateProgramPipeline(GLSeparableShaderProgramRef vertexShader, GLSeparableShaderProgramRef fragmentShader);
	GLProgramPipelineRef CreateProgramPipelineFromSources(std::string_view vertSource, std::string_view fragSource);
	GLProgramPipelineRef CreateProgramPipelineFromFiles(std::string_view vertFilepath, std::string_view fragFilepath);

	GLBufferRef CreateBuffer();
	template<typename T>
	GLBufferRef CreateBuffer(const std::vector<T>& buff, GLenum flags = GL_DYNAMIC_STORAGE_BIT);

	GLVertexArrayRef CreateVertexArray();
	GLVertexArrayRef CreateVertexArray(const std::vector<AttribFormat>& attribFormats);
	GLVertexArrayRef CreateVertexArray(GLBufferRef vbo, size_t vertexSize, const std::vector<AttribFormat>& attribFormats);
	GLVertexArrayRef CreateVertexArray(GLBufferRef vbo, size_t vertexSize, GLBufferRef ibo, const std::vector<AttribFormat>& attribFormats);
	template<typename T>
	GLVertexArrayRef CreateVertexArray(const std::vector<T>& vertices, const std::vector<uint8_t>& indices, const std::vector<AttribFormat>& attribFormats);
	template<typename T>
	GLVertexArrayRef CreateVertexArray(const std::vector<T>& vertices, const std::vector<uint16_t>& indices, const std::vector<AttribFormat>& attribFormats);
	template<typename T>
	GLVertexArrayRef CreateVertexArray(const std::vector<T>& vertices, const std::vector<uint32_t>& indices, const std::vector<AttribFormat>& attribFormats);

	template<typename T>
	GLGeometryRef CreateGeometry(const std::vector<T>& vertices, const std::vector<uint8_t>& indices, const std::vector<AttribFormat>& attribFormats);
	template<typename T>
	GLGeometryRef CreateGeometry(const std::vector<T>& vertices, const std::vector<uint16_t>& indices, const std::vector<AttribFormat>& attribFormats);
	template<typename T>
	GLGeometryRef CreateGeometry(const std::vector<T>& vertices, const std::vector<uint32_t>& indices, const std::vector<AttribFormat>& attribFormats);

	GLTextureRef CreateTexture2D();
	GLTextureRef CreateTexture2D(GLenum internalFormat, GLenum format, GLsizei width, GLsizei height, void* data = nullptr, GLenum filter = GL_LINEAR, GLenum repeat = GL_REPEAT, bool generateMipMaps = false);
	GLTextureRef CreateTexture2DFromFile(std::string_view filepath, int comp = STBI_rgb_alpha, bool generateMipMaps = false);

	GLTextureRef CreateTextureCube();
	template<typename T = nullptr_t>
	GLTextureRef CreateTextureCube(GLenum internalFormat, GLenum format, GLsizei width, GLsizei height, std::array<T*, 6> const& data);
	GLTextureRef CreateTextureCubeFromFile(std::array<std::string_view, 6> const& filepath, int comp = STBI_rgb_alpha);

	GLFramebufferRef CreateFramebuffer();
	GLFramebufferRef CreateFramebuffer(const std::vector<GLTextureRef>& cols, GLTextureRef depth = nullptr);

	//-------------------------------------------------------------------------
	// Validation Resource
	//-------------------------------------------------------------------------
	inline bool IsValid(GLSeparableShaderProgramRef resource) const noexcept { return resource && resource->IsValid(); }
	inline bool IsValid(GLProgramPipelineRef resource) const noexcept { return resource && resource->IsValid(); }
	inline bool IsValid(GLBufferRef resource) const noexcept { return resource && resource->IsValid(); }
	inline bool IsValid(GLVertexArrayRef resource) const noexcept { return resource && resource->IsValid(); }
	inline bool IsValid(GLGeometryRef resource) const noexcept { return resource && IsValid(resource->vao); }
	inline bool IsValid(GLTextureRef resource) const noexcept { return resource && resource->IsValid(); }
	inline bool IsValid(GLFramebufferRef resource) const noexcept { return resource && resource->IsValid(); }

	//-------------------------------------------------------------------------
	// Modify Resource
	//-------------------------------------------------------------------------
	template <typename T>
	void SetUniform(GLSeparableShaderProgramRef shader, GLint location, const T& value);

	void ProgramPipelineSetSeparableShaders(GLProgramPipelineRef pipeline, GLSeparableShaderProgramRef vertexShader, GLSeparableShaderProgramRef fragmentShader);

	// внимание: можно вызвать только один раз и только для пустого буфера (проверить), также нельзя использовать BufferSetData(). в будущем сделать проверку. переделать этот комментарий под общий для трех функций
	template<typename T>
	void SetDataImmutable(GLBufferRef buffer, const std::vector<T>& buff, GLenum flags = GL_DYNAMIC_STORAGE_BIT);
	template<typename T>
	void SetData(GLBufferRef buffer, const std::vector<T>& buff, GLenum usage = GL_DYNAMIC_DRAW);
	template<typename T>
	void SetSubData(GLBufferRef buffer, GLintptr offset, const std::vector<T>& buff);

	void ClearSubData(GLBufferRef buffer, const GLenum internalFormat, const GLintptr offset, const GLsizeiptr size, const GLenum format, const GLenum data_type, const void* data = nullptr);
	void ClearData(GLBufferRef buffer, const GLenum internalFormat, const GLenum format, const GLenum data_type, const void* data = nullptr);

	[[nodiscard]] void* MapRange(GLBufferRef buffer, const GLintptr offset, const GLsizeiptr size, const GLbitfield accessFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT) const;
	[[nodiscard]] void* Map(GLBufferRef buffer, const GLenum access = GL_READ_WRITE) const;
	void FlushMappedRange(GLBufferRef buffer, const GLintptr offset, const GLsizeiptr size) const;
	void Unmap(GLBufferRef buffer) const;

	void InvalidateSubData(GLBufferRef buffer, const GLintptr offset, const GLsizeiptr size) const;
	void Invalidate(GLBufferRef buffer) const;

	void CopySubData(GLBufferRef source, GLBufferRef dest, const GLintptr sourceOffset, const GLintptr offset, const GLsizeiptr size) const;

	void VertexArraySetAttribFormats(GLVertexArrayRef vao, const std::vector<AttribFormat>& attribFormats);
	void VertexArraySetVertexBuffer(GLVertexArrayRef vao, GLBufferRef vbo, size_t vertexSize);
	void VertexArraySetVertexBuffer(GLVertexArrayRef vao, const GLuint bindingIndex, GLBufferRef vbo, const GLintptr offset = 0, const GLsizei stride = 1);
	void VertexArraySetIndexBuffer(GLVertexArrayRef vao, GLBufferRef ibo);

	void FramebufferSetTextures(GLFramebufferRef framebuffer, const std::vector<GLTextureRef>& cols, GLTextureRef depth = nullptr);
	void FramebufferClear(GLFramebufferRef framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);

	//-------------------------------------------------------------------------
	// Bind Resource
	//-------------------------------------------------------------------------
	void Bind(GLProgramPipelineRef pipeline);
	void Bind(GLVertexArrayRef vao);
	void Bind(GLGeometryRef geom);
	void BindSlot(GLTextureRef texture, GLuint unit);
	void Bind(GLFramebufferRef framebuffer);

	void MainFrameBuffer();
	void BlitFrameBuffer(GLFramebufferRef readFramebuffer, GLFramebufferRef drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

private:
	Systems& m_systems;

	RenderSystem(RenderSystem&&) = delete;
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;

	void initializeExtensions(bool print);
	void initializeCapabilities(bool print);

	std::unordered_map<std::string, GLTextureRef> m_cacheFileTextures2D;
};

#include "RenderSystem.inl"