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

			// TODO: нет поддержи формата текстур GL_FLOAT - возможно нужно тут
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

	struct RSMBuffer
	{
		void Create(RenderSystem& render, int inWidth, int inHeight)
		{
			Destroy();

			width = inWidth;
			height = inHeight;

			// TODO: нет поддержи формата текстур GL_FLOAT - возможно нужно тут
			textureFlux     = render.CreateTexture2D(GL_RGB16F, GL_RGB, width, height, nullptr, GL_NEAREST);
			textureNormal   = render.CreateTexture2D(GL_RGB16F, GL_RGB, width, height, nullptr, GL_NEAREST);
			texturePosition = render.CreateTexture2D(GL_RGB16F, GL_RGB, width, height, nullptr, GL_NEAREST);
			textureDepth = render.CreateTexture2D(GL_DEPTH_COMPONENT32, GL_DEPTH, width, height, nullptr, GL_NEAREST);

			fbo = render.CreateFramebuffer({ textureFlux, textureNormal, texturePosition }, textureDepth);
		}

		void Destroy()
		{
			textureFlux.reset();
			textureNormal.reset();
			texturePosition.reset();
			textureDepth.reset();

			fbo.reset();
		}

		void Bind(RenderSystem& render)
		{
			const auto depthClearVal = 1.0f;
			render.FramebufferClear(fbo, GL_COLOR, 0, glm::value_ptr(glm::vec3(0.0f)));
			render.FramebufferClear(fbo, GL_COLOR, 1, glm::value_ptr(glm::vec3(0.0f)));
			render.FramebufferClear(fbo, GL_COLOR, 2, glm::value_ptr(glm::vec3(0.0f)));
			render.FramebufferClear(fbo, GL_DEPTH, 0, &depthClearVal);

			render.Bind(fbo);
		}

		GLFramebufferRef fbo = nullptr;

		GLTextureRef textureFlux = nullptr;
		GLTextureRef textureNormal = nullptr;
		GLTextureRef texturePosition = nullptr;
		GLTextureRef textureDepth = nullptr;

		int width = 0;
		int height = 0;

		int rsmResolution = 256; // 1024
		glm::vec3 directionalLightDirection = glm::vec3(-1.0f); // TODO: временно
	};

	struct ShadingWithRSMPass
	{
		void Create(RenderSystem& render, int inWidth, int inHeight, GLProgramPipelineRef shader)
		{
			Destroy();

			initVPLsSampleCoordsAndWeights();

			// TODO: нет поддержи формата текстур GL_FLOAT - возможно нужно тут
			texture = render.CreateTexture2D(GL_RGBA32F, GL_RGBA, inWidth, inHeight, nullptr, GL_NEAREST);

			std::vector<int> LocalGroupSize;
			LocalGroupSize.resize(3);
			glGetProgramiv(*shader->GetComputeShader(), GL_COMPUTE_WORK_GROUP_SIZE, LocalGroupSize.data());
			globalGroupSize.push_back((inWidth + LocalGroupSize[0] - 1) / LocalGroupSize[0]);
			globalGroupSize.push_back((inHeight + LocalGroupSize[1] - 1) / LocalGroupSize[1]);
			globalGroupSize.push_back(1);
		}

		void Destroy()
		{
			texture.reset();
			globalGroupSize.clear();
			VPLsSampleCoordsAndWeights.clear();
		}

		void Bind(RenderSystem& render)
		{
		}

		void initVPLsSampleCoordsAndWeights()
		{
			std::default_random_engine e;
			std::uniform_real_distribution<float> u(0, 1);
			for (size_t i = 0; i < VPLNum; i++)
			{
				float xi1 = u(e);
				float xi2 = u(e);
				VPLsSampleCoordsAndWeights.push_back({ xi1 * sin(2 * glm::pi<float>() * xi2), xi1 * cos(2 * glm::pi<float>() * xi2), xi1 * xi1, 0 });
			}
			uniformBuffer = genUniformBuffer(GL_UNIFORM_BUFFER, VPLsSampleCoordsAndWeights.size() * 4 * sizeof(GL_FLOAT), VPLsSampleCoordsAndWeights.data(), GL_STATIC_DRAW, 0);
		}

		GLuint genUniformBuffer(GLenum target, GLsizeiptr size, const void* data, GLenum usage, GLint bindingIndex) // TODO: переделать
		{
			GLuint buffer;
			glGenBuffers(1, &buffer);
			glBindBuffer(target, buffer);
			glBufferData(target, size, data, usage);
			glBindBuffer(target, 0);
			if (bindingIndex != -1)
				glBindBufferBase(target, bindingIndex, buffer);
			return buffer;
		}


		std::vector<int> globalGroupSize;
		std::vector<glm::vec4> VPLsSampleCoordsAndWeights;
		int VPLNum = 32;
		float MaxSampleRadius = 25;
		//glm::mat4 LightVPMatrix;
		glm::vec4 LightDir;

		GLTextureRef texture = nullptr;
		GLuint uniformBuffer;
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

#pragma region RSMShader
	const char* rsmVertSource = R"(
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

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec2 TexCoords;

layout (location = 0) uniform mat4 projectionMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 modelMatrix;
layout (location = 3) uniform mat4 lightVPMatrix;

void main()
{
	const vec4 FragPosInViewSpace = modelMatrix * vec4(Position, 1.0);
	gl_Position = lightVPMatrix * FragPosInViewSpace;
	o.Color = Color;
	o.TexCoords = TexCoords;
	o.Normal = normalize(mat3(transpose(inverse(viewMatrix * modelMatrix))) * Normal);
	o.FragPosInViewSpace = vec3(viewMatrix * FragPosInViewSpace);
}
		)";

	const char* rsmFragSource = R"(
