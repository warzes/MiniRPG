#include "stdafx.h"
#include "nRenderResources.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
void GLProgramObject::createHandle(GLenum type, std::string_view source)
{
	const auto srcPtr = source.data();
	m_handle = glCreateShaderProgramv(type, 1, &srcPtr);
	assert(m_handle);
	validate();
}
//-----------------------------------------------------------------------------
void GLProgramObject::destroyHandle()
{
	if (m_handle != 0)
		glDeleteProgram(m_handle);
	m_handle = 0;
}
//-----------------------------------------------------------------------------
void GLProgramObject::validate()
{
	GLint compiled = 0;
	glProgramParameteri(m_handle, GL_PROGRAM_SEPARABLE, GL_TRUE);
	glGetProgramiv(m_handle, GL_LINK_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		std::array<char, 1024> compiler_log;
		glGetProgramInfoLog(m_handle, compiler_log.size(), nullptr, compiler_log.data());
		glDeleteShader(m_handle);

		std::ostringstream message;
		message << "shader contains error(s):\n\n" << compiler_log.data() << '\n';
		LogError(message.str());
	}
}
//-----------------------------------------------------------------------------