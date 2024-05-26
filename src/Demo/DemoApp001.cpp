#include "stdafx.h"
#include "DemoApp001.h"
#include "GraphicsSystem.h"

namespace
{
	struct GBuffer
	{
		void Create(RenderSystem& render, int inWidth, int inHeight)
		{
			Destroy();

			width = inWidth;
			height = inHeight;

			textureGBufferPosition = render.CreateTexture2D(GL_RGB16F, GL_RGB, width, height, nullptr, GL_NEAREST);
			textureGBufferNormal = render.CreateTexture2D(GL_RGB16F, GL_RGB, width, height, nullptr, GL_NEAREST);
			textureGBufferAlbedo = render.CreateTexture2D(GL_RGBA16F, GL_RGBA, width, height, nullptr, GL_NEAREST);
			//textureGBufferVelocity = render.CreateTexture2D(GL_RG16F, GL_RG, width, height, nullptr, GL_NEAREST);
			//textureGBufferEmissive = render.CreateTexture2D(GL_RGB8, GL_RGB, width, height, nullptr, GL_NEAREST);
			textureGBufferDepth = render.CreateTexture2D(GL_DEPTH_COMPONENT32, GL_DEPTH, width, height, nullptr, GL_NEAREST);

			fbo = render.CreateFramebuffer({ textureGBufferPosition, textureGBufferNormal, textureGBufferAlbedo/*, textureGBufferVelocity, textureGBufferEmissive*/ }, textureGBufferDepth);
		}

		void Destroy()
		{
			textureGBufferPosition.reset();
			textureGBufferNormal.reset();
			textureGBufferAlbedo.reset();
			//textureGBufferVelocity.reset();
			//textureGBufferEmissive.reset();
			textureGBufferDepth.reset();

			fbo.reset();
		}

		void Bind(RenderSystem& render)
		{
			const auto depthClearVal = 1.0f;
			render.FramebufferClear(fbo, GL_COLOR, 0, glm::value_ptr(glm::vec3(0.0f)));
			render.FramebufferClear(fbo, GL_COLOR, 1, glm::value_ptr(glm::vec3(0.0f)));
			render.FramebufferClear(fbo, GL_COLOR, 2, glm::value_ptr(glm::vec4(0.0f)));
			//render.FramebufferClear(fbo, GL_COLOR, 3, glm::value_ptr(glm::vec2(0.0f)));
			//render.FramebufferClear(fbo, GL_COLOR, 4, glm::value_ptr(glm::vec2(0.0f)));
			render.FramebufferClear(fbo, GL_DEPTH, 0, &depthClearVal);

			render.Bind(fbo);
		}

		GLFramebufferRef fbo = nullptr;

		GLTextureRef textureGBufferPosition = nullptr;
		GLTextureRef textureGBufferNormal = nullptr;
		GLTextureRef textureGBufferAlbedo = nullptr;
		//GLTextureRef textureGBufferVelocity = nullptr;
		//GLTextureRef textureGBufferEmissive = nullptr;

		GLTextureRef textureGBufferDepth = nullptr;

		int width = 0;
		int height = 0;
	};

	struct FinalFrameBuffer
	{
		void Create(RenderSystem& render, int width, int height)
		{
			Destroy();

			textureColor = render.CreateTexture2D(GL_RGB8, GL_RGB, width, height, nullptr, GL_NEAREST);
			fbo = render.CreateFramebuffer({ textureColor });
		}

		void Destroy()
		{
			textureColor.reset();
			fbo.reset();
		}

		void Bind(RenderSystem& render)
		{
			const auto depthClearCal = 1.0f;
			render.FramebufferClear(fbo, GL_COLOR, 0, glm::value_ptr(glm::vec3(0.2f, 0.6f, 1.0f)));
			render.FramebufferClear(fbo, GL_DEPTH, 0, &depthClearCal);

			render.Bind(fbo);
		}

		GLFramebufferRef fbo = nullptr;
		GLTextureRef textureColor = nullptr;
	};

#pragma region shader

#pragma region gbufferShader
	const char* gbufferVertSource = R"(
#version 460
#pragma debug(on)

out gl_PerVertex { vec4 gl_Position; };

out outBlock
{
	vec3 FragPosInViewSpace; // POSITION
	vec3 Color;
	vec3 Normal;
	vec2 TexCoords;
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
	const vec4 FragPosInViewSpace = view * modl * vec4(pos, 1.0);
	gl_Position = proj * FragPosInViewSpace;
	o.Color = col;
	o.TexCoords = uvs;
	o.Normal = normalize(mat3(transpose(inverse(view * modl))) * nrm);
	o.FragPosInViewSpace = FragPosInViewSpace.xyz;
}
		)";

	const char* gbufferFragSource = R"(