#version 460
#pragma debug(on)

in inBlock
{
	vec3 FragPosInViewSpace;
	vec3 Color;
	vec3 Normal;
	vec2 TexCoords;
} i;

layout (location = 0) out vec3 outFlux;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outPosition;

layout(location = 0) uniform vec3 lightColor = vec3(1);

layout(binding = 0) uniform sampler2D DiffuseTexture;

void main()
{
	vec3 diffuseTex = texture(DiffuseTexture, i.TexCoords).rgb * i.Color;
	//diffuseTex = pow(diffuseTex, vec3(2.2));
	vec3 VPLFlux = lightColor * diffuseTex;
	outFlux = VPLFlux;
	outNormal = i.Normal;
	outPosition = i.FragPosInViewSpace;
}
		)";
#pragma endregion

#pragma region ComputeRSMShader
	const char* RSMComputeSource = R"(
#version 460
#pragma debug(on)
#pragma optionNV (unroll all)

#define LOCAL_GROUP_SIZE 16
#define VPL_NUM 32

layout (local_size_x = LOCAL_GROUP_SIZE, local_size_y = LOCAL_GROUP_SIZE) in;

layout (rgba32f, binding = 0) uniform writeonly image2D u_OutputImage;

layout(std140, binding = 0) uniform VPLsSampleCoordsAndWeights
{
	vec4 u_VPLsSampleCoordsAndWeights[VPL_NUM];
};

uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_PositionTexture;
uniform sampler2D u_RSMFluxTexture;
uniform sampler2D u_RSMNormalTexture;
uniform sampler2D u_RSMPositionTexture;

layout (location = 1) uniform mat4  u_LightVPMatrixMulInverseCameraViewMatrix; // TODO: 1 retype
layout (location = 2) uniform float u_MaxSampleRadius;
layout (location = 3) uniform int   u_RSMSize;
layout (location = 4) uniform int   u_VPLNum;
layout (location = 5) uniform vec3  u_LightDirInViewSpace;

vec3 calcVPLIrradiance(vec3 vVPLFlux, vec3 vVPLNormal, vec3 vVPLPos, vec3 vFragPos, vec3 vFragNormal, float vWeight)
{
	vec3 VPL2Frag = normalize(vFragPos - vVPLPos);
	return vVPLFlux * max(dot(vVPLNormal, VPL2Frag), 0) * max(dot(vFragNormal, -VPL2Frag), 0) * vWeight;
}

