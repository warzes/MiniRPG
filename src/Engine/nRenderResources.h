#pragma once

#include "RenderCore.h"
#include "nRenderCore.h"

template<typename T>
class GLObject
{
public:
	GLObject() : m_handle(createHandle()) {}
	GLObject(const GLObject&) = delete;
	GLObject(GLObject&& other) noexcept : m_handle{ other.m_handle } { other.m_handle = 0; }
	~GLObject() { destroyHandle(m_handle); }
	GLObject& operator=(const GLObject&) = delete;
	GLObject& operator=(GLObject&& other) noexcept
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;

		return *this;
	};

	operator GLuint() const noexcept { return m_handle; }
	operator bool() const noexcept { return m_handle != 0; }

private:
	static GLuint createHandle()
	{
		GLuint handle;
		T::Create(1, &handle);
		return handle;
	}
	static void destroyHandle(GLuint& handle)
	{
		if (handle != 0)
			T::Destroy(1, &handle);
		handle = 0;
	}
	GLuint m_handle = 0;
};

class GLBufferHelper
{
public:
	static void Create(GLsizei count, GLuint* handles)
	{
		glCreateBuffers(count, handles);
	}
	static void Destroy(GLsizei count, GLuint* handles)
	{
		glDeleteBuffers(count, handles);
	}
};

using GLBuffer = GLObject<GLBufferHelper>;
using GLBufferRef = std::shared_ptr<GLBuffer>;

class GLVertexArrayHelper
{
public:
	static void Create(GLsizei count, GLuint* handles)
	{
		glCreateVertexArrays(count, handles);
	}
	static void Destroy(GLsizei count, GLuint* handles)
	{
		glDeleteVertexArrays(count, handles);
	}
};

using GLVertexArray = GLObject<GLVertexArrayHelper>;
using GLVertexArrayRef = std::shared_ptr<GLVertexArray>;

class GLGeometry
{
public:
	GLBufferRef vbo = nullptr;
	GLBufferRef ibo = nullptr;
	GLVertexArrayRef vao = nullptr;

	size_t vertexSize = 0;
	IndexFormat indexFormat = IndexFormat::UInt8;
};

using GLGeometryRef = std::shared_ptr<GLGeometry>;