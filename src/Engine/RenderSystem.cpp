#include "stdafx.h"
#include "RenderSystem.h"
#include "WindowSystem.h"
#include "LogSystem.h"
#include "Utils.h"
#include "CheckSupportOGLExtensions.h"
//-----------------------------------------------------------------------------
// TODO: добавить проверки ошибок ресурсов. сейчас только ассеты
//-----------------------------------------------------------------------------
// Use discrete GPU by default.
#if PLATFORM_DESKTOP
extern "C"
{
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

	// https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
	__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#endif
//-----------------------------------------------------------------------------
#if defined(_DEBUG)
void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*user_param*/) noexcept
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::string msgSource;
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             msgSource = "GL_DEBUG_SOURCE_API";             break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: msgSource = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     msgSource = "GL_DEBUG_SOURCE_THIRD_PARTY";     break;
	case GL_DEBUG_SOURCE_APPLICATION:     msgSource = "GL_DEBUG_SOURCE_APPLICATION";     break;
	case GL_DEBUG_SOURCE_OTHER:           msgSource = "GL_DEBUG_SOURCE_OTHER";           break;
	}

	std::string msgType;
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               msgType = "GL_DEBUG_TYPE_ERROR";               break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: msgType = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  msgType = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";  break;
	case GL_DEBUG_TYPE_PORTABILITY:         msgType = "GL_DEBUG_TYPE_PORTABILITY";         break;
	case GL_DEBUG_TYPE_PERFORMANCE:         msgType = "GL_DEBUG_TYPE_PERFORMANCE";         break;
	case GL_DEBUG_TYPE_OTHER:               msgType = "GL_DEBUG_TYPE_OTHER";               break;
	}

	std::string msgSeverity = "DEFAULT";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:    msgSeverity = "GL_DEBUG_SEVERITY_LOW";    break;
	case GL_DEBUG_SEVERITY_MEDIUM: msgSeverity = "GL_DEBUG_SEVERITY_MEDIUM"; break;
	case GL_DEBUG_SEVERITY_HIGH:   msgSeverity = "GL_DEBUG_SEVERITY_HIGH";   break;
	}

	std::string logMsg = "glDebugMessage: " + std::string(message) + ", type = " + msgType + ", source = " + msgSource + ", severity = " + msgSeverity;

	if (type == GL_DEBUG_TYPE_ERROR) LogWarning(logMsg);
	else                             LogError(logMsg);

}
#endif // _DEBUG
//-----------------------------------------------------------------------------
RenderSystem *gRenderSystem = nullptr; // TODO: удалить
//-----------------------------------------------------------------------------
RenderSystem::RenderSystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.render = this;
	gRenderSystem = this;
}
//-----------------------------------------------------------------------------
RenderSystem::~RenderSystem()
{
	Destroy();
}
//-----------------------------------------------------------------------------
bool RenderSystem::Create(const RenderSystemCreateInfo& createInfo)
{
	assert(m_systems.window);

	LogPrint("OpenGL: OpenGL device information:");
	LogPrint("    > Vendor:   " + std::string((const char*)glGetString(GL_VENDOR)));
	LogPrint("    > Renderer: " + std::string((const char*)glGetString(GL_RENDERER)));
	LogPrint("    > Version:  " + std::string((const char*)glGetString(GL_VERSION)));
	LogPrint("    > GLSL:     " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));

	if (!CheckSupportExtensions())
		return false;

	initializeExtensions(true);
	initializeCapabilities(true);

	// не использовать Bind(state) так как дефолтные значения из кеша могут не соответствовать установкам.

	// set default depth state
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glDepthMask(GL_TRUE);

	//// set defautl stensil state
	//// TODO: проставить дефолтные значения StencilState

	////glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(createInfo.clearColor.x, createInfo.clearColor.y, createInfo.clearColor.z, 1.0f);

	LogPrint("RenderSystem Create");

	return true;
}
//-----------------------------------------------------------------------------
void RenderSystem::Destroy()
{
	m_cacheFileTextures2D.clear();
	m_systems.render = nullptr;
}
//-----------------------------------------------------------------------------
void RenderSystem::BeginFrame()
{
	//glViewport(0, 0, m_systems.window->GetWidth(), m_systems.window->GetHeight());
	//glClearColor(0.2f, 0.4f, 1.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
}
//-----------------------------------------------------------------------------
void RenderSystem::EndFrame()
{
	m_systems.window->SwapBuffers();
}
//-----------------------------------------------------------------------------
GLSeparableShaderProgramRef RenderSystem::CreateSeparableShaderProgram(GLenum shaderType, std::string_view sourceCode)
{
	auto resource = std::make_shared<GLSeparableShaderProgram>(shaderType, sourceCode);
	if (IsValid(resource)) return resource;
	else
	{
		m_systems.log->Error("GLSeparableShaderProgram not create");
		return nullptr;
	}
}
//-----------------------------------------------------------------------------
GLProgramPipelineRef RenderSystem::CreateProgramPipeline()
{
	auto resource = std::make_shared<GLProgramPipeline>();
	if (IsValid(resource)) return resource;
	else
	{
		m_systems.log->Error("GLProgramPipeline not create");
		return nullptr;
	}
}
//-----------------------------------------------------------------------------
GLProgramPipelineRef RenderSystem::CreateProgramPipeline(GLSeparableShaderProgramRef vertexShader, GLSeparableShaderProgramRef fragmentShader)
{
	if (!IsValid(vertexShader))
	{
		m_systems.log->Error("vertexShader is null");
		return nullptr;
	}
	if (!IsValid(fragmentShader))
	{
		m_systems.log->Error("fragmentShader is null");
		return nullptr;
	}

	auto resource = CreateProgramPipeline();
	if (resource)
		ProgramPipelineSetSeparableShaders(resource, vertexShader, fragmentShader);
	return resource;
}
//-----------------------------------------------------------------------------
GLProgramPipelineRef RenderSystem::CreateProgramPipelineFromSources(std::string_view vertSource, std::string_view fragSource)
{
	GLSeparableShaderProgramRef vertexShader = CreateSeparableShaderProgram(GL_VERTEX_SHADER, vertSource);
	GLSeparableShaderProgramRef fragmentShader = CreateSeparableShaderProgram(GL_FRAGMENT_SHADER, fragSource);
	return CreateProgramPipeline(vertexShader, fragmentShader);
}
//-----------------------------------------------------------------------------
GLProgramPipelineRef RenderSystem::CreateProgramPipelineFromFiles(std::string_view vertFilepath, std::string_view fragFilepath)
{
	const auto vertSource = ReadTextFile(vertFilepath);
	const auto fragSource = ReadTextFile(fragFilepath);
	return CreateProgramPipelineFromSources(vertSource, fragSource);
}
//-----------------------------------------------------------------------------
GLBufferRef RenderSystem::CreateBuffer()
{
	auto resource = std::make_shared<GLBuffer>();
	if (IsValid(resource)) return resource;
	else
	{
		m_systems.log->Error("GLBuffer not create");
		return nullptr;
	}
}
//-----------------------------------------------------------------------------
GLVertexArrayRef RenderSystem::CreateVertexArray()
{
	auto resource = std::make_shared<GLVertexArray>();
	if (IsValid(resource)) return resource;
	else
	{
		m_systems.log->Error("GLVertexArray not create");
		return nullptr;
	}
}
//-----------------------------------------------------------------------------
GLVertexArrayRef RenderSystem::CreateVertexArray(const std::vector<AttribFormat>& attribFormats)
{
	auto resource = CreateVertexArray();
	if (resource) VertexArraySetAttribFormats(resource, attribFormats);
	return resource;
}
//-----------------------------------------------------------------------------
GLVertexArrayRef RenderSystem::CreateVertexArray(GLBufferRef vbo, size_t vertexSize, const std::vector<AttribFormat>& attribFormats)
{
	return CreateVertexArray(vbo, vertexSize, nullptr, attribFormats);
}
//-----------------------------------------------------------------------------
GLVertexArrayRef RenderSystem::CreateVertexArray(GLBufferRef vbo, size_t vertexSize, GLBufferRef ibo, const std::vector<AttribFormat>& attribFormats)
{
	auto resource = CreateVertexArray(attribFormats);
	if (!IsValid(resource)) return nullptr;
	if (IsValid(vbo) && vertexSize) VertexArraySetVertexBuffer(resource, vbo, vertexSize);
	if (IsValid(ibo)) VertexArraySetIndexBuffer(resource, ibo);
	return resource;
}
//-----------------------------------------------------------------------------
GLTextureRef RenderSystem::CreateTexture2D()
{
	auto resource = std::make_shared<GLTexture>(GL_TEXTURE_2D);
	if (IsValid(resource)) return resource;
	else
	{
		m_systems.log->Error("GLTexture(GL_TEXTURE_2D) not create");
		return nullptr;
	}
}
//-----------------------------------------------------------------------------
GLTextureRef RenderSystem::CreateTexture2D(GLenum internalFormat, GLenum format, GLsizei width, GLsizei height, void* data, GLenum filter, GLenum repeat, bool generateMipMaps)
{
	auto texture = CreateTexture2D();
	if (!IsValid(texture)) return nullptr;

	int levels = 1;
	if (generateMipMaps)
		levels = 1 + (int)std::floor(std::log2(std::min(width, height)));

	glTextureStorage2D(*texture, levels, internalFormat, width, height);

	if (generateMipMaps)
	{
		if (filter == GL_LINEAR)
			glTextureParameteri(*texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else if (filter == GL_NEAREST)
			glTextureParameteri(*texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else
			LogFatal("Unsupported filter");
	}
	else
	{
		glTextureParameteri(*texture, GL_TEXTURE_MIN_FILTER, filter);
	}
	glTextureParameteri(*texture, GL_TEXTURE_MAG_FILTER, filter);

	glTextureParameteri(*texture, GL_TEXTURE_WRAP_S, repeat);
	glTextureParameteri(*texture, GL_TEXTURE_WRAP_T, repeat);
	glTextureParameteri(*texture, GL_TEXTURE_WRAP_R, repeat);

	if (data)
		glTextureSubImage2D(*texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

	if (generateMipMaps) 
		glGenerateTextureMipmap(*texture);

	return texture;
}
//-----------------------------------------------------------------------------
GLTextureRef RenderSystem::CreateTexture2DFromFile(std::string_view filepath, int comp, bool generateMipMaps)
{
	int x, y, c;
	if (!std::filesystem::exists(filepath.data()))
	{
		std::ostringstream message;
		message << "file " << filepath.data() << " does not exist.";
		LogFatal(message.str());
	}
	const auto data = stbi_load(filepath.data(), &x, &y, &c, comp);

	auto const [in, ex] = [comp]() {
		switch (comp)
		{
		case STBI_rgb_alpha:	return std::make_pair(GL_RGBA8, GL_RGBA);
		case STBI_rgb:			return std::make_pair(GL_RGB8, GL_RGB);
		case STBI_grey:			return std::make_pair(GL_R8, GL_RED);
		case STBI_grey_alpha:	return std::make_pair(GL_RG8, GL_RG);
		default: LogFatal("invalid format");
		}
		}();

	auto resource = CreateTexture2D(in, ex, x, y, data, GL_LINEAR, GL_REPEAT, generateMipMaps);
	stbi_image_free(data);
	return resource;
}
//-----------------------------------------------------------------------------
GLTextureRef RenderSystem::CreateTextureCube()
{
	auto resource = std::make_shared<GLTexture>(GL_TEXTURE_CUBE_MAP);
	if (IsValid(resource)) return resource;
	else
	{
		m_systems.log->Error("GLTexture(GL_TEXTURE_CUBE_MAP) not create");
		return nullptr;
	}
}
//-----------------------------------------------------------------------------
GLTextureRef RenderSystem::CreateTextureCubeFromFile(std::array<std::string_view, 6> const& filepath, int comp)
{
	int x, y, c;
	std::array<stbi_uc*, 6> faces;

	auto const [in, ex] = [comp]() {
		switch (comp)
		{
		case STBI_rgb_alpha:	return std::make_pair(GL_RGBA8, GL_RGBA);
		case STBI_rgb:			return std::make_pair(GL_RGB8, GL_RGB);
		case STBI_grey:			return std::make_pair(GL_R8, GL_RED);
		case STBI_grey_alpha:	return std::make_pair(GL_RG8, GL_RG);
		default: throw std::runtime_error("invalid format");
		}
		}();

	for (auto i = 0; i < 6; i++)
		faces[i] = stbi_load(filepath[i].data(), &x, &y, &c, comp);

	auto resource = CreateTextureCube(in, ex, x, y, faces);
	for (auto face : faces)
		stbi_image_free(face);

	return resource;
}
//-----------------------------------------------------------------------------
GLFramebufferRef RenderSystem::CreateFramebuffer()
{
	auto resource = std::make_shared<GLFramebuffer>();
	if (IsValid(resource)) return resource;
	else
	{
		m_systems.log->Error("GLFramebuffer not create");
		return nullptr;
	}
}
//-----------------------------------------------------------------------------
GLFramebufferRef RenderSystem::CreateFramebuffer(const std::vector<GLTextureRef>& cols, GLTextureRef depth)
{
	auto resource = CreateFramebuffer();
	if (IsValid(resource))
		FramebufferSetTextures(resource, cols, depth);
	return resource;
}
//-----------------------------------------------------------------------------
void RenderSystem::ProgramPipelineSetSeparableShaders(GLProgramPipelineRef pipeline, GLSeparableShaderProgramRef vertexShader, GLSeparableShaderProgramRef fragmentShader)
{
	assert(IsValid(pipeline));
	assert(IsValid(vertexShader));
	assert(IsValid(fragmentShader));

	if (!IsValid(pipeline))
	{
		m_systems.log->Error("GLProgramPipelineRef is null");
		return;
	}
	if (!IsValid(vertexShader))
	{
		m_systems.log->Error("GLSeparableShaderProgramRef is null");
		return;
	}
	if (!IsValid(fragmentShader))
	{
		m_systems.log->Error("GLSeparableShaderProgramRef is null");
		return;
	}

	glUseProgramStages(*pipeline, GL_VERTEX_SHADER_BIT, *vertexShader);
	glUseProgramStages(*pipeline, GL_FRAGMENT_SHADER_BIT, *fragmentShader);

	pipeline->m_vertexShader = vertexShader;
	pipeline->m_fragmentShader = fragmentShader;
}
//-----------------------------------------------------------------------------
void RenderSystem::ClearSubData(GLBufferRef buffer, const GLenum internalFormat, const GLintptr offset, const GLsizeiptr size, const GLenum format, const GLenum data_type, const void* data)
{
	glClearNamedBufferSubData(*buffer, internalFormat, offset, size, format, data_type, data);
}
//-----------------------------------------------------------------------------
void RenderSystem::ClearData(GLBufferRef buffer, const GLenum internalFormat, const GLenum format, const GLenum data_type, const void* data)
{
	glClearNamedBufferData(*buffer, internalFormat, format, data_type, data);
}
//-----------------------------------------------------------------------------
void* RenderSystem::MapRange(GLBufferRef buffer, const GLintptr offset, const GLsizeiptr size, const GLbitfield accessFlags) const
{
	return glMapNamedBufferRange(*buffer, offset, size, accessFlags);
}
//-----------------------------------------------------------------------------
void* RenderSystem::Map(GLBufferRef buffer, const GLenum access) const
{
	return glMapNamedBuffer(*buffer, access);
}
//-----------------------------------------------------------------------------
void RenderSystem::FlushMappedRange(GLBufferRef buffer, const GLintptr offset, const GLsizeiptr size) const
{
	glFlushMappedNamedBufferRange(*buffer, offset, size);
}
//-----------------------------------------------------------------------------
void RenderSystem::Unmap(GLBufferRef buffer) const
{
	glUnmapNamedBuffer(*buffer);
}
//-----------------------------------------------------------------------------
void RenderSystem::InvalidateSubData(GLBufferRef buffer, const GLintptr offset, const GLsizeiptr size) const
{
	glInvalidateBufferSubData(*buffer, offset, size);
}
//-----------------------------------------------------------------------------
void RenderSystem::Invalidate(GLBufferRef buffer) const
{
	glInvalidateBufferData(*buffer);
}
//-----------------------------------------------------------------------------
void RenderSystem::CopySubData(GLBufferRef source, GLBufferRef dest, const GLintptr sourceOffset, const GLintptr offset, const GLsizeiptr size) const
{
	glCopyNamedBufferSubData(*source, *dest, sourceOffset, offset, size);
}
//-----------------------------------------------------------------------------
void RenderSystem::VertexArraySetAttribFormats(GLVertexArrayRef vao, const std::vector<AttribFormat>& attribFormats)
{
	assert(IsValid(vao));
	if (!IsValid(vao)) return;
	for (auto const& format : attribFormats)
	{
		glEnableVertexArrayAttrib(*vao, format.attribIndex);
		glVertexArrayAttribFormat(*vao, format.attribIndex, format.size, format.type, GL_FALSE, format.relativeOffset);
		glVertexArrayAttribBinding(*vao, format.attribIndex, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::VertexArraySetVertexBuffer(GLVertexArrayRef vao, GLBufferRef vbo, size_t vertexSize)
{
	VertexArraySetVertexBuffer(vao, 0, vbo, 0, vertexSize);
}
//-----------------------------------------------------------------------------
void RenderSystem::VertexArraySetVertexBuffer(GLVertexArrayRef vao, const GLuint bindingIndex, GLBufferRef vbo, const GLintptr offset, const GLsizei stride)
{
	assert(IsValid(vao));
	assert(IsValid(vbo));
	if (IsValid(vao) && IsValid(vbo))
		glVertexArrayVertexBuffer(*vao, bindingIndex, *vbo, offset, stride);
}
//-----------------------------------------------------------------------------
void RenderSystem::VertexArraySetIndexBuffer(GLVertexArrayRef vao, GLBufferRef ibo)
{
	assert(IsValid(vao));
	assert(IsValid(ibo));
	if (IsValid(vao) && IsValid(ibo))
		glVertexArrayElementBuffer(*vao, *ibo);
}
//-----------------------------------------------------------------------------
void RenderSystem::FramebufferSetTextures(GLFramebufferRef fbo, const std::vector<GLTextureRef>& cols, GLTextureRef depth)
{
	assert(IsValid(fbo));
	if (!IsValid(fbo)) return;

	for (auto i = 0; i < cols.size(); i++)
		glNamedFramebufferTexture(*fbo, GL_COLOR_ATTACHMENT0 + i, *cols[i], 0);

	std::array<GLenum, 32> drawBuffs{};
	for (GLenum i = 0; i < cols.size(); i++)
		drawBuffs[i] = GL_COLOR_ATTACHMENT0 + i;

	glNamedFramebufferDrawBuffers(*fbo, cols.size(), drawBuffs.data());

	if (depth)
		glNamedFramebufferTexture(*fbo, GL_DEPTH_ATTACHMENT, *depth, 0);

	if (glCheckNamedFramebufferStatus(*fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LogFatal("incomplete framebuffer");
}
//-----------------------------------------------------------------------------
void RenderSystem::FramebufferClear(GLFramebufferRef framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value)
{
	glClearNamedFramebufferfv(*framebuffer, buffer, drawbuffer, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(GLProgramPipelineRef pipeline)
{
	glBindProgramPipeline(*pipeline);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(GLVertexArrayRef vao)
{
	glBindVertexArray(*vao);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(GLGeometryRef geom)
{
	glBindVertexArray(*geom->vao);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindSlot(GLTextureRef texture, GLuint unit)
{
	glBindTextureUnit(unit, *texture);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(GLFramebufferRef framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
}
//-----------------------------------------------------------------------------
void RenderSystem::MainFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//-----------------------------------------------------------------------------
void RenderSystem::BlitFrameBuffer(
	GLFramebufferRef readFramebuffer, GLFramebufferRef drawFramebuffer, 
	GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, 
	GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, 
	GLbitfield mask, GLenum filter)
{
	GLuint readFramebufferId = 0;
	GLuint drawFramebufferId = 0;
	if (readFramebuffer != nullptr) readFramebufferId = *readFramebuffer;
	if (drawFramebuffer != nullptr) drawFramebufferId = *drawFramebuffer;

	glBlitNamedFramebuffer(
		readFramebufferId,
		drawFramebufferId,
		srcX0, srcY0, srcX1, srcY1,
		dstX0, dstY0, dstX1, dstY1,
		mask, filter);
}
//-----------------------------------------------------------------------------
void RenderSystem::initializeExtensions(bool print)
{
	// reset extensions state
	OpenGLExtensions::version = OPENGL33;
	OpenGLExtensions::coreDebug = false;
	OpenGLExtensions::coreDirectStateAccess = false;

#if PLATFORM_DESKTOP
	if (!GLAD_GL_VERSION_3_3)
	{
		LogFatal("OpenGL 3.3+ not support");
		return;
	}
	if (GLAD_GL_VERSION_4_0) OpenGLExtensions::version = OPENGL40;
	if (GLAD_GL_VERSION_4_1) OpenGLExtensions::version = OPENGL41;
	if (GLAD_GL_VERSION_4_2) OpenGLExtensions::version = OPENGL42;
	if (GLAD_GL_VERSION_4_3) OpenGLExtensions::version = OPENGL43;
	if (GLAD_GL_VERSION_4_4) OpenGLExtensions::version = OPENGL44;
	if (GLAD_GL_VERSION_4_5) OpenGLExtensions::version = OPENGL45;
	if (GLAD_GL_VERSION_4_6) OpenGLExtensions::version = OPENGL46;
#else
	// TODO:
#endif // PLATFORM_DESKTOP

#if defined(_DEBUG)
	if (OpenGLExtensions::version >= OPENGL43 && glDebugMessageCallback)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugCallback, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
		OpenGLExtensions::coreDebug = true;
	}
#endif

	if (OpenGLExtensions::version >= OPENGL45)
		OpenGLExtensions::coreDirectStateAccess = true;

	if (print)
	{
		LogPrint("OpenGL: Extensions information:");
		LogPrint(std::string("    > OpenGL Debug: ") + (OpenGLExtensions::coreDebug ? "enable" : "disable"));
		LogPrint(std::string("    > OpenGL Direct State Access: ") + (OpenGLExtensions::coreDirectStateAccess ? "enable" : "disable"));
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::initializeCapabilities(bool print)
{
	if (print) LogPrint("OpenGL: Capabilities information:");

	GLint openGLValue = 0;

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &openGLValue);
	Capabilities::maximumTextureDimension = static_cast<uint32_t>(openGLValue);
	if (print) LogPrint("    > Maximum Texture Dimension = " + std::to_string(Capabilities::maximumTextureDimension));

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &openGLValue);
	Capabilities::maximumTextureUnitCount = static_cast<uint32_t>(openGLValue);
	if (print) LogPrint("    > Maximum Texture Unit Count = " + std::to_string(Capabilities::maximumTextureUnitCount));

	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &openGLValue);
	Capabilities::maximumUniformBufferSize = static_cast<uint32_t>(openGLValue);
	if (print) LogPrint("    > Maximum Uniform Buffer Size = " + std::to_string(Capabilities::maximumUniformBufferSize));
}
//-----------------------------------------------------------------------------