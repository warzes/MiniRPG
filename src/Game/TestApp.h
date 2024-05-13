#pragma once

class TestApp
{
public:
	void Run();

private:
	ShaderProgramRef m_shader;
	Uniform m_uniformQuadProjectionMatrix;
	Uniform m_uniformQuadWorldMatrix;
	glm::mat4 m_perspective;
	GeometryBufferRef m_geom;
	Texture2DRef m_texture;

	ShaderProgramRef m_shader2;
	Uniform m_uniformProjectionMatrix2;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;
	ShaderProgramRef screenShader;
	GeometryBufferRef m_geomBox;
	GeometryBufferRef m_geomPlane;
	GeometryBufferRef m_geomQuad;
	Texture2DRef cubeTexture;
	Texture2DRef floorTexture;
	Texture2DRef m_textureColorbuffer;
	RenderbufferRef m_rbo;
	FramebufferRef m_fbo;
	RenderCamera m_camera;
};