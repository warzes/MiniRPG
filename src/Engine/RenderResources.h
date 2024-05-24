#pragma once

#include "RenderCore.h"

// ref ARB_separate_shader_objects 
class GLSeparableShaderProgram final
{
public:
	GLSeparableShaderProgram() = delete;
	GLSeparableShaderProgram(GLenum shaderType, std::string_view sourceCode);
	GLSeparableShaderProgram(const GLSeparableShaderProgram&) = delete;
	GLSeparableShaderProgram(GLSeparableShaderProgram&& other) noexcept;
	~GLSeparableShaderProgram();
	GLSeparableShaderProgram& operator=(const GLSeparableShaderProgram&) = delete;
	GLSeparableShaderProgram& operator=(GLSeparableShaderProgram&& other) noexcept;

	operator GLuint() const noexcept { return m_handle; }
	bool IsValid() const noexcept { return m_handle != 0; }

private:
	void createHandle(GLenum shaderType, std::string_view sourceCode);
	void destroyHandle();
	void validate();

	GLuint m_handle = 0;
};
using GLSeparableShaderProgramRef = std::shared_ptr<GLSeparableShaderProgram>;

class GLProgramPipeline final
{
	friend class RenderSystem;
public:
	GLProgramPipeline() { createHandle(); }
	GLProgramPipeline(const GLProgramPipeline&) = delete;
	GLProgramPipeline(GLProgramPipeline&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLProgramPipeline() { destroyHandle(); }
	GLProgramPipeline& operator=(const GLProgramPipeline&) = delete;
	GLProgramPipeline& operator=(GLProgramPipeline&& other) noexcept;

	operator GLuint() const noexcept { return m_handle; }
	bool IsValid() const noexcept { return m_handle != 0; }

	GLSeparableShaderProgramRef GetVertexShader() { return m_vertexShader; }
	GLSeparableShaderProgramRef GetFragmentShader() { return m_fragmentShader; }

private:
	void createHandle();
	void destroyHandle();

	GLuint m_handle = 0;
	GLSeparableShaderProgramRef m_vertexShader = nullptr;
	GLSeparableShaderProgramRef m_fragmentShader = nullptr;

};
using GLProgramPipelineRef = std::shared_ptr<GLProgramPipeline>;

class GLBuffer final
{
public:
	GLBuffer() { createHandle(); }
	GLBuffer(const GLBuffer&) = delete;
	GLBuffer(GLBuffer&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLBuffer() { destroyHandle(); }
	GLBuffer& operator=(const GLBuffer&) = delete;
	GLBuffer& operator=(GLBuffer&& other) noexcept;

	operator GLuint() const noexcept { return m_handle; }
	bool IsValid() const noexcept { return m_handle != 0; }

	[[nodiscard]] GLsizeiptr Size() const;
	[[nodiscard]] GLenum Usage() const;
	[[nodiscard]] GLenum Access() const;
	[[nodiscard]] GLbitfield AccessFlags() const;
	[[nodiscard]] bool IsMapped() const;
	[[nodiscard]] bool IsImmutable() const;
	[[nodiscard]] GLbitfield StorageFlags() const;
	[[nodiscard]] GLintptr MapOffset() const;
	[[nodiscard]] GLsizeiptr MapSize() const;
	[[nodiscard]] void* MapPointer() const;

private:
	void createHandle();
	void destroyHandle();

	[[nodiscard]] GLint getParameter(const GLenum parameter) const;
	[[nodiscard]] GLint getParameter64(const GLenum parameter) const;

	GLuint m_handle = 0;
};
using GLBufferRef = std::shared_ptr<GLBuffer>;

class GLVertexArray final
{
public:
	GLVertexArray() { createHandle(); }
	GLVertexArray(const GLVertexArray&) = delete;
	GLVertexArray(GLVertexArray&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLVertexArray() { destroyHandle(); }
	GLVertexArray& operator=(const GLVertexArray&) = delete;
	GLVertexArray& operator=(GLVertexArray&& other) noexcept;

	operator GLuint() const noexcept { return m_handle; }
	bool IsValid() const noexcept { return m_handle != 0; }

private:
	void createHandle();
	void destroyHandle();

	GLuint m_handle = 0;
};
using GLVertexArrayRef = std::shared_ptr<GLVertexArray>;

class GLGeometry final
{
public:
	GLGeometry() = default;
	GLGeometry(const GLGeometry&) = delete;
	GLGeometry(GLGeometry&&) noexcept = default;
	GLGeometry& operator=(const GLGeometry&) = delete;
	GLGeometry& operator=(GLGeometry&&) noexcept = default;

	GLBufferRef vbo = nullptr;
	GLBufferRef ibo = nullptr;
	GLVertexArrayRef vao = nullptr;

	size_t vertexSize = 0;
	IndexFormat indexFormat = IndexFormat::UInt8;
};
using GLGeometryRef = std::shared_ptr<GLGeometry>;

class GLTexture final
{
	friend class RenderSystem;
public:
	GLTexture() = delete;
	GLTexture(GLenum target) { createHandle(target); }
	GLTexture(const GLTexture&) = delete;
	GLTexture(GLTexture&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLTexture() { destroyHandle(); }
	GLTexture& operator=(const GLTexture&) = delete;
	GLTexture& operator=(GLTexture&& other) noexcept;

	operator GLuint() const noexcept { return m_handle; }
	bool IsValid() const noexcept { return m_handle != 0; }

	GLenum GetTarget() const noexcept { return m_target; }

private:
	void createHandle(GLenum target);
	void destroyHandle();

	GLuint m_handle = 0;
	GLenum m_target = 0;
};
using GLTextureRef = std::shared_ptr<GLTexture>;

class GLFramebuffer final
{
	friend class RenderSystem;
public:
	GLFramebuffer() { createHandle(); }
	GLFramebuffer(const GLFramebuffer&) = delete;
	GLFramebuffer(GLFramebuffer&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLFramebuffer() { destroyHandle(); }
	GLFramebuffer& operator=(const GLFramebuffer&) = delete;
	GLFramebuffer& operator=(GLFramebuffer&& other) noexcept;

	operator GLuint() const noexcept { return m_handle; }
	bool IsValid() const noexcept { return m_handle != 0; }

private:
	void createHandle();
	void destroyHandle();

	GLuint m_handle = 0;
	std::vector<GLTextureRef> m_colorTextures;
	GLTextureRef m_depthTexture = nullptr;

};
using GLFramebufferRef = std::shared_ptr<GLFramebuffer>;