#include "stdafx.h"
#include "DemoApp001.h"
#include "GraphicsSystem.h"

namespace
{
	const char* mainVertSource = R"(
#version 460
#pragma debug(on)

out gl_PerVertex { vec4 gl_Position; };

out outBlock
{
	vec3 FragPosInViewSpace; // POSITION
	vec3 Normal;
	vec2 TexCoords;
} o;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec2 uvs;

layout (location = 0) uniform mat4 proj;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 modl;

void main()
{
	const vec4 FragPosInViewSpace = view * modl * vec4(pos, 1.0);
	gl_Position = proj * FragPosInViewSpace;
	o.TexCoords = uvs;
	o.Normal = normalize(mat3(transpose(inverse(view * modl))) * nrm);
	o.FragPosInViewSpace = FragPosInViewSpace.xyz;
}
)";

	const char* mainFragSource = R"(
#version 460
#pragma debug(on)

in inBlock
{
	vec3 FragPosInViewSpace; // POSITION
	vec3 Normal;
	vec2 TexCoords;
} i;

layout(binding = 0) uniform sampler2D dif;
layout(binding = 1) uniform sampler2D spc;
layout(binding = 2) uniform sampler2D nrm;
layout(binding = 3) uniform sampler2D emi;

layout (location = 0) out vec4 outColor;

void main()
{
	outColor = texture(dif, i.TexCoords);
}
)";
}

void DemoApp001::Run()
{
	Systems systems;
	LogSystem log(systems);
	WindowSystem window(systems);
	RenderSystem render(systems);
	GraphicsSystem graphics(systems);
	MainGUISystem mainGUI(systems);

	log.Create({});
	window.Create({});
	render.Create({});
	graphics.Create({});
	mainGUI.Create({});

	//graphics.RegisterObject(std::make_shared<DA001_HalfCornellBox>(systems, "HalfCornellBox", 1));

	m_perspective = glm::perspective(glm::radians(60.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 1000.f);
	glViewport(0, 0, window.GetWidth(), window.GetHeight());

	glm::ivec2 lastMousePosition = Mouse::GetPosition(window);

	m_camera.Teleport({ 0.0f, 0.3f, -1.0f });

	glEnable(GL_DEPTH_TEST);

	GLProgramPipelineRef ppMain = render.CreateProgramPipelineFromSources(mainVertSource, mainFragSource);
	constexpr auto uniform_projection = 0;
	constexpr auto uniform_view = 1;
	constexpr auto uniform_modl = 2;

#pragma region deltaTime
	int fpsCount = 0;
	float timeFpsCount = 0;
	int timeBeg = clock();
#pragma endregion

	GModel* model = new GModel(systems, "Data/models/sponza/sponza.obj");

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

		mainGUI.Update();
		{
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static int counter = 0;

				ImGui::Begin("Hello, world!");
				//ImGui::PushFont(font1);
				ImGui::Text((const char*)u8"Test/Тест/%s", u8"тест 2");
				//ImGui::PopFont();
				if (ImGui::Button("Button"))
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}
		}

		graphics.Update();

		// Draw
		render.BeginFrame();
		{
			glViewport(0, 0, window.GetWidth(), window.GetHeight());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			graphics.Frame(false);

			render.Bind(ppMain);
			render.SetUniform(ppMain->GetVertexShader(), uniform_projection, m_perspective);
			render.SetUniform(ppMain->GetVertexShader(), uniform_view, m_camera.GetViewMatrix());
			render.SetUniform(ppMain->GetVertexShader(), uniform_modl, glm::mat4(1.0f));

			model->Update(ppMain);

			mainGUI.Draw();
		}
		render.EndFrame();
	}
	mainGUI.Destroy();
	graphics.Destroy();
	render.Destroy();
	window.Destroy();
	log.Destroy();
}


