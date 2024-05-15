#include "stdafx.h"
#include "GameApp.h"
#include "Model.h"
#include "Util.h"

// Shader code
const char* blurFragSource =
#include "blur.frag.glsl"
;

const char* blurVertSource =
#include "blur.vert.glsl"
;

const char* gbufferFragSource =
#include "gbuffer.frag.glsl"
;

const char* gbufferVertSource =
#include "gbuffer.vert.glsl"
;

const char* mainFragSource =
#include "main.frag.glsl"
;

const char* mainVertSource =
#include "main.vert.glsl"
;

enum class shape_t
{
	cube = 0,
	quad = 1
};

struct scene_object_t
{
	glm::mat4 model = glm::mat4(0.f);
	glm::mat4 mvp_inv_prev = glm::mat4(0.f);
	shape_t shape = shape_t::cube;
	// Excempt this object from being motion blurred
	bool except = false;
	float emissive_strength = 1.f;
	scene_object_t(shape_t shape, bool except = false, float emissive_strength = 1.f)
		: model(), mvp_inv_prev(), shape(shape), except(except), emissive_strength(emissive_strength)
	{}
};

std::vector<vertex_t> const vertices_cube =
{
	vertex_t(glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f), glm::vec2(0.0f, 0.0f)),
	vertex_t(glm::vec3(0.5f, 0.5f,-0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f), glm::vec2(1.0f, 0.0f)),
	vertex_t(glm::vec3(0.5f,-0.5f,-0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
	vertex_t(glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f,-1.0f), glm::vec2(0.0f, 1.0f)),

	vertex_t(glm::vec3(0.5f, 0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
	vertex_t(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
	vertex_t(glm::vec3(0.5f,-0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
	vertex_t(glm::vec3(0.5f,-0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),

	vertex_t(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
	vertex_t(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
	vertex_t(glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
	vertex_t(glm::vec3(0.5f,-0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),

	vertex_t(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
	vertex_t(glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
	vertex_t(glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
	vertex_t(glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),

	vertex_t(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
	vertex_t(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
	vertex_t(glm::vec3(0.5f, 0.5f,-0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
	vertex_t(glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),

	vertex_t(glm::vec3(0.5f,-0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
	vertex_t(glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
	vertex_t(glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
	vertex_t(glm::vec3(0.5f,-0.5f,-0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,-1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
};

std::vector<vertex_t> const	vertices_quad =
{
	vertex_t(glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
	vertex_t(glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
	vertex_t(glm::vec3(0.5f, 0.0f,-0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
	vertex_t(glm::vec3(-0.5f, 0.0f,-0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
};

std::vector<uint8_t> const indices_cube =
{
	0,   1,  2,  2,  3,  0,
	4,   5,  6,  6,  7,  4,
	8,   9, 10, 10, 11,  8,

	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20,
};

std::vector<uint8_t> const indices_quad =
{
	0,   1,  2,  2,  3,  0,
};

void GameApp::Run()
{
	Systems systems;
	LogSystem log(systems);
	WindowSystem window(systems);
	RenderSystem render(systems);

	log.Create({});
	window.Create({});
	render.Create({});

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDisable(GL_BLEND); // из-за настроек рендера не рисует, поэтому отключаю

	m_perspective = glm::perspective(glm::radians(60.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 1000.f);
	render.SetViewport(window.GetWidth(), window.GetHeight());

	glm::ivec2 lastMousePosition = Mouse::GetPosition(window);

	m_camera.Teleport({ 0.0f, 0.0f, -7.0f });

	//=============================================================================
	//=============================================================================

	GLTextureRef textureCubeDiffuse = render.CreateTexture2DFromFile("data/textures/T_Default_D.png", STBI_rgb, true);
	GLTextureRef textureCubeSpecular = render.CreateTexture2DFromFile("data/textures/T_Default_S.png", STBI_grey, true);
	GLTextureRef textureCubeNormal = render.CreateTexture2DFromFile("data/textures/T_Default_N.png", STBI_rgb, true);
	GLTextureRef textureCubeEmissive = render.CreateTexture2DFromFile("data/textures/T_Default_E.png", STBI_rgb, true);

	GLTextureRef textureSkybox = render.CreateTextureCubeFromFile({
			"data/textures/TC_AboveClouds_Xn.png",
			"data/textures/TC_AboveClouds_Xp.png",
			"data/textures/TC_AboveClouds_Yn.png",
			"data/textures/TC_AboveClouds_Yp.png",
			"data/textures/TC_AboveClouds_Zn.png",
			"data/textures/TC_AboveClouds_Zp.png"
		});

	int screen_width = window.GetWidth();
	int screen_height = window.GetHeight();

	/* framebuffer textures */
	GLTextureRef textureGBufferColor = render.CreateTexture2D(GL_RGB8, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);
	GLTextureRef textureGBufferPosition = render.CreateTexture2D(GL_RGB16F, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);
	GLTextureRef textureGBufferNormal = render.CreateTexture2D(GL_RGB16F, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);
	GLTextureRef textureGBufferAlbedo = render.CreateTexture2D(GL_RGBA16F, GL_RGBA, screen_width, screen_height, nullptr, GL_NEAREST);
	GLTextureRef textureGBufferDepth = render.CreateTexture2D(GL_DEPTH_COMPONENT32, GL_DEPTH, screen_width, screen_height, nullptr, GL_NEAREST);
	GLTextureRef textureGBufferVelocity = render.CreateTexture2D(GL_RG16F, GL_RG, screen_width, screen_height, nullptr, GL_NEAREST);
	GLTextureRef textureGBufferEmissive = render.CreateTexture2D(GL_RGB8, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);

	GLTextureRef textureMotionBlur = render.CreateTexture2D(GL_RGB8, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);
	GLTextureRef textureMotionBlurMask = render.CreateTexture2D(GL_R8, GL_RED, screen_width, screen_height, nullptr, GL_NEAREST);

	GLFramebufferRef fbGBuffer = render.CreateFramebuffer({ textureGBufferPosition, textureGBufferNormal, textureGBufferAlbedo, textureGBufferVelocity, textureGBufferEmissive }, textureGBufferDepth);
	GLFramebufferRef fbFinalColor = render.CreateFramebuffer({ textureGBufferColor });
	GLFramebufferRef fbBlur = render.CreateFramebuffer({ textureMotionBlur });

	/* vertex formatting information */
	std::vector<AttribFormat> const vertex_format =
	{
		create_attrib_format<glm::vec3>(0, offsetof(vertex_t, position)),
		create_attrib_format<glm::vec3>(1, offsetof(vertex_t, color)),
		create_attrib_format<glm::vec3>(2, offsetof(vertex_t, normal)),
		create_attrib_format<glm::vec2>(3, offsetof(vertex_t, texcoord))
	};

	/* geometry buffers */
	auto const vao_empty = [] { GLuint name = 0; glCreateVertexArrays(1, &name); return name; }();

	GLGeometryRef cubeGeom = render.CreateGeometry(vertices_cube, indices_cube, vertex_format);
	GLGeometryRef quadGeom = render.CreateGeometry(vertices_quad, indices_quad, vertex_format);

	/* shaders */
	GLProgramPipelineRef ppMain = render.CreateProgramPipelineFromSources(mainVertSource, mainFragSource);
	GLProgramPipelineRef ppGBuffer = render.CreateProgramPipelineFromSources(gbufferVertSource, gbufferFragSource);
	GLProgramPipelineRef ppBlur = render.CreateProgramPipelineFromSources(blurVertSource, blurFragSource);

	/* uniforms */
	constexpr auto uniform_projection = 0;
	constexpr auto uniform_cam_pos = 0;
	constexpr auto uniform_light_col = 1;
	constexpr auto uniform_light_pos = 2;
	constexpr auto uniform_cam_dir = 0;
	constexpr auto uniform_view = 1;
	constexpr auto uniform_fov = 1;
	constexpr auto uniform_aspect = 2;
	constexpr auto uniform_modl = 2;
	constexpr auto uniform_lght = 3;
	constexpr auto uniform_blur_bias = 0;
	constexpr auto uniform_uvs_diff = 3;
	constexpr auto uniform_mvp = 3;
	constexpr auto uniform_mvp_inverse = 4;
	constexpr auto uniform_blur_except = 5;
	constexpr auto uniform_emissive_strength = 0;

	render.SetUniform(ppGBuffer->GetVertexShader(), uniform_projection, m_perspective);

	std::vector<scene_object_t> objects = {
		scene_object_t(shape_t::cube),
		scene_object_t(shape_t::cube),
		scene_object_t(shape_t::cube),
		scene_object_t(shape_t::cube),
		scene_object_t(shape_t::cube),
		scene_object_t(shape_t::quad, false, 0.f)
	};

	double totalTime = 0.0;
	constexpr auto fov = glm::radians(60.0f);

	while (1)
	{
		if (window.ShouldQuit()) break;
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
				render.SetViewport(width, height);

				screen_width = window.GetWidth();
				screen_height = window.GetHeight();
			}

			if (const auto* mousePos = event.GetIf<Event::MouseMoved>())
			{
				const auto [x, y] = mousePos->position;
				//puts((std::to_string(x) + "/" + std::to_string(y)).c_str());
			}
		}

		float deltaTime = 0.01f;

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

		static glm::vec3 light_pos = glm::vec3(0.0, 4.0, 0.0);

		static float cube_speed = 0.1f;
		if (Keyboard::IsKeyPressed(Keyboard::Key::Q)) cube_speed -= 0.001f;
		if (Keyboard::IsKeyPressed(Keyboard::Key::E)) cube_speed += 0.001f;

		totalTime += deltaTime;

		/* light movement */
		light_pos.y = 4.0 + std::sin(totalTime) * 3.0;

		/* cube orbit */
		auto const orbit_center = glm::vec3(0.0f, 0.0f, 0.0f);
		static auto orbit_progression = 0.0f;

		/* rotate inner cube */
		objects[0].model = glm::translate(orbit_center) * glm::rotate(orbit_progression * cube_speed, glm::vec3(0.0f, 1.0f, 0.0f));

		for (auto i = 0; i < 4; i++)
		{
			auto const orbit_amount = (orbit_progression * cube_speed + float(i) * 90.0f * glm::pi<float>() / 180.0f);
			auto const orbit_pos = orbit_axis(orbit_amount, glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f)) + glm::vec3(-2.0f, 0.0f, 0.0f);
			objects[1 + i].model = glm::translate(orbit_center + orbit_pos) * glm::rotate(orbit_amount, glm::vec3(0.0f, -1.0f, 0.0f));
		}
		orbit_progression += 0.01f;

		objects[5].model = glm::translate(glm::vec3(0.0f, -3.0f, 0.0f)) * glm::scale(glm::vec3(10.0f, 1.0f, 10.0f));

		render.SetUniform(ppGBuffer->GetVertexShader(), uniform_view, m_camera.GetViewMatrix());

		// Draw
		render.BeginFrame();

		{
			/* g-buffer pass */
			static auto const viewport_width = screen_width;
			static auto const viewport_height = screen_height;
			glViewport(0, 0, viewport_width, viewport_height);

			auto const depth_clear_val = 1.0f;
			render.FramebufferClear(fbGBuffer, GL_COLOR, 0, glm::value_ptr(glm::vec3(1.0f)));
			render.FramebufferClear(fbGBuffer, GL_COLOR, 1, glm::value_ptr(glm::vec3(1.0f)));
			render.FramebufferClear(fbGBuffer, GL_COLOR, 2, glm::value_ptr(glm::vec4(1.0f)));
			render.FramebufferClear(fbGBuffer, GL_COLOR, 3, glm::value_ptr(glm::vec2(1.0f)));
			render.FramebufferClear(fbGBuffer, GL_COLOR, 4, glm::value_ptr(glm::vec2(1.0f)));
			render.FramebufferClear(fbGBuffer, GL_DEPTH, 0, &depth_clear_val);

			render.Bind(fbGBuffer);

			render.BindSlot(textureCubeDiffuse, 0);
			render.BindSlot(textureCubeSpecular, 1);
			render.BindSlot(textureCubeNormal, 2);
			render.BindSlot(textureCubeEmissive, 3);

			render.Bind(ppGBuffer);

			for (auto& object : objects)
			{
				switch (object.shape)
				{
				case shape_t::cube: render.Bind(cubeGeom); break;
				case shape_t::quad: render.Bind(quadGeom); break;
				}

				auto const curr_mvp_inv = m_perspective * m_camera.GetViewMatrix() * object.model;

				render.SetUniform(ppGBuffer->GetVertexShader(), uniform_modl, object.model);
				render.SetUniform(ppGBuffer->GetVertexShader(), uniform_mvp, curr_mvp_inv);
				render.SetUniform(ppGBuffer->GetVertexShader(), uniform_mvp_inverse, object.mvp_inv_prev);
				render.SetUniform(ppGBuffer->GetVertexShader(), uniform_blur_except, object.except);
				render.SetUniform(ppGBuffer->GetFragmentShader(), uniform_emissive_strength, object.emissive_strength);

				object.mvp_inv_prev = curr_mvp_inv;

				for (auto const& object : objects)
				{
					switch (object.shape)
					{
					case shape_t::cube: glDrawElements(GL_TRIANGLES, indices_cube.size(), GL_UNSIGNED_BYTE, nullptr); break;
					case shape_t::quad: glDrawElements(GL_TRIANGLES, indices_quad.size(), GL_UNSIGNED_BYTE, nullptr); break;
					}
				}
			}

			/* actual shading pass */
			render.FramebufferClear(fbFinalColor, GL_COLOR, 0, glm::value_ptr(glm::vec3(0.2f, 0.6f, 1.0f)));
			render.FramebufferClear(fbFinalColor, GL_DEPTH, 0, &depth_clear_val);

			render.Bind(fbFinalColor);

			render.BindSlot(textureGBufferPosition, 0);
			render.BindSlot(textureGBufferNormal, 1);
			render.BindSlot(textureGBufferAlbedo, 2);
			render.BindSlot(textureGBufferDepth, 3);
			render.BindSlot(textureGBufferEmissive, 4);
			render.BindSlot(textureSkybox, 5);

			render.Bind(ppMain);
			glBindVertexArray(vao_empty);

			render.SetUniform(ppMain->GetFragmentShader(), uniform_cam_pos, m_camera.position);
			render.SetUniform(ppMain->GetFragmentShader(), uniform_light_col, glm::vec3(1.0));
			render.SetUniform(ppMain->GetFragmentShader(), uniform_light_pos, light_pos);
			render.SetUniform(ppMain->GetVertexShader(), uniform_cam_dir, glm::inverse(glm::mat3(m_camera.GetViewMatrix())));
			render.SetUniform(ppMain->GetVertexShader(), uniform_fov, fov);
			render.SetUniform(ppMain->GetVertexShader(), uniform_aspect, float(viewport_width) / float(viewport_height));
			render.SetUniform(ppMain->GetVertexShader(), uniform_uvs_diff, glm::vec2(
				float(viewport_width) / float(screen_width),
				float(viewport_height) / float(screen_height)
			));

			glDrawArrays(GL_TRIANGLES, 0, 6);

			/* motion blur */

			render.FramebufferClear(fbBlur, GL_COLOR, 0, glm::value_ptr(glm::vec3(0.0f)));

			render.Bind(fbBlur);

			render.BindSlot(textureGBufferColor, 0);
			render.BindSlot(textureGBufferVelocity, 1);

			render.Bind(ppBlur);
			glBindVertexArray(vao_empty);

			render.SetUniform(ppBlur->GetFragmentShader(), uniform_blur_bias, 0.03f/*float(fps_sum) / float(60)*/);
			render.SetUniform(ppBlur->GetVertexShader(), uniform_uvs_diff, glm::vec2(
				float(viewport_width) / float(screen_width),
				float(viewport_height) / float(screen_height)
			));

			glDrawArrays(GL_TRIANGLES, 0, 6);

			/* scale raster */
			glViewport(0, 0, window.GetWidth(), window.GetHeight());

			render.MainFrameBuffer();
			render.BlitFrameBuffer(fbBlur, nullptr, 
				0, 0, viewport_width, viewport_height, 
				0, 0, window.GetWidth(), window.GetHeight(), 
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
		render.EndFrame();
	}

	cubeGeom.reset();
	quadGeom.reset();
	ppMain.reset();
	ppGBuffer.reset();
	ppBlur.reset();
	textureCubeDiffuse.reset();
	textureCubeSpecular.reset();
	textureCubeNormal.reset();
	textureCubeEmissive.reset();
	textureSkybox.reset();

	textureGBufferPosition.reset();
	textureGBufferAlbedo.reset();
	textureGBufferNormal.reset();
	textureGBufferDepth.reset();
	textureGBufferColor.reset();
	textureMotionBlur.reset();
	textureMotionBlurMask.reset();

	fbGBuffer.reset();
	fbFinalColor.reset();
	fbBlur.reset();

	render.Destroy();
	window.Destroy();
	log.Destroy();
}