void main()
{
	if(u_VPLNum != VPL_NUM)
		return;

	ivec2 FragPos = ivec2(gl_GlobalInvocationID.xy);
	vec3 FragViewNormal = normalize(texelFetch(u_NormalTexture, FragPos, 0).xyz);
	vec3 FragAlbedo = texelFetch(u_AlbedoTexture, FragPos, 0).xyz;
	vec3 FragViewPos = texelFetch(u_PositionTexture, FragPos, 0).xyz;

	vec4 FragPosInLightSpace = u_LightVPMatrixMulInverseCameraViewMatrix * vec4(FragViewPos, 1);
	FragPosInLightSpace /= FragPosInLightSpace.w;
	vec2 FragNDCPos4Light = (FragPosInLightSpace.xy + 1) / 2;
	float RSMTexelSize = 1.0 / u_RSMSize;
	vec3 DirectIllumination;
	if(FragPosInLightSpace.z < 0.0f || FragPosInLightSpace.x > 1.0f || FragPosInLightSpace.y > 1.0f || FragPosInLightSpace.x < 0.0f || FragPosInLightSpace.y < 0.0f )
		DirectIllumination = vec3(0.1) * FragAlbedo;
	else
		DirectIllumination = FragAlbedo * max(dot(-u_LightDirInViewSpace, FragViewNormal), 0.1);
	vec3 IndirectIllumination = vec3(0);
	for(int i = 0; i < u_VPLNum; ++i)
	{
		vec3 VPLSampleCoordAndWeight = u_VPLsSampleCoordsAndWeights[i].xyz;
		vec2 VPLSamplePos = FragNDCPos4Light + u_MaxSampleRadius * VPLSampleCoordAndWeight.xy * RSMTexelSize;
		vec3 VPLFlux = texture(u_RSMFluxTexture, VPLSamplePos).xyz;
		vec3 VPLNormalInViewSpace = normalize(texture(u_RSMNormalTexture, VPLSamplePos).xyz);
		vec3 VPLPositionInViewSpace = texture(u_RSMPositionTexture, VPLSamplePos).xyz;

		IndirectIllumination += calcVPLIrradiance(VPLFlux, VPLNormalInViewSpace, VPLPositionInViewSpace, FragViewPos, FragViewNormal, VPLSampleCoordAndWeight.z);
	}
	IndirectIllumination *= FragAlbedo;

	vec3 Result = DirectIllumination  + IndirectIllumination / u_VPLNum;

	imageStore(u_OutputImage, FragPos, vec4(Result, 1.0));
}
		)";
#pragma endregion


#pragma region MainShader
	const char* MainVertSource = R"(
#version 460
#pragma debug(on)

out gl_PerVertex { vec4 gl_Position; };

out outBlock
{
	vec2 texcoord;
} o;

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

	o.texcoord = texcoord;
	gl_Position = vec4(position, 0.0, 1.0);
}
)";

	const char* MainFragSource = R"(
#version 460
#pragma debug(on)

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D computeTexture;

in in_block
{
	vec2 texcoord;
} i;

