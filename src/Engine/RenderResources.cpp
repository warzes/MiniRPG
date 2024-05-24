#include "stdafx.h"
#include "RenderResources.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
GLSeparableShaderProgram::GLSeparableShaderProgram(GLenum shaderType, std::string_view sourceCode)
{ 
	createHandle(shaderType, sourceCode); 
}
//-----------------------------------------------------------------------------
GLSeparableShaderProgram::GLSeparableShaderProgram(GLSeparableShaderProgram&& other) noexcept
	: m_handle{ other.m_handle } 
{ 
	other.m_handle = 0; 
}
//-----------------------------------------------------------------------------
GLSeparableShaderProgram::~GLSeparableShaderProgram()
{ 
	destroyHandle();
}
//-----------------------------------------------------------------------------
GLSeparableShaderProgram& GLSeparableShaderProgram::operator=(GLSeparableShaderProgram&& other) noexcept
{
	if (this != &other)
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;
	}
	return *this;
};
//-----------------------------------------------------------------------------
void GLSeparableShaderProgram::createHandle(GLenum shaderType, std::string_view sourceCode)
{
	const auto srcPtr = sourceCode.data();
	m_handle = glCreateShaderProgramv(shaderType, 1, &srcPtr);
	glProgramParameteri(m_handle, GL_PROGRAM_SEPARABLE, GL_TRUE);
	validate();
}
//-----------------------------------------------------------------------------
void GLSeparableShaderProgram::destroyHandle()
{
	if (m_handle != 0)
		glDeleteProgram(m_handle);
	m_handle = 0;
}
//-----------------------------------------------------------------------------
void GLSeparableShaderProgram::validate()
{
	GLint compiled = 0;
	glGetProgramiv(m_handle, GL_LINK_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		std::array<char, 1024> compiler_log;
		glGetProgramInfoLog(m_handle, compiler_log.size(), nullptr, compiler_log.data());
		glDeleteShader(m_handle);

		std::ostringstream message;
		message << "shader contains error(s):\n\n" << compiler_log.data() << '\n';
		LogError(message.str());
		glDeleteProgram(m_handle);
		m_handle = 0;
	}
}
//-----------------------------------------------------------------------------
GLProgramPipeline& GLProgramPipeline::operator=(GLProgramPipeline&& other) noexcept
{
	if (this != &other)
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;
	}
	return *this;
};
//-----------------------------------------------------------------------------
void GLProgramPipeline::createHandle()
{
	glCreateProgramPipelines(1, &m_handle);
}
//-----------------------------------------------------------------------------
void GLProgramPipeline::destroyHandle()
{
	if (m_handle != 0)
		glDeleteProgramPipelines(1, &m_handle);
	m_handle = 0;

	m_vertexShader.reset();
	m_fragmentShader.reset();
}
//-----------------------------------------------------------------------------
GLBuffer& GLBuffer::operator=(GLBuffer&& other) noexcept
{
	if (this != &other)
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;
	}
	return *this;
}
//-----------------------------------------------------------------------------
GLsizeiptr GLBuffer::Size() const
{
	return getParameter(GL_BUFFER_SIZE);
}
//-----------------------------------------------------------------------------
GLenum GLBuffer::Usage() const
{
	return getParameter(GL_BUFFER_USAGE);
}
//-----------------------------------------------------------------------------
GLenum GLBuffer::Access() const
{
	return getParameter(GL_BUFFER_ACCESS);
}
//-----------------------------------------------------------------------------
GLbitfield GLBuffer::AccessFlags() const
{
	return getParameter(GL_BUFFER_ACCESS_FLAGS);
}
//-----------------------------------------------------------------------------
bool GLBuffer::IsMapped() const
{
	return getParameter(GL_BUFFER_MAPPED) != 0;
}
//-----------------------------------------------------------------------------
bool GLBuffer::IsImmutable() const
{
	return getParameter(GL_BUFFER_IMMUTABLE_STORAGE) != 0;
}
//-----------------------------------------------------------------------------
GLbitfield GLBuffer::StorageFlags() const
{
	return getParameter(GL_BUFFER_STORAGE_FLAGS);
}
//-----------------------------------------------------------------------------
GLintptr GLBuffer::MapOffset() const
{
	return getParameter64(GL_BUFFER_MAP_OFFSET);
}
//-----------------------------------------------------------------------------
GLsizeiptr GLBuffer::MapSize() const
{
	return getParameter64(GL_BUFFER_MAP_LENGTH);
}
//-----------------------------------------------------------------------------
void* GLBuffer::MapPointer() const
{
	void* pointer;
	glGetNamedBufferPointerv(m_handle, GL_BUFFER_MAP_POINTER, &pointer);
	return pointer;
}
//-----------------------------------------------------------------------------
void GLBuffer::createHandle()
{
	glCreateBuffers(1, &m_handle);
	assert(m_handle);
}
//-----------------------------------------------------------------------------
void GLBuffer::destroyHandle()
{
	if (m_handle != 0)
		glDeleteBuffers(1, &m_handle);
	m_handle = 0;
}
//-----------------------------------------------------------------------------
GLint GLBuffer::getParameter(const GLenum parameter) const
{
	GLint result;
	glGetNamedBufferParameteriv(m_handle, parameter, &result);
	return result;
}
//-----------------------------------------------------------------------------
GLint GLBuffer::getParameter64(const GLenum parameter) const
{
	GLint64 result;
	glGetNamedBufferParameteri64v(m_handle, parameter, &result);
	return result;
}
//-----------------------------------------------------------------------------
GLVertexArray& GLVertexArray::operator=(GLVertexArray&& other) noexcept
{
	if (this != &other)
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;
	}
	return *this;
};
//-----------------------------------------------------------------------------
void GLVertexArray::createHandle()
{
	glCreateVertexArrays(1, &m_handle);
	assert(m_handle);
}
//-----------------------------------------------------------------------------
void GLVertexArray::destroyHandle()
{
	if (m_handle != 0)
		glDeleteVertexArrays(1, &m_handle);
	m_handle = 0;
}
//-----------------------------------------------------------------------------
GLTexture& GLTexture::operator=(GLTexture&& other) noexcept
{
	if (this != &other)
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;
	}
	return *this;
};
//-----------------------------------------------------------------------------
void GLTexture::createHandle(GLenum target)
{
	glCreateTextures(target, 1, &m_handle);
	assert(m_handle);
}
//-----------------------------------------------------------------------------
void GLTexture::destroyHandle()
{
	if (m_handle != 0)
		glDeleteTextures(1, &m_handle);
	m_handle = 0;
}
//-----------------------------------------------------------------------------
GLFramebuffer& GLFramebuffer::operator=(GLFramebuffer&& other) noexcept
{
	if (this != &other)
	{
		destroyHandle();
		m_handle = other.m_handle;
		other.m_handle = 0;
	}
	return *this;
};
//-----------------------------------------------------------------------------
void GLFramebuffer::createHandle()
{
	glCreateFramebuffers(1, &m_handle);
	assert(m_handle);
}
//-----------------------------------------------------------------------------
void GLFramebuffer::destroyHandle()
{
	if (m_handle != 0)
		glDeleteFramebuffers(1, &m_handle);
	m_handle = 0;
	m_colorTextures.clear();
	m_depthTexture.reset();
}
//-----------------------------------------------------------------------------