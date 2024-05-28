#include "stdafx.h"
#include "GameApp.h"

namespace
{
	template<typename T>
	constexpr std::pair<GLint, GLenum> type_to_size_enum()
	{
		if constexpr (std::is_same_v<T, float>)
			return std::make_pair(1, GL_FLOAT);
		if constexpr (std::is_same_v<T, int>)
			return std::make_pair(1, GL_INT);
		if constexpr (std::is_same_v<T, unsigned int>)
			return std::make_pair(1, GL_UNSIGNED_INT);
		if constexpr (std::is_same_v<T, glm::vec2>)
			return std::make_pair(2, GL_FLOAT);
		if constexpr (std::is_same_v<T, glm::vec3>)
			return std::make_pair(3, GL_FLOAT);
		if constexpr (std::is_same_v<T, glm::vec4>)
			return std::make_pair(4, GL_FLOAT);
		throw std::runtime_error("unsupported type");
	}

	template<typename T>
	inline AttribFormat create_attrib_format(GLuint attrib_index, GLuint relative_offset)
	{
		auto const [comp_count, type] = type_to_size_enum<T>();
		return AttribFormat{ attrib_index, comp_count, type, relative_offset };
	}

	struct vertex
	{
		vertex(glm::vec3 const& position, glm::vec3 const& color, glm::vec3 const& normal, glm::vec2 const& texcoord) : position(position), color(color), normal(normal), texcoord(texcoord) {}

		glm::vec3 position = glm::vec3(0.f);
		glm::vec3 color = glm::vec3(0.f);
		glm::vec3 normal = glm::vec3(0.f);
		glm::vec2 texcoord = glm::vec2(0.f);
	};