void main()
{
	vec3 TexelColor = texture(computeTexture, i.texcoord).rgb;
	TexelColor = pow(TexelColor, vec3(1.0f/2.2f));
	outColor = vec4(TexelColor, 1.0f);
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
	GLProgramPipelineRef ppRSMBuffer = render.CreateProgramPipelineFromSources(rsmVertSource, rsmFragSource);
	//GLProgramPipelineRef oldppMain = render.CreateProgramPipelineFromSources(oldMainVertSource, oldMainFragSource);
	GLProgramPipelineRef ppMain = render.CreateProgramPipelineFromSources(MainVertSource, MainFragSource);
	GLProgramPipelineRef rsmComputeShader = render.CreateProgramPipelineFromSources(RSMComputeSource);

	constexpr auto uniform_projection = 0;
	constexpr auto uniform_view = 1;
	constexpr auto uniform_modl = 2;

	// rsm vertex
	constexpr auto uniform_RSM_lightVPMatrix = 3;

	// rsm fragment
	constexpr auto uniform_RSM_light_color = 0;

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
	RSMBuffer rsmBuffer;
	rsmBuffer.Create(render, window.GetWidth(), window.GetHeight());
	ShadingWithRSMPass rsmCompute;
	rsmCompute.Create(render, window.GetWidth(), window.GetHeight(), rsmComputeShader);
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

	// RSM
	glm::vec3 LightPos = glm::vec3(-0.15f, -1.13f, -0.58);
	glm::vec3 LightDir = glm::normalize(glm::vec3(-1.0f, -0.7071f, 0.0f));
	glm::mat4 LightViewMatrix = glm::lookAt(LightPos, LightPos + LightDir, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 LightProjectionMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 10.0f);
	glm::mat4 LightVPMatrix = LightProjectionMatrix * LightViewMatrix;

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
				rsmBuffer.Create(render, width, height);
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

			// RSM
			{
				rsmBuffer.Bind(render);
				render.Bind(ppRSMBuffer);
				render.SetUniform(ppRSMBuffer->GetVertexShader(), uniform_projection, m_perspective);
				render.SetUniform(ppRSMBuffer->GetVertexShader(), uniform_view, m_camera.GetViewMatrix());
				render.SetUniform(ppRSMBuffer->GetVertexShader(), uniform_modl, glm::mat4(1.0f));
				render.SetUniform(ppRSMBuffer->GetVertexShader(), uniform_RSM_lightVPMatrix, LightVPMatrix);
				model->Update(ppRSMBuffer);
			}

			// Compute RSM
			{
				render.Bind(rsmComputeShader);
				glBindImageTexture(0, *rsmCompute.texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F); // куда рисовать
				render.BindSlot(gbuffer.textureGBufferAlbedo, 0);
				render.BindSlot(gbuffer.textureGBufferNormal, 1);
				render.BindSlot(gbuffer.textureGBufferPosition, 2);
				render.BindSlot(rsmBuffer.textureFlux, 3);
				render.BindSlot(rsmBuffer.textureNormal, 4);
				render.BindSlot(rsmBuffer.texturePosition, 5);

				glBindBuffer(GL_UNIFORM_BUFFER, rsmCompute.uniformBuffer);

				glm::mat4 lightVPMatrixMulInverseCameraViewMatrix = LightVPMatrix * glm::inverse(m_camera.GetViewMatrix());
				render.SetUniform(rsmComputeShader->GetComputeShader(), 1, lightVPMatrixMulInverseCameraViewMatrix);
				render.SetUniform(rsmComputeShader->GetComputeShader(), 2, rsmCompute.MaxSampleRadius);
				render.SetUniform(rsmComputeShader->GetComputeShader(), 3, rsmBuffer.rsmResolution);
				render.SetUniform(rsmComputeShader->GetComputeShader(), 4, rsmCompute.VPLNum);
				glm::vec3 lightDirInViewSpace = glm::normalize(glm::vec3(m_camera.GetViewMatrix() * glm::vec4(LightDir, 0.0f)));
				render.SetUniform(rsmComputeShader->GetComputeShader(), 5, lightDirInViewSpace);

				glDispatchCompute(rsmCompute.globalGroupSize[0], rsmCompute.globalGroupSize[1], rsmCompute.globalGroupSize[2]);
				glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

				glBindBuffer(GL_UNIFORM_BUFFER, 0);
			}

			// Final Frame
			{
				finalFB.Bind(render);
				render.Bind(ppMain);
				render.BindSlot(rsmCompute.texture, 0);
				render.Bind(VAOEmpty);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}


			//// Old Final Frame
			//{
			//	finalFB.Bind(render);
			//	render.BindSlot(gbuffer.textureGBufferPosition, 0);
			//	render.BindSlot(gbuffer.textureGBufferNormal, 1);
			//	render.BindSlot(gbuffer.textureGBufferAlbedo, 2);
			//	render.BindSlot(gbuffer.textureGBufferDepth, 3);
			//	//render.BindSlot(gbuffer.textureGBufferEmissive, 4);
			//	render.BindSlot(textureSkybox, 4);

			//	render.Bind(oldppMain);
			//	render.SetUniform(oldppMain->GetVertexShader(), uniform_cam_dir, glm::inverse(glm::mat3(m_camera.GetViewMatrix())));
			//	render.SetUniform(oldppMain->GetVertexShader(), uniform_fov, fov);
			//	render.SetUniform(oldppMain->GetVertexShader(), uniform_aspect, float(window.GetWidth()) / float(window.GetHeight()));
			//	render.SetUniform(oldppMain->GetVertexShader(), uniform_uvs_diff, glm::vec2(
			//		float(window.GetWidth()) / float(window.GetWidth()),
			//		float(window.GetHeight()) / float(window.GetHeight())
			//	));

			//	render.SetUniform(oldppMain->GetFragmentShader(), uniform_cam_pos, m_camera.position);
			//	render.SetUniform(oldppMain->GetFragmentShader(), uniform_light_col, glm::vec3(1.0));
			//	render.SetUniform(oldppMain->GetFragmentShader(), uniform_light_pos, light_pos);

			//	render.Bind(VAOEmpty);
			//	glDrawArrays(GL_TRIANGLES, 0, 6);
			//}

			render.MainFrameBuffer();
			render.BlitFrameBuffer(finalFB.fbo, nullptr,
				0, 0, window.GetWidth(), window.GetHeight(),
				0, 0, window.GetWidth(), window.GetHeight(),
				GL_COLOR_BUFFER_BIT, GL_NEAREST);

			mainGUI.Draw();
		}
		render.EndFrame();
	}
	ppRSMBuffer.reset();
	rsmBuffer.Destroy();
	ppMain.reset();
	finalFB.Destroy();
	ppGBuffer.reset();
	gbuffer.Destroy();
	mainGUI.Destroy();
	graphics.Destroy();
	render.Destroy();
	window.Destroy();
	log.Destroy();
}