#version 460
#pragma debug(on)

in inBlock
{
	vec3 FragPosInViewSpace;
	vec3 Color;
	vec3 Normal;
	vec2 TexCoords;
} i;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedo;
//layout (location = 3) out vec2 outVelocity;
//layout (location = 4) out vec3 outEmissive;

//layout(location = 0) uniform float emissiveStrength;

layout(binding = 0) uniform sampler2D DiffuseTexture;

void main()
{
	vec4 diffuseTex = texture(DiffuseTexture, i.TexCoords);

	outPosition = i.FragPosInViewSpace;
	outNormal = i.Normal;
	outAlbedo.rgb = diffuseTex.rgb * i.Color;
	outAlbedo.a = diffuseTex.a;
}
		)";
#pragma endregion

#pragma region oldMainShader
	const char* oldMainVertSource = R"(
#version 460
#pragma debug(on)

out gl_PerVertex { vec4 gl_Position; };

out outBlock
{
	vec2 texcoord;
	vec3 ray;
} o;

layout(location = 0) uniform mat3 u_camera_direction;
layout(location = 1) uniform float u_fov;
layout(location = 2) uniform float u_ratio;
layout(location = 3) uniform vec2 u_uv_diff;

vec3 skyray(vec2 texcoord, float fovy, float aspect)
{
	float d = 0.5 / tan(fovy / 2.0);
	return normalize(vec3((texcoord.x - 0.5) * aspect, texcoord.y - 0.5, -d));
}

void main()
{
	vec2 v[] = {
		vec2(-1.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f,-1.0f),
		vec2(-1.0f,-1.0f)
	};
	vec2 t[] = {
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f)

	};
	uint i[] = { 0, 3, 2, 2, 1, 0 };

	const vec2 position = v[i[gl_VertexID]];
	const vec2 texcoord = t[i[gl_VertexID]];

	o.ray = u_camera_direction * skyray(texcoord, u_fov, u_ratio);
	o.texcoord = texcoord * u_uv_diff;
	gl_Position = vec4(position, 0.0, 1.0);
}
)";

	const char* oldMainFragSource = R"(
#version 460
#pragma debug(on)

layout (location = 0) out vec3 col;

// Textures
layout(binding = 0) uniform sampler2D tex_position;
layout(binding = 1) uniform sampler2D tex_normal;
layout(binding = 2) uniform sampler2D tex_albedo;
layout(binding = 3) uniform sampler2D tex_depth;
//layout(binding = 4) uniform sampler2D tex_emissive;
layout(binding = 4) uniform samplerCube texcube_skybox;

layout(location = 0) uniform vec3 u_camera_position;
// Point light
layout(location = 1) uniform vec3 light_col;
layout(location = 2) uniform vec3 light_pos;

in in_block
{
	vec2 texcoord;
	vec3 ray;
} i;

vec3 calculate_specular(float strength, vec3 color, vec3 view_pos, vec3 vert_pos, vec3 light_dir, vec3 normal)
{
    vec3 view_dir = normalize(view_pos - vert_pos);
    vec3 ref_dir = reflect(-light_dir, normal);

    float spec = pow(max(dot(view_dir, ref_dir), 0.0), 32.0);
    return strength * spec * color;
}

void main()
{

  const float depth = texture(tex_depth, i.texcoord).r;
  if (depth == 1.0)
  {
      col = texture(texcube_skybox, i.ray).rgb;
      return;
  }

  const vec3 position = texture(tex_position, i.texcoord).rgb;
  const vec3 normal = texture(tex_normal, i.texcoord).rgb;
  // Albedo = diffuse
  const vec4 albedo_specular = texture(tex_albedo, i.texcoord);
  const vec3 albedo = albedo_specular.rgb;
  const float specular = albedo_specular.a;

  vec3 light_dir = normalize(light_pos - position);
  float light_dif = max(dot(normal, light_dir), 0.0);


  vec3 ambient_col = vec3(0.1);

  // Final colors
  vec3 ambient = ambient_col * albedo;
  vec3 diffuse = light_col * light_dif * albedo;
  vec3 light_spec = calculate_specular(specular, light_col, u_camera_position, position, light_dir, normal);
  //vec3 emission = texture(tex_emissive, i.texcoord).rgb;

  //col = ambient + diffuse + light_spec + emission;
	col = ambient + diffuse + light_spec;
  col = col / (col + vec3(1.0));
}
)";
#pragma endregion


	
#pragma endregion
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

	GLProgramPipelineRef ppGBuffer = render.CreateProgramPipelineFromSources(gbufferVertSource, gbufferFragSource);
	GLProgramPipelineRef oldppMain = render.CreateProgramPipelineFromSources(oldMainVertSource, oldMainFragSource);
	constexpr auto uniform_projection = 0;
	constexpr auto uniform_view = 1;
	constexpr auto uniform_modl = 2;

	// main vertex
	constexpr auto uniform_cam_dir = 0;
	constexpr auto uniform_fov = 1;
	constexpr auto uniform_aspect = 2;
	constexpr auto uniform_uvs_diff = 3;

	// main fragment
	constexpr auto uniform_cam_pos = 0;
	constexpr auto uniform_light_col = 1;
	constexpr auto uniform_light_pos = 2;

	//constexpr auto uniform_lght = 3;
	//constexpr auto uniform_blur_bias = 0;

	//constexpr auto uniform_mvp = 3;
	//constexpr auto uniform_mvp_inverse = 4;
	//constexpr auto uniform_blur_except = 5;
	//constexpr auto uniform_emissive_strength = 0;

