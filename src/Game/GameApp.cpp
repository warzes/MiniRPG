#include "stdafx.h"
#include "GameApp.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Util.h"

using glDeleterFunc = void (APIENTRY*)(GLuint item);
using glDeleterFuncv = void (APIENTRY*)(GLsizei n, const GLuint* items);
inline void delete_items(glDeleterFuncv deleter, std::initializer_list<GLuint> items) { deleter(items.size(), items.begin()); }
inline void delete_items(glDeleterFunc deleter, std::initializer_list<GLuint> items)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		deleter(*(items.begin() + i));
	}
}


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

	m_camera.Teleport({ 0.0f, 0.0f, -7.0f }, { 0.0f, 0.0f, 1.0f });

	//=============================================================================
	//=============================================================================

	auto const texture_cube_diffuse = create_texture_2d_from_file("data/textures/T_Default_D.png", STBI_rgb, true);
	auto const texture_cube_specular = create_texture_2d_from_file("data/textures/T_Default_S.png", STBI_grey, true);
	auto const texture_cube_normal = create_texture_2d_from_file("data/textures/T_Default_N.png", STBI_rgb, true);
	auto const texture_cube_emissive = create_texture_2d_from_file("data/textures/T_Default_E.png", STBI_rgb, true);
	auto const texture_skybox = create_texture_cube_from_file({
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
	auto const texture_gbuffer_color = create_texture_2d(GL_RGB8, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);

	auto const texture_gbuffer_position = create_texture_2d(GL_RGB16F, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);
	auto const texture_gbuffer_normal = create_texture_2d(GL_RGB16F, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);
	auto const texture_gbuffer_albedo = create_texture_2d(GL_RGBA16F, GL_RGBA, screen_width, screen_height, nullptr, GL_NEAREST);
	auto const texture_gbuffer_depth = create_texture_2d(GL_DEPTH_COMPONENT32, GL_DEPTH, screen_width, screen_height, nullptr, GL_NEAREST);
	auto const texture_gbuffer_velocity = create_texture_2d(GL_RG16F, GL_RG, screen_width, screen_height, nullptr, GL_NEAREST);
	auto const texture_gbuffer_emissive = create_texture_2d(GL_RGB8, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);

	auto const texture_motion_blur = create_texture_2d(GL_RGB8, GL_RGB, screen_width, screen_height, nullptr, GL_NEAREST);
	auto const texture_motion_blur_mask = create_texture_2d(GL_R8, GL_RED, screen_width, screen_height, nullptr, GL_NEAREST);

	auto const fb_gbuffer = create_framebuffer({ texture_gbuffer_position, texture_gbuffer_normal, texture_gbuffer_albedo, texture_gbuffer_velocity, texture_gbuffer_emissive }, texture_gbuffer_depth);
	auto const fb_finalcolor = create_framebuffer({ texture_gbuffer_color });
	auto const fb_blur = create_framebuffer({ texture_motion_blur });

	/* vertex formatting information */
	std::vector<attrib_format_t> const vertex_format =
	{
		create_attrib_format<glm::vec3>(0, offsetof(vertex_t, position)),
		create_attrib_format<glm::vec3>(1, offsetof(vertex_t, color)),
		create_attrib_format<glm::vec3>(2, offsetof(vertex_t, normal)),
		create_attrib_format<glm::vec2>(3, offsetof(vertex_t, texcoord))
	};

	/* geometry buffers */
	auto const vao_empty = [] { GLuint name = 0; glCreateVertexArrays(1, &name); return name; }();
	auto const [vao_cube, vbo_cube, ibo_cube] = create_geometry(vertices_cube, indices_cube, vertex_format);
	auto const [vao_quad, vbo_quad, ibo_quad] = create_geometry(vertices_quad, indices_quad, vertex_format);

	/* shaders */
	auto const [pr_main, vert_shader, frag_shader] = create_program_from_sources(mainVertSource, mainFragSource);
	auto const [pr_gbuffer, vert_shader_g, frag_shader_g] = create_program_from_sources(gbufferVertSource, gbufferFragSource);
	auto const [pr_blur, vert_shader_blur, frag_shader_blur] = create_program_from_sources(blurVertSource, blurFragSource);

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

	set_uniform(vert_shader_g, uniform_projection, m_perspective);

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
		//if (window.PeekMessages())
		//	continue;
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

		set_uniform(vert_shader_g, uniform_view, m_camera.GetViewMatrix());

		// Draw
		render.BeginFrame();

		{
			/* g-buffer pass */
			static auto const viewport_width = screen_width;
			static auto const viewport_height = screen_height;
			glViewport(0, 0, viewport_width, viewport_height);

			auto const depth_clear_val = 1.0f;
			glClearNamedFramebufferfv(fb_gbuffer, GL_COLOR, 0, glm::value_ptr(glm::vec3(1.0f)));
			glClearNamedFramebufferfv(fb_gbuffer, GL_COLOR, 1, glm::value_ptr(glm::vec3(1.0f)));
			glClearNamedFramebufferfv(fb_gbuffer, GL_COLOR, 2, glm::value_ptr(glm::vec4(1.0f)));
			glClearNamedFramebufferfv(fb_gbuffer, GL_COLOR, 3, glm::value_ptr(glm::vec2(1.0f)));
			glClearNamedFramebufferfv(fb_gbuffer, GL_COLOR, 4, glm::value_ptr(glm::vec2(1.0f)));
			glClearNamedFramebufferfv(fb_gbuffer, GL_DEPTH, 0, &depth_clear_val);

			glBindFramebuffer(GL_FRAMEBUFFER, fb_gbuffer);

			glBindTextureUnit(0, texture_cube_diffuse);
			glBindTextureUnit(1, texture_cube_specular);
			glBindTextureUnit(2, texture_cube_normal);
			glBindTextureUnit(3, texture_cube_emissive);

			glBindProgramPipeline(pr_gbuffer);

			for (auto& object : objects)
			{
				switch (object.shape)
				{
				case shape_t::cube: glBindVertexArray(vao_cube); break;
				case shape_t::quad: glBindVertexArray(vao_quad); break;
				}

				auto const curr_mvp_inv = m_perspective * m_camera.GetViewMatrix() * object.model;

				set_uniform(vert_shader_g, uniform_modl, object.model);
				set_uniform(vert_shader_g, uniform_mvp, curr_mvp_inv);
				set_uniform(vert_shader_g, uniform_mvp_inverse, object.mvp_inv_prev);
				set_uniform(vert_shader_g, uniform_blur_except, object.except);
				set_uniform(frag_shader_g, uniform_emissive_strength, object.emissive_strength);

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
			glClearNamedFramebufferfv(fb_finalcolor, GL_COLOR, 0, glm::value_ptr(glm::vec3(0.2f, 0.6f, 1.0f)));
			glClearNamedFramebufferfv(fb_finalcolor, GL_DEPTH, 0, &depth_clear_val);

			glBindFramebuffer(GL_FRAMEBUFFER, fb_finalcolor);

			glBindTextureUnit(0, texture_gbuffer_position);
			glBindTextureUnit(1, texture_gbuffer_normal);
			glBindTextureUnit(2, texture_gbuffer_albedo);
			glBindTextureUnit(3, texture_gbuffer_depth);
			glBindTextureUnit(4, texture_gbuffer_emissive);
			glBindTextureUnit(5, texture_skybox);

			glBindProgramPipeline(pr_main);
			glBindVertexArray(vao_empty);

			set_uniform(frag_shader, uniform_cam_pos, m_camera.position);
			set_uniform(frag_shader, uniform_light_col, glm::vec3(1.0));
			set_uniform(frag_shader, uniform_light_pos, light_pos);
			set_uniform(vert_shader, uniform_cam_dir, glm::inverse(glm::mat3(m_camera.GetViewMatrix())));
			set_uniform(vert_shader, uniform_fov, fov);
			set_uniform(vert_shader, uniform_aspect, float(viewport_width) / float(viewport_height));
			set_uniform(vert_shader, uniform_uvs_diff, glm::vec2(
				float(viewport_width) / float(screen_width),
				float(viewport_height) / float(screen_height)
			));

			glDrawArrays(GL_TRIANGLES, 0, 6);

			/* motion blur */

			glClearNamedFramebufferfv(fb_blur, GL_COLOR, 0, glm::value_ptr(glm::vec3(0.0f)));

			glBindFramebuffer(GL_FRAMEBUFFER, fb_blur);

			glBindTextureUnit(0, texture_gbuffer_color);
			glBindTextureUnit(1, texture_gbuffer_velocity);

			glBindProgramPipeline(pr_blur);
			glBindVertexArray(vao_empty);

			set_uniform(frag_shader_blur, uniform_blur_bias, 0.03f/*float(fps_sum) / float(60)*/);
			set_uniform(vert_shader_blur, uniform_uvs_diff, glm::vec2(
				float(viewport_width) / float(screen_width),
				float(viewport_height) / float(screen_height)
			));

			glDrawArrays(GL_TRIANGLES, 0, 6);

			/* scale raster */
			glViewport(0, 0, window.GetWidth(), window.GetHeight());

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBlitNamedFramebuffer(fb_blur, 0, 0, 0, viewport_width, viewport_height, 0, 0, window.GetWidth(), window.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
		render.EndFrame();
	}

	delete_items(glDeleteBuffers,
		{
		vbo_cube,
		ibo_cube,

		vbo_quad,
		ibo_quad,
		});
	delete_items(glDeleteTextures,
		{
		texture_cube_diffuse,
		texture_cube_specular,
		texture_cube_normal,

		texture_gbuffer_position,
		texture_gbuffer_albedo,
		texture_gbuffer_normal,
		texture_gbuffer_depth,
		texture_gbuffer_color,

		texture_skybox,

		texture_motion_blur,
		texture_motion_blur_mask
		});
	delete_items(glDeleteProgram, {
		vert_shader,
		frag_shader,

		vert_shader_g,
		frag_shader_g,
		});

	delete_items(glDeleteProgramPipelines, { pr_main, pr_gbuffer });
	delete_items(glDeleteVertexArrays, { vao_cube, vao_empty });
	delete_items(glDeleteFramebuffers, { fb_gbuffer, fb_finalcolor, fb_blur });

	render.Destroy();
	window.Destroy();
	log.Destroy();
}