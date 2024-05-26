#pragma once

#include "GMesh.h"

// TEMP: delete this
struct GTexture
{
	enum class TextureType
	{
		Texture2D = GL_TEXTURE_2D,
		Texture3D = GL_TEXTURE_3D,
		Texture2DArray = GL_TEXTURE_2D_ARRAY,
		TextureCubeMap = GL_TEXTURE_CUBE_MAP,
		TextureCubeArray = GL_TEXTURE_CUBE_MAP_ARRAY,
		DepthCubeMap = GL_DEPTH_COMPONENT
	};

	enum class TextureAttachmentType
	{
		ColorTexture,
		DepthTexture,
		CubeDepthTexture,
		StencilTexture,
		DepthAndStencilTexture,
		DepthArrayTexture
	};

	GLint					InternalFormat;
	GLenum					ExternalFormat;
	GLenum					DataType;
	GLsizei					Width;
	GLsizei					Height;
	GLsizei					Depth;
	std::vector<GLvoid*>	pDataSet;
	GLint					Type4WrapS;
	GLint					Type4WrapT;
	GLint					Type4WrapR;
	GLint					Type4MinFilter;
	GLint					Type4MagFilter;
	GLboolean				isMipmap;
	GLboolean				isSRGBSpace;
	GLint					TextureID;
	GLint					ImageBindUnit;
	GLint					FrameBufferID;
	GLint					AttachmentID;
	std::string				TextureName;
	TextureType				TextureType;
	TextureAttachmentType	TextureAttachmentType;
	std::vector<float>		BorderColor;

	GTexture() : TextureID(-1), InternalFormat(GL_RGBA), ExternalFormat(GL_RGBA), DataType(GL_UNSIGNED_BYTE), Width(512),
		Height(512), Depth(1), Type4WrapS(GL_REPEAT), Type4WrapT(GL_REPEAT), Type4WrapR(GL_REPEAT), Type4MinFilter(GL_LINEAR),
		Type4MagFilter(GL_LINEAR), isMipmap(GL_FALSE), isSRGBSpace(GL_FALSE), TextureType(TextureType::Texture2D), TextureAttachmentType(TextureAttachmentType::ColorTexture),
		BorderColor{ 1,1,1,1 }, ImageBindUnit(-1), FrameBufferID(-1), AttachmentID(-1)
	{
	}

	bool operator<(const GTexture& vAnotherTexture) const
	{
		return TextureID < vAnotherTexture.TextureID;
	}
};

class GModel final
{
public:
	GModel(Systems& systems);
	GModel(Systems& systems, const std::string& modelPath);

	void Update(const GLProgramPipelineRef& program) const;

	std::shared_ptr<AABB> GetBounding();
	std::vector<glm::vec3> GetTriangles();

private:
	void loadModel(const std::string& modelPath);
	void traverseNodes();
	GMesh processMesh(const aiMesh* AiMesh);
	void processVertex(const aiMesh* AiMesh, std::vector<GMeshVertex>& vertices);
	void processIndices(const aiMesh* AiMesh, std::vector<uint32_t>& indices);
	void processTextures(const aiMesh* AiMesh, std::vector<GMeshTexture>& textures);
	void processMatProperties(const aiMesh* AiMesh, GMeshMatProperties& meshMatProperties);
	void loadTextureFromMaterial(aiTextureType textureType, const aiMaterial* mat, const std::string& textureNamePrefix, std::vector<GMeshTexture>& textures, std::shared_ptr<GTexture> vTexture2D = std::make_shared<GTexture>());

	Systems& m_systems;

	int m_meshCount = -1;
	std::vector<GMeshTexture> m_loadedTextures;
	std::vector<GMesh> m_meshes;
	std::string m_directory;
	const aiScene* m_scene = nullptr;
	std::shared_ptr<AABB> m_bounding;
};