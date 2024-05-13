#include "stdafx.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#	pragma comment( lib, "Engine.lib" )
#endif
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	Systems systems;
	LogSystem log(systems);
	WindowSystem window(systems);
	RenderSystem render(systems);

	log.Create({});
	window.Create({});
	render.Create({});

	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	glm::mat4 m_perspective;
	GeometryBufferRef m_geom;
	Texture2DRef m_texture;

	const char* vertexShaderText = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 ProjectionMatrix;

out vec2 TexCoord;

void main()
{
	gl_Position = ProjectionMatrix * vec4(aPosition, 1.0);
	TexCoord    = aTexCoord;
}
)";

	const char* fragmentShaderText = R"(
in vec2 TexCoord;
out vec4 FragmentColor;

uniform sampler2D DiffuseTexture;

void main()
{
	FragmentColor = texture(DiffuseTexture, TexCoord);
}
)";

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
	} vert[] =
	{
		{{ -0.2f,  0.2f, 2.0f}, {0.0f, 0.0f}}, // top left
		{{  0.2f,  0.2f, 2.0f}, {1.0f, 0.0f}}, // top right
		{{  0.2f, -0.2f, 2.0f}, {1.0f, 1.0f}}, // bottom right
		{{ -0.2f, -0.2f, 2.0f}, {0.0f, 1.0f}}, // bottom left
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 3, 2,   // first triangle
		2, 1, 0    // second triangle
	};

	glEnable(GL_CULL_FACE); // для теста - квад выше против часой стрелки

	m_shader = render.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = render.GetUniform(m_shader, "ProjectionMatrix");

	m_geom = render.CreateGeometryBuffer(BufferUsage::StaticDraw,
		static_cast<unsigned>(Countof(vert)), static_cast<unsigned>(sizeof(Vertex)), vert,
		static_cast<unsigned>(Countof(indices)), IndexFormat::UInt32, indices,
		m_shader);

	m_texture = render.CreateTexture2D("Data/textures/texel_checker.png");

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

	{
		const char* vertexShaderText2 = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
TexCoords = aTexCoords;
gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

		const char* fragmentShaderText2 = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
FragColor = texture(texture1, TexCoords);
}
)";

		m_shader2 = render.CreateShaderProgram({ vertexShaderText2 }, { fragmentShaderText2 });
		m_uniformProjectionMatrix2 = render.GetUniform(m_shader2, "projection");
		m_uniformViewMatrix = render.GetUniform(m_shader2, "view");
		m_uniformWorldMatrix = render.GetUniform(m_shader2, "model");
	}

	{
		const char* vertexShaderText2 = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
TexCoords = aTexCoords;
gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  
)";

		const char* fragmentShaderText2 = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, TexCoords);
	float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	FragColor = vec4(average, average, average, 1.0);
} 
)";
		screenShader = render.CreateShaderProgram({ vertexShaderText2 }, { fragmentShaderText2 });
	}

	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	m_geomBox = render.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(cubeVertices), (unsigned)sizeof(float), cubeVertices, m_shader2);

	float planeVertices[] = {
		// positions          // texture Coords 
		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	m_geomPlane = render.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(planeVertices), (unsigned)sizeof(float), planeVertices, m_shader2);

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates. NOTE that this plane is now much smaller and at the top of the screen
		// positions   // texCoords
		-0.3f,  1.0f,  0.0f, 1.0f,
		-0.3f,  0.5f,  0.0f, 0.0f,
		 0.3f,  0.5f,  1.0f, 0.0f,

		-0.3f,  1.0f,  0.0f, 1.0f,
		 0.3f,  0.5f,  1.0f, 0.0f,
		 0.3f,  1.0f,  1.0f, 1.0f
	};
	m_geomQuad = render.CreateGeometryBuffer(BufferUsage::StaticDraw, (unsigned)Countof(quadVertices), (unsigned)sizeof(float), quadVertices, screenShader);

	cubeTexture = render.CreateTexture2D("Data/textures/texel_checker.png");
	floorTexture = render.CreateTexture2D("Data/textures/1mx1m.png");

	{
		Texture2DCreateInfo createInfo;
		createInfo.format = TexelsFormat::RGB_U8;
		createInfo.width = (uint16_t)window.GetWidth();
		createInfo.height = (uint16_t)window.GetHeight();
		Texture2DInfo info;
		info.magFilter = TextureMagFilter::Linear;
		info.minFilter = TextureMinFilter::Linear;
		info.mipmap = false;
		m_textureColorbuffer = render.CreateTexture2D(createInfo, info);
		m_rbo = render.CreateRenderbuffer({ window.GetWidth(),  window.GetHeight() }, ImageFormat::D24S8);

		m_fbo = render.CreateFramebuffer(m_textureColorbuffer, m_rbo);
	}

	while (1)
	{
		if (window.ShouldQuit()) break;
		if (window.PeekMessages())
			continue;

		// Update
		{
	/*		const float mouseSensitivity = 10.0f * deltaTime;
			const float moveSpeed = 10.0f * deltaTime;
			const glm::vec3 oldCameraPos = m_camera.position;

			if (GetInputSystem().IsKeyDown(Input::KEY_W)) m_camera.MoveBy(moveSpeed);
			if (GetInputSystem().IsKeyDown(Input::KEY_S)) m_camera.MoveBy(-moveSpeed);
			if (GetInputSystem().IsKeyDown(Input::KEY_A)) m_camera.StrafeBy(moveSpeed);
			if (GetInputSystem().IsKeyDown(Input::KEY_D)) m_camera.StrafeBy(-moveSpeed);

			glm::vec2 delta = GetInputSystem().GetMouseDeltaPosition();
			if (delta.x != 0.0f)  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
			if (delta.y != 0.0f)  m_camera.RotateUpDown(-delta.y * mouseSensitivity);*/
		}

		// Draw
		render.BeginFrame();

		{
			m_perspective = glm::perspective(glm::radians(45.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.01f, 100.f);
			render.SetViewport(window.GetWidth(), window.GetHeight());

			{
				glm::mat4 model = glm::mat4(1.0f);

				render.Bind(m_fbo);
				render.SetClearColor({ 0.1f, 0.1f, 0.1f });
				render.ClearFrame();
				glEnable(GL_DEPTH_TEST);

				render.Bind(m_shader2);
				render.SetUniform(m_uniformProjectionMatrix2, m_perspective);
				render.SetUniform(m_uniformViewMatrix, m_camera.GetViewMatrix());

				// cubes
				render.Bind(cubeTexture, 0);
				model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
				render.SetUniform(m_uniformWorldMatrix, model);
				render.Draw(m_geomBox->vao);
				model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
				render.SetUniform(m_uniformWorldMatrix, model);
				render.Draw(m_geomBox->vao);
				// floor
				render.Bind(floorTexture, 0);
				render.SetUniform(m_uniformWorldMatrix, glm::mat4(1.0f));
				render.Draw(m_geomPlane->vao);

				render.ResetState(ResourceType::Framebuffer);
				render.SetClearColor({ 0.1f, 0.1f, 0.1f });
				render.ClearFrame();
				// cubes
				render.Bind(cubeTexture, 0);
				model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
				render.SetUniform(m_uniformWorldMatrix, model);
				render.Draw(m_geomBox->vao);
				model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
				render.SetUniform(m_uniformWorldMatrix, model);
				render.Draw(m_geomBox->vao);
				// floor
				render.Bind(floorTexture, 0);
				render.SetUniform(m_uniformWorldMatrix, glm::mat4(1.0f));
				render.Draw(m_geomPlane->vao);

				glDisable(GL_DEPTH_TEST);
				render.Bind(screenShader);
				render.Bind(m_fbo->colorTextures[0], 0);
				render.Draw(m_geomQuad->vao);
			}

	/*		render.SetClearColor({ 0.0f, 0.64f, 0.91f });
			render.SetViewport(window.GetWidth(), window.GetHeight());
			render.ClearFrame();*/

			{
				render.Bind(m_texture, 0);
				render.Bind(m_shader);
				render.SetUniform(m_uniformProjectionMatrix, m_perspective);
				render.SetUniform("DiffuseTexture", 0);
				render.Draw(m_geom);
			}
		}
		render.EndFrame();
	}

	m_shader2.reset();
	screenShader.reset();
	m_geomBox.reset();
	m_geomPlane.reset();
	m_geomQuad.reset();
	cubeTexture.reset();
	floorTexture.reset();

	m_textureColorbuffer.reset();
	m_rbo.reset();
	m_fbo.reset();

	m_shader.reset();
	m_geom.reset();
	m_texture.reset();

	render.Destroy();
	window.Destroy();
	log.Destroy();
}
//-----------------------------------------------------------------------------