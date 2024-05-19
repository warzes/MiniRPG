#pragma once

#include "Systems.h"
#include "Image.h"
#include "RenderResources.h"

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

	GLSeparableShaderProgramRef CreateProgramObject(GLenum shaderType, std::string_view sourceCode);
	GLProgramPipelineRef CreateProgramPipeline();
	GLProgramPipelineRef CreateProgramPipeline(GLSeparableShaderProgramRef vertexShader, GLSeparableShaderProgramRef fragmentShader);
	GLProgramPipelineRef CreateProgramPipelineFromSources(std::string_view vertSource, std::string_view fragSource);
	GLProgramPipelineRef CreateProgramPipelineFromFiles(std::string_view vertFilepath, std::string_view fragFilepath);

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
	// Modify Resource
	//-------------------------------------------------------------------------
	template <typename T>
	void SetUniform(GLSeparableShaderProgramRef shader, GLint location, const T& value);

	void ProgramPipelineSetSeparableShaders(GLProgramPipelineRef pipeline, GLSeparableShaderProgramRef vertexShader, GLSeparableShaderProgramRef fragmentShader);

	void VertexArraySetAttribFormats(GLVertexArrayRef vao, const std::vector<AttribFormat>& attribFormats);
	void VertexArraySetVertexBuffer(GLVertexArrayRef vao, GLBufferRef vbo, size_t vertexSize);
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