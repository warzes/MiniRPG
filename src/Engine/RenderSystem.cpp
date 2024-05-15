#include "stdafx.h"
#include "RenderSystem.h"
#include "WindowSystem.h"
#include "LogSystem.h"
#include "Utils.h"
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

	initializeExtensions(true);
	initializeCapabilities(true);

	// не использовать Bind(state) так как дефолтные значения из кеша могут не соответствовать установкам.

	// set default depth state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// set defautl stensil state
	// TODO: проставить дефолтные значения StencilState

	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(createInfo.clearColor.x, createInfo.clearColor.y, createInfo.clearColor.z, 1.0f);

	setClearMask(true, true, false);

	LogPrint("RenderSystem Create");

	return true;
}
//-----------------------------------------------------------------------------
void RenderSystem::Destroy()
{
	ResetAllStates();
	m_cacheFileTextures2D.clear();
	m_systems.render = nullptr;
}
//-----------------------------------------------------------------------------
void RenderSystem::BeginFrame()
{
	glViewport(0, 0, m_systems.window->GetWidth(), m_systems.window->GetHeight());
	glClearColor(0.2f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
//-----------------------------------------------------------------------------
void RenderSystem::EndFrame()
{
	m_systems.window->SwapBuffers();
}
//-----------------------------------------------------------------------------
void RenderSystem::SetClearColor(const glm::vec3& color)
{
	glClearColor(color.x, color.y, color.z, 1.0f);
}
//-----------------------------------------------------------------------------
GLProgramObjectRef RenderSystem::CreateProgramObject(GLenum type, std::string_view source)
{
	return std::make_shared<GLProgramObject>(type, source);
}
//-----------------------------------------------------------------------------
GLProgramPipelineRef RenderSystem::CreateProgramPipeline()
{
	return std::make_shared<GLProgramPipeline>();
}
//-----------------------------------------------------------------------------
GLProgramPipelineRef RenderSystem::CreateProgramPipeline(GLProgramObjectRef vertexShader, GLProgramObjectRef fragmentShader)
{
	GLProgramPipelineRef ret = CreateProgramPipeline();
	ProgramPipelineSetProgramObjects(ret, vertexShader, fragmentShader);
	return ret;
}
//-----------------------------------------------------------------------------
GLProgramPipelineRef RenderSystem::CreateProgramPipelineFromSources(std::string_view vertSource, std::string_view fragSource)
{
	GLProgramObjectRef vertexShader = CreateProgramObject(GL_VERTEX_SHADER, vertSource);
	GLProgramObjectRef fragmentShader = CreateProgramObject(GL_FRAGMENT_SHADER, fragSource);
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
GLVertexArrayRef RenderSystem::CreateVertexArray()
{
	return std::make_shared<GLVertexArray>();
}
//-----------------------------------------------------------------------------
GLVertexArrayRef RenderSystem::CreateVertexArray(const std::vector<AttribFormat>& attribFormats)
{
	GLVertexArrayRef resource = std::make_shared<GLVertexArray>();
	VertexArraySetAttribFormats(resource, attribFormats);
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
	GLVertexArrayRef vao = CreateVertexArray(attribFormats);
	if (vbo && *vbo && vertexSize) VertexArraySetVertexBuffer(vao, vbo, vertexSize);
	if (ibo && *ibo) VertexArraySetIndexBuffer(vao, ibo);
	return vao;
}
//-----------------------------------------------------------------------------
void RenderSystem::ProgramPipelineSetProgramObjects(GLProgramPipelineRef pipeline, GLProgramObjectRef vertexShader, GLProgramObjectRef fragmentShader)
{
	assert(pipeline && *pipeline);
	assert(vertexShader && *vertexShader);
	assert(fragmentShader && *fragmentShader);

	glUseProgramStages(*pipeline, GL_VERTEX_SHADER_BIT, *vertexShader);
	glUseProgramStages(*pipeline, GL_FRAGMENT_SHADER_BIT, *fragmentShader);

	pipeline->m_vertexShader = vertexShader;
	pipeline->m_fragmentShader = fragmentShader;
}
//-----------------------------------------------------------------------------
void RenderSystem::VertexArraySetAttribFormats(GLVertexArrayRef vao, const std::vector<AttribFormat>& attribFormats)
{
	assert(vao && *vao);
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
	assert(vao && *vao);
	assert(vbo && *vbo);
	glVertexArrayVertexBuffer(*vao, 0, *vbo, 0, vertexSize);
}
//-----------------------------------------------------------------------------
void RenderSystem::VertexArraySetIndexBuffer(GLVertexArrayRef vao, GLBufferRef ibo)
{
	assert(vao && *vao);
	assert(ibo && *ibo);
	glVertexArrayElementBuffer(*vao, *ibo);
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




//=============================================================================
// OLD
//=============================================================================
void RenderSystem::ClearFrame()
{
	glClear(m_cache.CurrentClearMask);
}
//-----------------------------------------------------------------------------
void RenderSystem::ClearFrame(const glm::vec3& color)
{
	glClearColor(color.x, color.y, color.z, 1.0f);
	ClearFrame();
}
//-----------------------------------------------------------------------------
void RenderSystem::SetViewport(int width, int height)
{
	SetViewport(0, 0, width, height);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RenderSystem::MainScreen()
{
	if (m_cache.CurrentFramebuffer > 0)
	{
		m_cache.CurrentFramebuffer = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// TODO: set current viewport and clear color?
		ClearFrame();
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::ResetAllStates()
{
	m_cache.Reset();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	for (unsigned i = 0; i < MaxBindingTextures; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//-----------------------------------------------------------------------------
void RenderSystem::ResetState(ResourceType type)
{
	if (type == ResourceType::ShaderProgram)
	{
		m_cache.CurrentShaderProgram = 0;
		glUseProgram(0);
	}
	else if (type == ResourceType::VertexBuffer)
	{
		m_cache.CurrentVBO = 0;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if (type == ResourceType::IndexBuffer)
	{
		m_cache.CurrentIBO = 0;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else if (type == ResourceType::VertexArray)
	{
		ResetState(ResourceType::VertexBuffer);
		ResetState(ResourceType::IndexBuffer);
		m_cache.CurrentVAO = 0;
		glBindVertexArray(0);
	}
	else if (type == ResourceType::Texture2D)
	{
		for (unsigned i = 0; i < MaxBindingTextures; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			m_cache.CurrentTexture2D[i] = 0;
		}
		glActiveTexture(GL_TEXTURE0);
	}
	else if (type == ResourceType::Framebuffer)
	{
		m_cache.CurrentFramebuffer = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(DepthState state)
{
	if (m_cache.CurrentDepthState.enable != state.enable)
	{
		if (state.enable) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
		setClearMask(true, state.enable, m_cache.CurrentStencilState.enable);
	}

	if (m_cache.CurrentDepthState.compareFunction != state.compareFunction)
		glDepthFunc(TranslateToGL(state.compareFunction));

	if (m_cache.CurrentDepthState.depthWrite != state.depthWrite)
		glDepthMask(static_cast<GLboolean>(state.depthWrite ? GL_TRUE : GL_FALSE));

	m_cache.CurrentDepthState = state;
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(StencilState state)
{
	StencilState& cache = m_cache.CurrentStencilState;
	if (cache.enable != state.enable)
	{
		if (state.enable) glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);

		setClearMask(true, m_cache.CurrentDepthState.enable, state.enable);
	}

	if (cache.stencilRef != state.stencilRef
		|| cache.readMask != state.readMask
		|| cache.frontFaceStencilCompareFunction != state.frontFaceStencilCompareFunction
		|| cache.frontFaceStencilFailureOperation != state.frontFaceStencilFailureOperation
		|| cache.frontFaceStencilDepthFailureOperation != state.frontFaceStencilDepthFailureOperation
		|| cache.frontFaceStencilPassOperation != state.frontFaceStencilPassOperation
		|| cache.backFaceStencilCompareFunction != state.backFaceStencilCompareFunction
		|| cache.backFaceStencilFailureOperation != state.backFaceStencilFailureOperation
		|| cache.backFaceStencilDepthFailureOperation != state.backFaceStencilDepthFailureOperation
		|| cache.backFaceStencilPassOperation != state.backFaceStencilPassOperation)
	{
		glStencilFuncSeparate(GL_FRONT, TranslateToGL(state.frontFaceStencilCompareFunction), state.stencilRef, state.readMask);
		glStencilOpSeparate(GL_FRONT, TranslateToGL(state.frontFaceStencilFailureOperation), TranslateToGL(state.frontFaceStencilDepthFailureOperation), TranslateToGL(state.frontFaceStencilPassOperation));
		glStencilFuncSeparate(GL_BACK, TranslateToGL(state.backFaceStencilCompareFunction), state.stencilRef, state.readMask);
		glStencilOpSeparate(GL_BACK, TranslateToGL(state.backFaceStencilFailureOperation), TranslateToGL(state.backFaceStencilDepthFailureOperation), TranslateToGL(state.backFaceStencilPassOperation));
	}

	if (cache.writeMask != state.writeMask)
		glStencilMask(state.writeMask);

	cache = state;
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(VertexBufferRef buffer)
{
	if (!buffer) return;
	assert(IsValid(buffer));

	if (m_cache.CurrentVBO == *buffer) return;
	m_cache.CurrentVBO = *buffer;
	glBindBuffer(GL_ARRAY_BUFFER, *buffer);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(IndexBufferRef buffer)
{
	if (!buffer) return;
	assert(IsValid(buffer));

	if (m_cache.CurrentIBO == *buffer) return;
	m_cache.CurrentIBO = *buffer;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *buffer);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(VertexArrayRef vao)
{
	if (m_cache.CurrentVAO != *vao)
	{
		m_cache.CurrentVAO = *vao;
		m_cache.CurrentVBO = 0;
		m_cache.CurrentIBO = 0;
		glBindVertexArray(*vao);
	}
	Bind(vao->vbo);
	Bind(vao->ibo);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(const VertexAttribute& attribute)
{
	const GLuint oglLocation = static_cast<GLuint>(attribute.location);
	glEnableVertexAttribArray(oglLocation);
	glVertexAttribPointer(
		oglLocation,
		attribute.size,
		GL_FLOAT,
		(GLboolean)(attribute.normalized ? GL_TRUE : GL_FALSE),
		attribute.stride,
		attribute.offset);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(Texture2DRef resource, unsigned slot)
{
	if (!resource) return;
	assert(IsValid(resource));
	if (m_cache.CurrentTexture2D[slot] == *resource) return;
	m_cache.CurrentTexture2D[slot] = *resource;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, *resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(FramebufferRef resource)
{
	if (!resource) return;
	assert(IsValid(resource));
	if (m_cache.CurrentFramebuffer == *resource) return;
	m_cache.CurrentFramebuffer = *resource;
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	glViewport(0, 0, resource->size.x, resource->size.y);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLVertexBuffer(unsigned id)
{
	if (m_cache.CurrentVBO == id) return;
	m_cache.CurrentVBO = id;
	glBindBuffer(GL_ARRAY_BUFFER, id);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLIndexBuffer(unsigned id)
{
	if (m_cache.CurrentIBO == id) return;
	m_cache.CurrentIBO = id;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLVertexArray(unsigned id)
{
	if (m_cache.CurrentVAO == id) return;
	m_cache.CurrentVAO = id;
	m_cache.CurrentVBO = 0;
	m_cache.CurrentIBO = 0;
	glBindVertexArray(id);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLTexture2D(unsigned id, unsigned slot)
{
	if (m_cache.CurrentTexture2D[slot] == id) return;
	m_cache.CurrentTexture2D[slot] = id;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}
//-----------------------------------------------------------------------------
void RenderSystem::BindGLFramebuffer(unsigned id)
{
	if (m_cache.CurrentFramebuffer == id) return;
	m_cache.CurrentFramebuffer = id;
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
//-----------------------------------------------------------------------------
void RenderSystem::Draw(VertexArrayRef vao, PrimitiveTopology primitive)
{
	assert(IsValid(vao));

	Bind(vao);
	if (vao->ibo)
	{
		glDrawElements(TranslateToGL(primitive), (GLsizei)vao->ibo->count, SizeIndexType(vao->ibo->sizeInBytes), nullptr);
	}
	else
	{
		glDrawArrays(TranslateToGL(primitive), 0, (GLsizei)vao->vbo->count);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::Draw(GeometryBufferRef geom, PrimitiveTopology primitive)
{
	if (!IsValid(geom)) return;
	Draw(geom->vao, primitive);
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
bool RenderSystem::checkCurrentFrameBuffer() const
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LogError("FRAMEBUFFER:: Framebuffer is not complete!");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void RenderSystem::setClearMask(bool color, bool depth, bool stensil)
{
	m_cache.CurrentClearMask = color ? GL_COLOR_BUFFER_BIT : 0u;
	if (depth)   m_cache.CurrentClearMask |= GL_DEPTH_BUFFER_BIT;
	if (stensil) m_cache.CurrentClearMask |= GL_STENCIL_BUFFER_BIT;
}
//-----------------------------------------------------------------------------
RenderSystem& GetRenderSystem()
{
	return *gRenderSystem;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VertexBufferRef RenderSystem::CreateVertexBuffer(BufferUsage usage)
{
	VertexBufferRef resource(new VertexBuffer(usage));
	if (!IsValid(resource))
	{
		LogError("VertexBuffer create failed!");
		return {};
	}
	return resource;
}
//-----------------------------------------------------------------------------
VertexBufferRef RenderSystem::CreateVertexBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	VertexBufferRef resource(new VertexBuffer(usage, vertexCount, vertexSize));
	if (!IsValid(resource))
	{
		LogError("VertexBuffer create failed!");
		return {};
	}
	glBindBuffer(GL_ARRAY_BUFFER, *resource);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, data, TranslateToGL(usage));
	glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO); // restore current vb
	return resource;
}
//-----------------------------------------------------------------------------
IndexBufferRef RenderSystem::CreateIndexBuffer(BufferUsage usage)
{
	IndexBufferRef resource(new IndexBuffer(usage));
	if (!IsValid(resource))
	{
		LogError("IndexBuffer create failed!");
		return {};
	}
	return resource;
}
//-----------------------------------------------------------------------------
IndexBufferRef RenderSystem::CreateIndexBuffer(BufferUsage usage, unsigned indexCount, IndexFormat indexFormat, const void* data)
{
	if (!indexCount)
	{
		LogError("Can not define index buffer with no indices");
		return {};
	}

	const unsigned indexSize = SizeIndexType(indexFormat);
	IndexBufferRef resource(new IndexBuffer(usage, indexCount, indexSize));
	if (!IsValid(resource))
	{
		LogError("Failed to create index buffer");
		return {};
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *resource);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, data, TranslateToGL(usage));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO); // restore current ib
	return resource;
}
//-----------------------------------------------------------------------------
VertexArrayRef RenderSystem::CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, const std::vector<VertexAttribute>& attribs)
{
	if (vbo == nullptr || !IsValid(vbo) || attribs.size() == 0)
	{
		LogError("VertexBuffer is null");
		return {};
	}
	if (vbo->type != BufferTarget::ArrayBuffer)
	{
		LogError("vbo is not VertexBuffer valid!");
		return {};
	}
	if (ibo && ibo->type != BufferTarget::ElementArrayBuffer)
	{
		LogError("ibo is not IndexBuffer valid!");
		return {};
	}

	VertexArrayRef resource(new VertexArray(vbo, ibo, static_cast<unsigned>(attribs.size())));
	if (!IsValid(resource))
	{
		LogError("VertexArray create failed!");
		return {};
	}
	vbo->parentArray = resource;
	if (ibo) ibo->parentArray = resource;

	glBindVertexArray(*resource);
	Bind(resource->vbo);
	for (size_t i = 0; i < attribs.size(); i++)
	{
		Bind(attribs[i]);
	}
	Bind(resource->ibo);

	glBindVertexArray(m_cache.CurrentVAO); // restore VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO); // restore current VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO); // restore current IBO

	return resource;
}
//-----------------------------------------------------------------------------
VertexArrayRef RenderSystem::CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, ShaderProgramRef shaders)
{
	auto attribInfo = GetAttributesInfo(shaders);
	if (attribInfo.empty()) return {};

	size_t offset = 0;
	std::vector<VertexAttribute> attribs(attribInfo.size());
	for (size_t i = 0; i < attribInfo.size(); i++)
	{
		// TODO: gl_VertexID ñ÷èòàåòñÿ àòðèáóòîì, íî ïðè ýòîì ïî÷åìó-òî location ó íåãî ðàâåí -1
		// â áóäóùåì ïîôèêñèòü, à ïîêà íå þçàòü
		assert(attribInfo[i].location > -1);
		attribs[i].location = (unsigned)attribInfo[i].location; // TODO: attribInfo[i].location ìîæåò áûòü -1, òîãäà êàê â attribs[i].location unsigned. íóæíî èñïðàâèòü
		attribs[i].normalized = false;

		size_t sizeType = 0;
		switch (attribInfo[i].typeId)
		{
		case GL_FLOAT:
			attribs[i].size = 1;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC2:
			attribs[i].size = 2;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC3:
			attribs[i].size = 3;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC4:
			attribs[i].size = 4;
			sizeType = attribs[i].size * sizeof(float);
			break;

		default:
			LogError("Shader attribute type: " + attribInfo[i].name + " not support!");
			return {};
		}

		attribs[i].offset = (void*)+offset;
		offset += sizeType;
	}
	for (size_t i = 0; i < attribs.size(); i++)
	{
		attribs[i].stride = (int)offset;
	}

	return CreateVertexArray(vbo, ibo, attribs);
}
//-----------------------------------------------------------------------------
bool RenderSystem::UpdateBuffer(VertexBufferRef buffer, unsigned offset, unsigned count, unsigned size, const void* data)
{
	assert(IsValid(buffer));

	const unsigned id = *buffer;
	const bool isNewBufferData = (offset == 0 && (buffer->count != count || buffer->sizeInBytes != size));
	const GLsizeiptr numberOfBytes = count * size;
	const GLenum glBufferUsage = TranslateToGL(buffer->usage);
	buffer->count = count;
	buffer->sizeInBytes = size;

#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
	{
		if (isNewBufferData) glNamedBufferData(id, numberOfBytes, data, glBufferUsage);
		else glNamedBufferSubData(id, offset, numberOfBytes, data);
	}
	else
#endif
	{
		if (m_cache.CurrentVBO != id) glBindBuffer(GL_ARRAY_BUFFER, id);

		if (isNewBufferData) glBufferData(GL_ARRAY_BUFFER, numberOfBytes, data, glBufferUsage);
		else glBufferSubData(GL_ARRAY_BUFFER, offset, numberOfBytes, data);

		if (m_cache.CurrentVBO != id) glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO);
	}

	return true;
}
//-----------------------------------------------------------------------------
bool RenderSystem::UpdateBuffer(IndexBufferRef buffer, unsigned offset, unsigned count, IndexFormat indexFormat, const void* data)
{
	assert(IsValid(buffer));
	if (!data)
	{
		LogError("Null source data for updating index buffer");
		return false;
	}

	const unsigned indexSize = SizeIndexType(indexFormat);
	const unsigned id = *buffer;
	const bool isNewBufferData = (offset == 0 && (buffer->count != count || buffer->sizeInBytes != indexSize));
	const GLsizeiptr numberOfBytes = count * indexSize;
	const GLenum glBufferUsage = TranslateToGL(buffer->usage);
	buffer->count = count;
	buffer->sizeInBytes = indexSize;

#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
	{
		if (isNewBufferData)
			glNamedBufferData(id, numberOfBytes, data, glBufferUsage);
		else
			glNamedBufferSubData(id, offset * indexSize, numberOfBytes, data);
	}
	else
#endif
	{
		if (m_cache.CurrentIBO != id)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

		if (isNewBufferData)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfBytes, data, glBufferUsage);
		else
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * indexSize, numberOfBytes, data);

		if (m_cache.CurrentIBO != id)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO);
	}
}
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN
inline void* mapBuffer(unsigned buffer, unsigned currentState, GLenum target, GLenum access)
{
#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
		return glMapNamedBuffer(buffer, access);
	else
#endif
	{
		if (buffer != currentState) glBindBuffer(target, buffer);
		void* data = glMapBuffer(target, access);
		if (buffer != currentState) glBindBuffer(target, currentState);
		return data;
	}
}
#endif
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN
void* RenderSystem::MapBuffer(VertexBufferRef buffer, GLenum access)
{
	assert(IsValid(buffer));
	return mapBuffer(*buffer, m_cache.CurrentVBO, GL_ARRAY_BUFFER, access);
}
#endif
//-----------------------------------------------------------------------------
#if !PLATFORM_EMSCRIPTEN
void* RenderSystem::MapBuffer(IndexBufferRef buffer, GLenum access)
{
	assert(IsValid(buffer));
	return mapBuffer(*buffer, m_cache.CurrentIBO, GL_ELEMENT_ARRAY_BUFFER, access);
}
#endif
//-----------------------------------------------------------------------------
inline void* mapBuffer(unsigned buffer, unsigned currentState, GLenum target, unsigned offset, unsigned size, GLbitfield access)
{
#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
		return glMapNamedBufferRange(buffer, offset, size, access);
	else
#endif
	{
		if (buffer != currentState) glBindBuffer(target, buffer);
		void* data = glMapBufferRange(target, offset, size, access);
		if (buffer != currentState) glBindBuffer(target, currentState);
		return data;
	}
}
//-----------------------------------------------------------------------------
void* RenderSystem::MapBuffer(VertexBufferRef buffer, unsigned offset, unsigned size, GLbitfield access)
{
	assert(IsValid(buffer));
	return mapBuffer(*buffer, m_cache.CurrentVBO, GL_ARRAY_BUFFER, offset, size, access);
}
//-----------------------------------------------------------------------------
void* RenderSystem::MapBuffer(IndexBufferRef buffer, unsigned offset, unsigned size, GLbitfield access)
{
	assert(IsValid(buffer));
	return mapBuffer(*buffer, m_cache.CurrentIBO, GL_ELEMENT_ARRAY_BUFFER, offset, size, access);
}
//-----------------------------------------------------------------------------
inline bool unmapBuffer(unsigned buffer, unsigned currentState, GLenum target)
{
#if PLATFORM_DESKTOP
	if (OpenGLExtensions::coreDirectStateAccess)
		return GL_TRUE == glUnmapNamedBuffer(buffer);
	else
#endif
	{
		if (buffer != currentState) glBindBuffer(target, buffer);
		bool ret = GL_TRUE == glUnmapBuffer(target);
		if (buffer != currentState) glBindBuffer(target, currentState);
		return ret;
	}
}
//-----------------------------------------------------------------------------
bool RenderSystem::UnmapBuffer(VertexBufferRef buffer)
{
	assert(IsValid(buffer));
	return unmapBuffer(*buffer, m_cache.CurrentVBO, GL_ARRAY_BUFFER);
}
//-----------------------------------------------------------------------------
bool RenderSystem::UnmapBuffer(IndexBufferRef buffer)
{
	assert(IsValid(buffer));
	return unmapBuffer(*buffer, m_cache.CurrentIBO, GL_ELEMENT_ARRAY_BUFFER);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
[[nodiscard]] inline GLenum getAttributeType(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
		return GL_FLOAT;
	case GL_INT:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
		return GL_INT;
	}
	assert(false && "Unknown active attribute type!");
	return 0;
}
//-----------------------------------------------------------------------------
[[nodiscard]] inline GLint getAttributeSize(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
	case GL_INT:
		return 1;
	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
		return 2;
	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
		return 3;
	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
		return 4;
	}

	assert(false && "Unknown active attribute type!");
	return 0;
}
//-----------------------------------------------------------------------------
ShaderProgramRef RenderSystem::CreateShaderProgram(const ShaderBytecode& vertexShaderSource, const ShaderBytecode& fragmentShaderSource)
{
	if (!vertexShaderSource.IsValid())
	{
		LogError("You must provide vertex shader (source is blank).");
		return nullptr;
	}

	if (!fragmentShaderSource.IsValid())
	{
		LogError("You must provide fragment shader (source is blank).");
		return nullptr;
	}

	std::string vertexShaderSourceCode = vertexShaderSource.GetSource();
	std::string fragmentShaderSourceCode = fragmentShaderSource.GetSource();
	validationShaderCode(vertexShaderSourceCode, fragmentShaderSourceCode);

	ShaderRef glShaderVertex = compileShader(ShaderPipelineStage::Vertex, vertexShaderSourceCode);
	ShaderRef glShaderFragment = compileShader(ShaderPipelineStage::Fragment, fragmentShaderSourceCode);

	ShaderProgramRef resource;
	if (IsValid(glShaderVertex) && IsValid(glShaderFragment))
	{
		resource.reset(new ShaderProgram());

		glAttachShader(*resource, *glShaderVertex);
		glAttachShader(*resource, *glShaderFragment);
		glLinkProgram(*resource);
		GLint linkStatus = 0;
		glGetProgramiv(*resource, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			GLint infoLogLength;
			glGetProgramiv(*resource, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::unique_ptr<GLchar> errorInfoText{ new GLchar[static_cast<size_t>(infoLogLength + 1)] };
			glGetProgramInfoLog(*resource, infoLogLength, nullptr, errorInfoText.get());
			LogError("OPENGL: Shader program linking failed: " + std::string(errorInfoText.get()));
			resource.reset();
		}
		glDetachShader(*resource, *glShaderVertex);
		glDetachShader(*resource, *glShaderFragment);
	}
	glShaderVertex.reset();
	glShaderFragment.reset();

	return resource;
}
//-----------------------------------------------------------------------------
bool RenderSystem::IsReadyUniform(const Uniform& uniform) const
{
	return IsValid(uniform) && uniform.programId == m_cache.CurrentShaderProgram;
}
//-----------------------------------------------------------------------------
std::vector<ShaderAttributeInfo> RenderSystem::GetAttributesInfo(ShaderProgramRef program) const
{
	assert(IsValid(program));
	if (!IsValid(program)) return {};

	int activeAttribsCount = 0;
	glGetProgramiv(*program, GL_ACTIVE_ATTRIBUTES, &activeAttribsCount);
	int maxNameLength = 0;
	glGetProgramiv(*program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);

	std::string name;
	name.resize(static_cast<size_t>(maxNameLength));

	std::vector<ShaderAttributeInfo> attribs;
	for (size_t i = 0; i < static_cast<size_t>(activeAttribsCount); i++)
	{
		GLint size;
		GLenum type = 0;
		glGetActiveAttrib(*program, (GLuint)i, maxNameLength, nullptr, &size, &type, name.data());

		std::size_t pos = name.find("gl_VertexID"); // âûêèäàåòñÿ èç øåéäåðà òàê êàê location = -1
		if (pos != std::string::npos) continue;

		int location = glGetAttribLocation(*program, name.c_str());
		assert(location >= 0);

		attribs.emplace_back(ShaderAttributeInfo{
			.typeId = type,
			.type = getAttributeType(type),
			.numType = getAttributeSize(type),
			.name = name,
			.location = location
			});
	}

	std::sort(attribs.begin(), attribs.end(), [](const ShaderAttributeInfo& a, const ShaderAttributeInfo& b) {return a.location < b.location; });

	return attribs;
}
//-----------------------------------------------------------------------------
Uniform RenderSystem::GetUniform(ShaderProgramRef program, const char* uniformName) const
{
	if (!IsValid(program) || uniformName == nullptr) return {};

	if (m_cache.CurrentShaderProgram != *program) glUseProgram(*program);
	Uniform uniform = {
		.location = glGetUniformLocation(*program, uniformName),
		.programId = *program
	};
	// restore prev shader program
	if (m_cache.CurrentShaderProgram != *program) glUseProgram(m_cache.CurrentShaderProgram);
	return uniform;
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, bool value)
{
	assert(IsReadyUniform(uniform));
	glUniform1i(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, int value)
{
	assert(IsReadyUniform(uniform));
	glUniform1i(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, unsigned value)
{
	assert(IsReadyUniform(uniform));
	glUniform1ui(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, float value)
{
	assert(IsReadyUniform(uniform));
	glUniform1f(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec2& value)
{
	assert(IsReadyUniform(uniform));
	glUniform2fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec3& value)
{
	assert(IsReadyUniform(uniform));
	glUniform3fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec4& value)
{
	assert(IsReadyUniform(uniform));
	glUniform4fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::mat3& value)
{
	assert(IsReadyUniform(uniform));
	glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::mat4& value)
{
	assert(IsReadyUniform(uniform));
	glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform3(const Uniform& uniform, unsigned number, float* values)
{
	assert(IsReadyUniform(uniform));
	glUniform3fv(uniform.location, static_cast<GLsizei>(number), values);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform4(const Uniform& uniform, unsigned number, float* values)
{
	assert(IsReadyUniform(uniform));
	glUniform4fv(uniform.location, static_cast<GLsizei>(number), values);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, std::span<float> values)
{
	assert(IsReadyUniform(uniform));
	if (values.size() == 0) return;
	glUniform1fv(uniform.location, static_cast<GLsizei>(values.size()), (GLfloat*)values.data());
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, std::span<glm::vec2> values)
{
	assert(IsReadyUniform(uniform));
	if (values.size() == 0) return;
	glUniform2fv(uniform.location, static_cast<GLsizei>(values.size()), (GLfloat*)values.data());
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, std::span<glm::vec3> values)
{
	assert(IsReadyUniform(uniform));
	if (values.size() == 0) return;
	glUniform3fv(uniform.location, static_cast<GLsizei>(values.size()), (GLfloat*)values.data());
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, std::span<glm::vec4> values)
{
	assert(IsReadyUniform(uniform));
	if (values.size() == 0) return;
	glUniform4fv(uniform.location, static_cast<GLsizei>(values.size()), (GLfloat*)values.data());
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, bool value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1i(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, int value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1i(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, unsigned value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1ui(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, float value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1f(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec2& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform2fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec3& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform3fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec4& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform4fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::mat3& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniformMatrix3fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::mat4& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniformMatrix4fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(ShaderProgramRef resource)
{
	assert(IsValid(resource));
	if (m_cache.CurrentShaderProgram == *resource) return;
	m_cache.CurrentShaderProgram = *resource;
	glUseProgram(*resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(unsigned rawShader)
{
	if (m_cache.CurrentShaderProgram == rawShader) return;
	m_cache.CurrentShaderProgram = rawShader;
	glUseProgram(rawShader);
}
//-----------------------------------------------------------------------------
void RenderSystem::validationShaderCode(std::string& vertexCode, std::string& fragmentCode)
{
	//============================================================================
	// Add version
	//============================================================================
	size_t posVS = vertexCode.find("#version");
	if (posVS == std::string::npos)
		vertexCode = ShaderBytecode::GetHeaderVertexShader() + vertexCode;

	size_t posFS = fragmentCode.find("#version");
	if (posFS == std::string::npos)
		fragmentCode = ShaderBytecode::GetHeaderFragmentShader() + fragmentCode;
}
//-----------------------------------------------------------------------------
ShaderRef RenderSystem::compileShader(ShaderPipelineStage type, const std::string& source)
{
	const char* shaderText = source.data();
	const GLint lenShaderText = static_cast<GLint>(source.size());

	ShaderRef shader(new Shader(type));
	glShaderSource(*shader, 1, &shaderText, &lenShaderText);
	glCompileShader(*shader);

	GLint compileStatus = GL_FALSE;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::unique_ptr<GLchar> errorInfoText{ new GLchar[static_cast<size_t>(infoLogLength + 1)] };
		glGetShaderInfoLog(*shader, infoLogLength, nullptr, errorInfoText.get());

		const std::string shaderName = ConvertToStr(type);
		LogError(shaderName + " Shader compilation failed : " + std::string(errorInfoText.get()) + ", Source: " + source);
		return nullptr;
	}

	return shader;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RenderSystem::Bind(RasterizerState state)
{
	if (m_cache.CurrentRasterizerState.polygonMode != state.polygonMode)
	{
		m_cache.CurrentRasterizerState.polygonMode = state.polygonMode;
		glPolygonMode(GL_FRONT_AND_BACK, TranslateToGL(state.polygonMode));
	}
	if (m_cache.CurrentRasterizerState.depthClampEnabled != state.depthClampEnabled)
	{
		m_cache.CurrentRasterizerState.depthClampEnabled = state.depthClampEnabled;
		if (state.depthClampEnabled) glEnable(GL_DEPTH_CLAMP);
		else glDisable(GL_DEPTH_CLAMP);
	}
	if (m_cache.CurrentRasterizerState.multiSampleEnabled != state.multiSampleEnabled)
	{
		m_cache.CurrentRasterizerState.multiSampleEnabled = state.multiSampleEnabled;
		if (state.multiSampleEnabled) glEnable(GL_MULTISAMPLE);
		else glDisable(GL_MULTISAMPLE);
	}
	if (m_cache.CurrentRasterizerState.antiAliasedLineEnabled != state.antiAliasedLineEnabled)
	{
		m_cache.CurrentRasterizerState.antiAliasedLineEnabled = state.antiAliasedLineEnabled;
		if (state.antiAliasedLineEnabled) glEnable(GL_LINE_SMOOTH);
		else glDisable(GL_LINE_SMOOTH);
	}
	if (m_cache.CurrentRasterizerState.face != state.face)
	{
		m_cache.CurrentRasterizerState.face = state.face;
		glFrontFace(TranslateToGL(state.face));
	}
	if (m_cache.CurrentRasterizerState.lineWidth != state.lineWidth)
	{
		m_cache.CurrentRasterizerState.lineWidth = state.lineWidth;
		glLineWidth(state.lineWidth);
	}
	if (m_cache.CurrentRasterizerState.discardEnabled != state.discardEnabled)
	{
		m_cache.CurrentRasterizerState.discardEnabled = state.discardEnabled;
		if (state.discardEnabled) glEnable(GL_RASTERIZER_DISCARD);
		else glDisable(GL_RASTERIZER_DISCARD);
	}
	if (m_cache.CurrentRasterizerState.scissorTestEnabled != state.scissorTestEnabled)
	{
		m_cache.CurrentRasterizerState.scissorTestEnabled = state.scissorTestEnabled;
		if (state.scissorTestEnabled) glEnable(GL_SCISSOR_TEST);
		else glDisable(GL_SCISSOR_TEST);
	}
	if (m_cache.CurrentRasterizerState.cullMode != state.cullMode)
	{
		m_cache.CurrentRasterizerState.cullMode = state.cullMode;
		if (state.cullMode != RasterizerCullMode::FrontAndBack)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(TranslateToGL(state.cullMode));
		}
		else
			glDisable(GL_CULL_FACE);
	}
	if (m_cache.CurrentRasterizerState.depthBias.constantFactor != state.depthBias.constantFactor ||
		m_cache.CurrentRasterizerState.depthBias.slopeFactor != state.depthBias.slopeFactor ||
		m_cache.CurrentRasterizerState.depthBias.clamp != state.depthBias.clamp)
	{
		m_cache.CurrentRasterizerState.depthBias = state.depthBias;

		const bool polygonOffsetEnabled = IsPolygonOffsetEnabled(state.depthBias);
		if (polygonOffsetEnabled)
		{
			if (state.polygonMode == RasterizerFillMode::Solid) glEnable(GL_POLYGON_OFFSET_FILL);
			else if (state.polygonMode == RasterizerFillMode::Wireframe) glEnable(GL_POLYGON_OFFSET_LINE);
			else if (state.polygonMode == RasterizerFillMode::Point) glEnable(GL_POLYGON_OFFSET_POINT);

			if (OpenGLExtensions::version >= OPENGL46)
				glPolygonOffsetClamp(state.depthBias.slopeFactor, state.depthBias.constantFactor, state.depthBias.clamp);
			else
				glPolygonOffset(state.depthBias.slopeFactor, state.depthBias.constantFactor);
		}
		else
		{
			if (state.polygonMode == RasterizerFillMode::Solid) glDisable(GL_POLYGON_OFFSET_FILL);
			else if (state.polygonMode == RasterizerFillMode::Wireframe) glDisable(GL_POLYGON_OFFSET_LINE);
			else if (state.polygonMode == RasterizerFillMode::Point) glDisable(GL_POLYGON_OFFSET_POINT);
		}
	}
}
//-----------------------------------------------------------------------------