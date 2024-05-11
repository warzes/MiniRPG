#include "stdafx.h"
#include "RenderCore.h"
//-----------------------------------------------------------------------------
uint8_t OpenGLExtensions::version;
bool OpenGLExtensions::coreDebug;
bool OpenGLExtensions::coreDirectStateAccess;
//-----------------------------------------------------------------------------
uint32_t Capabilities::maximumTextureDimension;
uint32_t Capabilities::maximumTextureUnitCount;
uint32_t Capabilities::maximumUniformBufferSize;
//-----------------------------------------------------------------------------
ShaderBytecode::ShaderBytecode(ShaderSourceType shaderSource, const std::string& text)
{
	m_shaderSource = shaderSource;
	if (m_shaderSource == ShaderSourceType::CodeMemory)
		m_sourceCode = text;
	else if (m_shaderSource == ShaderSourceType::CodeFile)
	{
		m_sourceCode = text;
		LoadFromFile(text);
	}
	else if (m_shaderSource == ShaderSourceType::BinaryBuffer)
	{
		LogFatal("Not impl!");
	}
	else if (m_shaderSource == ShaderSourceType::BinaryFile)
	{
		LogFatal("Not impl!");
	}
}
//-----------------------------------------------------------------------------
bool ShaderBytecode::LoadFromFile(const std::string& file)
{
	m_shaderSource = ShaderSourceType::CodeFile;

	// TODO: âîçìîæíî çàìåíèòü fstream íà file?
	m_filename = file;
	//m_path = ; íå çàáûòü
	std::ifstream shaderFile(file); // TODO: çàìåíèòü
	if (!shaderFile.is_open())
	{
		LogError("Shader file '" + file + "' not found.");
		return false;
	}
	std::stringstream shader_string;
	shader_string << shaderFile.rdbuf();
	m_sourceCode = shader_string.str();
	// Remove the EOF from the end of the string.
	if (m_sourceCode[m_sourceCode.length() - 1] == EOF)
		m_sourceCode.pop_back();
	return true;
}
//-----------------------------------------------------------------------------
std::string ShaderBytecode::GetHeaderVertexShader()
{
#if PLATFORM_EMSCRIPTEN
	return "#version 300 es";
#else
	return R"(
#version 460 core
)";
#endif
}
//-----------------------------------------------------------------------------
std::string ShaderBytecode::GetHeaderFragmentShader()
{
#if PLATFORM_EMSCRIPTEN
	return R"(#version 300 es
precision mediump float;

)";
#else
	return R"(
#version 460 core
)";
#endif
}
//-----------------------------------------------------------------------------