	const std::vector<vertex> verticesQuad =
	{
		vertex(glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		vertex(glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec3(0.5f, 0.0f,-0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec3(-0.5f, 0.0f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
	};
	const std::vector<uint8_t> indicesQuad =
	{
		0, 1, 2, 2, 3, 0,
	};

	const char* mainVertSource = R"(
#version 460
#pragma debug(on)

out gl_PerVertex { vec4 gl_Position; };

out out_block
{
	vec3 pos;
	vec3 col;
	vec3 nrm;
	vec2 uvs;
} o;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
layout (location = 2) in vec3 nrm;
layout (location = 3) in vec2 uvs;

layout (location = 0) uniform mat4 proj;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 modl;

void main()
{
	const vec4 mpos = (view * modl * vec4(pos, 1.0));
	o.pos = (modl * vec4(pos, 1.0)).xyz;
	o.col = col;
	o.nrm = mat3(transpose(inverse(modl))) * nrm;
	o.uvs = uvs;
	gl_Position = proj * mpos;
}
)";

	const char* mainFragSource = R"(
#version 460
#pragma debug(on)

in in_block
{
	vec3 pos;
	vec3 col;
	vec3 nrm;
	vec2 uvs;
} i;

layout (location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D dif;
layout(binding = 1) uniform sampler2D spc;
layout(binding = 2) uniform sampler2D nrm;
layout(binding = 3) uniform sampler2D emi;

void main()
{
	outColor.rgb = texture(dif, i.uvs).rgb + i.pos - i.nrm;
	outColor.rgb = outColor.rgb * i.col;
	outColor.a = 1.0;
}
)";
}

void GameApp::Run()
{
	Systems systems;
	LogSystem log(systems);
	WindowSystem window(systems);
	RenderSystem render(systems);

	log.Create({});
	window.Create({});
	render.Create({});

	m_perspective = glm::perspective(glm::radians(60.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 1000.f);
	glViewport(0, 0, window.GetWidth(), window.GetHeight());

	glm::ivec2 lastMousePosition = Mouse::GetPosition(window);

	m_camera.Teleport({ 0.0f, 0.3f, -1.0f });

	glEnable(GL_DEPTH_TEST);

	GLTextureRef textureCubeDiffuse = render.CreateTexture2DFromFile("data/textures/T_Default_D.png", STBI_rgb, true);
	GLTextureRef textureCubeSpecular = render.CreateTexture2DFromFile("data/textures/T_Default_S.png", STBI_grey, true);
	GLTextureRef textureCubeNormal = render.CreateTexture2DFromFile("data/textures/T_Default_N.png", STBI_rgb, true);
	GLTextureRef textureCubeEmissive = render.CreateTexture2DFromFile("data/textures/T_Default_E.png", STBI_rgb, true);

	const std::vector<AttribFormat> vertexFormat =
	{
		create_attrib_format<glm::vec3>(0, offsetof(vertex, position)),
		create_attrib_format<glm::vec3>(1, offsetof(vertex, color)),
		create_attrib_format<glm::vec3>(2, offsetof(vertex, normal)),
		create_attrib_format<glm::vec2>(3, offsetof(vertex, texcoord))
	};

	GLGeometryRef quadGeom = render.CreateGeometry(verticesQuad, indicesQuad, vertexFormat);
	GLProgramPipelineRef ppMain = render.CreateProgramPipelineFromSources(mainVertSource, mainFragSource);
	constexpr auto uniform_projection = 0;
	constexpr auto uniform_view = 1;
	constexpr auto uniform_modl = 2;

#pragma region deltaTime
	int fpsCount = 0;
	float timeFpsCount = 0;
	int timeBeg = clock();
#pragma endregion

	while (1)
	{
		if (window.ShouldQuit()) break;

#pragma region deltatime
		int timeEnd = clock();
		float deltaTime = (timeEnd - timeBeg) / 1000.f;
		timeBeg = clock();

		timeFpsCount += deltaTime;
		fpsCount++;
		if (timeFpsCount > 1)
		{
			timeFpsCount -= 1;
			fpsCount = 0;
		}
#pragma endregion

		// Process events
		while (const auto event = window.PollEvent())
		{
			// Close window: exit
			if (event.Is<Event::Closed>())
				Exit();

			// Escape key: exit
			if (const auto* keyPressed = event.GetIf<Event::KeyPressed>())
				if (keyPressed->code == Keyboard::Key::Escape)
					Exit();

			// Resize event: adjust the viewport
			if (const auto* resized = event.GetIf<Event::Resized>())
			{
				const auto [width, height] = resized->size;
				m_perspective = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 1000.f);
				glViewport(0, 0, window.GetWidth(), window.GetHeight());

				// resize framebuffer
				
			}

			if (const auto* mousePos = event.GetIf<Event::MouseMoved>())
			{
				const auto [x, y] = mousePos->position;
				//puts((std::to_string(x) + "/" + std::to_string(y)).c_str());
			}
		}

		// Update
		{
			const float mouseSensitivity = 10.0f * deltaTime;
			const float moveSpeed = 10.0f * deltaTime;
			const glm::vec3 oldCameraPos = m_camera.position;

			auto change = Mouse::GetPosition(window) - lastMousePosition;

			if (Keyboard::IsKeyPressed(Keyboard::Key::W)) m_camera.MoveBy(moveSpeed);
			if (Keyboard::IsKeyPressed(Keyboard::Key::S)) m_camera.MoveBy(-moveSpeed);
			if (Keyboard::IsKeyPressed(Keyboard::Key::A)) m_camera.StrafeBy(moveSpeed);
			if (Keyboard::IsKeyPressed(Keyboard::Key::D)) m_camera.StrafeBy(-moveSpeed);

			if (Mouse::IsButtonPressed(Mouse::Button::Right))
			{
				window.SetMouseCursorVisible(false);
				window.SetMouseCursorGrabbed(true);
				Mouse::SetPositionCentre(window);

				lastMousePosition = Mouse::GetPosition(window);

				if (change.x != 0.0f)  m_camera.RotateLeftRight(change.x * mouseSensitivity);
				if (change.y != 0.0f)  m_camera.RotateUpDown(-change.y * mouseSensitivity);
			}
			else
			{
				window.SetMouseCursorVisible(true);
				window.SetMouseCursorGrabbed(false);
			}
		}

		// Draw
		render.BeginFrame();
		{
			glViewport(0, 0, window.GetWidth(), window.GetHeight());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			render.BindSlot(textureCubeDiffuse, 0);
			render.BindSlot(textureCubeSpecular, 1);
			render.BindSlot(textureCubeNormal, 2);
			render.BindSlot(textureCubeEmissive, 3);

			render.Bind(ppMain);
			render.SetUniform(ppMain->GetVertexShader(), uniform_projection, m_perspective);
			render.SetUniform(ppMain->GetVertexShader(), uniform_view, m_camera.GetViewMatrix());
			render.SetUniform(ppMain->GetVertexShader(), uniform_modl, glm::mat4(1.0f));

			render.Bind(quadGeom);

			glDrawElements(GL_TRIANGLES, indicesQuad.size(), GL_UNSIGNED_BYTE, nullptr);
		}
		render.EndFrame();
	}

	render.Destroy();
	window.Destroy();
	log.Destroy();
}