#pragma once

#include "RenderCore.h"
#include "nRenderCore.h"

class GLProgramObject final
{
public:
	GLProgramObject() = delete;
	GLProgramObject(GLenum type, std::string_view source) { createHandle(type, source); }
	GLProgramObject(const GLProgramObject&) = delete;
	GLProgramObject(GLProgramObject&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLProgramObject() { destroyHandle(); }
	GLProgramObject& operator=(const GLProgramObject&) = delete;
	GLProgramObject& operator=(GLProgramObject&& other) noexcept
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;

		return *this;
	};

	operator GLuint() const noexcept { return m_handle; }
	operator bool() const noexcept { return m_handle != 0; }

private:
	void createHandle(GLenum type, std::string_view source);
	void destroyHandle();
	void validate();

	GLuint m_handle = 0;
};
using GLProgramObjectRef = std::shared_ptr<GLProgramObject>;

class GLProgramPipeline final
{
	friend class RenderSystem;
public:
	GLProgramPipeline() { createHandle(); }
	GLProgramPipeline(const GLProgramPipeline&) = delete;
	GLProgramPipeline(GLProgramPipeline&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLProgramPipeline() { destroyHandle(); }
	GLProgramPipeline& operator=(const GLProgramPipeline&) = delete;
	GLProgramPipeline& operator=(GLProgramPipeline&& other) noexcept
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;

		return *this;
	};

	operator GLuint() const noexcept { return m_handle; }
	operator bool() const noexcept { return m_handle != 0; }

	GLProgramObjectRef GetVertexShader() { return m_vertexShader; }
	GLProgramObjectRef GetFragmentShader() { return m_fragmentShader; }

private:
	void createHandle()
	{
		glCreateProgramPipelines(1, &m_handle);
		assert(m_handle);
	}
	void destroyHandle()
	{
		if (m_handle != 0)
			glDeleteProgramPipelines(1, &m_handle);
		m_handle = 0;

		m_vertexShader.reset();
		m_fragmentShader.reset();
	}

	GLuint m_handle = 0;
	GLProgramObjectRef m_vertexShader = nullptr;
	GLProgramObjectRef m_fragmentShader = nullptr;

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
	GLBuffer& operator=(GLBuffer&& other) noexcept
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;

		return *this;
	};

	operator GLuint() const noexcept { return m_handle; }
	operator bool() const noexcept { return m_handle != 0; }

private:
	void createHandle()
	{
		glCreateBuffers(1, &m_handle);
		assert(m_handle);
	}
	void destroyHandle()
	{
		if (m_handle != 0)
			glDeleteBuffers(1, &m_handle);
		m_handle = 0;
	}
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
	GLVertexArray& operator=(GLVertexArray&& other) noexcept
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;

		return *this;
	};

	operator GLuint() const noexcept { return m_handle; }
	operator bool() const noexcept { return m_handle != 0; }

private:
	void createHandle()
	{
		glCreateVertexArrays(1, &m_handle);
		assert(m_handle);
	}
	void destroyHandle()
	{
		if (m_handle != 0)
			glDeleteVertexArrays(1, &m_handle);
		m_handle = 0;
	}
	GLuint m_handle = 0;
};
using GLVertexArrayRef = std::shared_ptr<GLVertexArray>;

class GLGeometry final
{
public:
	GLBufferRef vbo = nullptr;
	GLBufferRef ibo = nullptr;
	GLVertexArrayRef vao = nullptr;

	size_t vertexSize = 0;
	IndexFormat indexFormat = IndexFormat::UInt8;
};
using GLGeometryRef = std::shared_ptr<GLGeometry>;

class GLTexture final
{
public:
	GLTexture() = delete;
	GLTexture(GLenum target) { createHandle(target); }
	GLTexture(const GLTexture&) = delete;
	GLTexture(GLTexture&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLTexture() { destroyHandle(); }
	GLTexture& operator=(const GLTexture&) = delete;
	GLTexture& operator=(GLTexture&& other) noexcept
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;

		return *this;
	};

	operator GLuint() const noexcept { return m_handle; }
	operator bool() const noexcept { return m_handle != 0; }

	GLenum GetTarget() const noexcept { return m_target; }

private:
	void createHandle(GLenum target)
	{
		glCreateTextures(target, 1, &m_handle);
		assert(m_handle);
	}
	void destroyHandle()
	{
		if (m_handle != 0)
			glDeleteTextures(1, &m_handle);
		m_handle = 0;
	}
	GLuint m_handle = 0;
	GLenum m_target = 0;
};
using GLTextureRef = std::shared_ptr<GLTexture>;

class GLFramebuffer final
{
public:
	GLFramebuffer() { createHandle(); }
	GLFramebuffer(const GLFramebuffer&) = delete;
	GLFramebuffer(GLFramebuffer&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLFramebuffer() { destroyHandle(); }
	GLFramebuffer& operator=(const GLFramebuffer&) = delete;
	GLFramebuffer& operator=(GLFramebuffer&& other) noexcept
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;

		return *this;
	};

	operator GLuint() const noexcept { return m_handle; }
	operator bool() const noexcept { return m_handle != 0; }

	GLenum GetTarget() const noexcept { return m_target; }

private:
	void createHandle()
	{
		glCreateFramebuffers(1, &m_handle);
		assert(m_handle);
	}
	void destroyHandle()
	{
		if (m_handle != 0)
			glDeleteFramebuffers(1, &m_handle);
		m_handle = 0;
	}
	GLuint m_handle = 0;
	GLenum m_target = 0;
};
using GLFramebufferRef = std::shared_ptr<GLFramebuffer>;