#pragma region deltaTime
	int fpsCount = 0;
	float timeFpsCount = 0;
	int timeBeg = clock();
#pragma endregion

	GModel* model = new GModel(systems, "Data/models/sponza/sponza.obj");
	GLVertexArrayRef VAOEmpty = render.CreateVertexArray();
	GBuffer gbuffer;
	gbuffer.Create(render, window.GetWidth(), window.GetHeight());
	FinalFrameBuffer finalFB;
	finalFB.Create(render, window.GetWidth(), window.GetHeight());

	GLTextureRef textureSkybox = render.CreateTextureCubeFromFile({
			"data/textures/TC_AboveClouds_Xn.png",
			"data/textures/TC_AboveClouds_Xp.png",
			"data/textures/TC_AboveClouds_Yn.png",
			"data/textures/TC_AboveClouds_Yp.png",
			"data/textures/TC_AboveClouds_Zn.png",
			"data/textures/TC_AboveClouds_Zp.png"
		});

	constexpr auto fov = glm::radians(60.0f);
	static glm::vec3 light_pos = glm::vec3(0.0, 4.0, 0.0);

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
				glViewport(0, 0, width, height);

				// resize framebuffer
				gbuffer.Create(render, width, height);
				finalFB.Create(render, width, height);
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
			
			// GBuffer
			{
				gbuffer.Bind(render);
				render.Bind(ppGBuffer);
				render.SetUniform(ppGBuffer->GetVertexShader(), uniform_projection, m_perspective);
				render.SetUniform(ppGBuffer->GetVertexShader(), uniform_view, m_camera.GetViewMatrix());
				render.SetUniform(ppGBuffer->GetVertexShader(), uniform_modl, glm::mat4(1.0f));
				model->Update(ppGBuffer);
			}

			// Final Frame
			{
				finalFB.Bind(render);
				render.BindSlot(gbuffer.textureGBufferPosition, 0);
				render.BindSlot(gbuffer.textureGBufferNormal, 1);
				render.BindSlot(gbuffer.textureGBufferAlbedo, 2);
				render.BindSlot(gbuffer.textureGBufferDepth, 3);
				//render.BindSlot(gbuffer.textureGBufferEmissive, 4);
				render.BindSlot(textureSkybox, 4);

				render.Bind(oldppMain);
				render.SetUniform(oldppMain->GetVertexShader(), uniform_cam_dir, glm::inverse(glm::mat3(m_camera.GetViewMatrix())));
				render.SetUniform(oldppMain->GetVertexShader(), uniform_fov, fov);
				render.SetUniform(oldppMain->GetVertexShader(), uniform_aspect, float(window.GetWidth()) / float(window.GetHeight()));
				render.SetUniform(oldppMain->GetVertexShader(), uniform_uvs_diff, glm::vec2(
					float(window.GetWidth()) / float(window.GetWidth()),
					float(window.GetHeight()) / float(window.GetHeight())
				));

				render.SetUniform(oldppMain->GetFragmentShader(), uniform_cam_pos, m_camera.position);
				render.SetUniform(oldppMain->GetFragmentShader(), uniform_light_col, glm::vec3(1.0));
				render.SetUniform(oldppMain->GetFragmentShader(), uniform_light_pos, light_pos);

				render.Bind(VAOEmpty);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			render.MainFrameBuffer();
			render.BlitFrameBuffer(finalFB.fbo, nullptr,
				0, 0, window.GetWidth(), window.GetHeight(),
				0, 0, window.GetWidth(), window.GetHeight(),
				GL_COLOR_BUFFER_BIT, GL_NEAREST);

			mainGUI.Draw();
		}
		render.EndFrame();
	}
	ppGBuffer.reset();
	gbuffer.Destroy();
	mainGUI.Destroy();
	graphics.Destroy();
	render.Destroy();
	window.Destroy();
	log.Destroy();
}


