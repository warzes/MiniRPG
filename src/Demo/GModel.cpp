#include "stdafx.h"
#include "GModel.h"
//-----------------------------------------------------------------------------
GModel::GModel(Systems& systems)
	: m_systems(systems)
{
}
//-----------------------------------------------------------------------------
GModel::GModel(Systems& systems, const std::string& modelPath)
	: m_systems(systems)
{
	loadModel(modelPath);
}
//-----------------------------------------------------------------------------
void GModel::Update(const GLProgramPipelineRef& program) const
{
	for (size_t i = 0; i < m_meshes.size(); i++)
		m_meshes[i].Update(program);
}
//-----------------------------------------------------------------------------
std::shared_ptr<AABB> GModel::GetBounding()
{
	if (!m_bounding)
	{
		m_bounding = std::make_shared<AABB>();
		size_t MeshSize = m_meshes.size();
		for (size_t i = 0; i < MeshSize; ++i)
		{
			m_bounding->Combine(*m_meshes[i].GetBounding());
		}
	}
	return m_bounding;
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> GModel::GetTriangles()
{
	std::vector<glm::vec3> Triangle;
	size_t MeshSize = m_meshes.size();
	for (size_t i = 0; i < MeshSize; ++i)
	{
		std::vector<glm::vec3> temp = m_meshes[i].GetTriangle();
		Triangle.insert(Triangle.end(), temp.begin(), temp.end());
	}
	return Triangle;
}
//-----------------------------------------------------------------------------
void GModel::loadModel(const std::string& modelPath)
{
	Assimp::Importer modelImporter;
	m_scene = modelImporter.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (!m_scene || !m_scene->mRootNode || m_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
	{
		m_systems.log->Error("Load Model error: " + std::string(modelImporter.GetErrorString()));
		return;
	}
	m_directory = modelPath.substr(0, modelPath.find_last_of('/'));
	traverseNodes();
}
//-----------------------------------------------------------------------------
void GModel::traverseNodes()
{
	GLint NumMeshes = static_cast<GLint>(m_scene->mNumMeshes);
	for (GLint i = 0; i < NumMeshes; ++i)
		m_meshes.push_back(processMesh(m_scene->mMeshes[i]));
}
//-----------------------------------------------------------------------------
GMesh GModel::processMesh(const aiMesh* AiMesh)
{
	_ASSERT(AiMesh);
	std::vector<GMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<GMeshTexture> Textures;
	GMeshMatProperties MeshMatProperties;
	processVertex(AiMesh, Vertices);
	processIndices(AiMesh, Indices);
	processTextures(AiMesh, Textures);
	processMatProperties(AiMesh, MeshMatProperties);
	return GMesh(m_systems, Vertices, Indices, Textures, MeshMatProperties, ++m_meshCount);
}
//-----------------------------------------------------------------------------
void GModel::processVertex(const aiMesh* AiMesh, std::vector<GMeshVertex>& vertices)
{
	_ASSERT(AiMesh);
	GLint NumVertices = (GLint)AiMesh->mNumVertices;
	for (GLint i = 0; i < NumVertices; ++i)
	{
		GMeshVertex Vertex;
		Vertex.position = glm::vec3(AiMesh->mVertices[i].x, AiMesh->mVertices[i].y, AiMesh->mVertices[i].z);
		if (AiMesh->mNormals != nullptr)
			Vertex.normal = glm::vec3(AiMesh->mNormals[i].x, AiMesh->mNormals[i].y, AiMesh->mNormals[i].z);
		else
			Vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
		
		if (AiMesh->mTextureCoords[0]) 
			Vertex.texCoords = glm::vec2(AiMesh->mTextureCoords[0][i].x, AiMesh->mTextureCoords[0][i].y);
		else 
			Vertex.texCoords = glm::vec2(0.0f, 0.0f);

		if (AiMesh->mTangents)
			Vertex.tangent = glm::vec3(AiMesh->mTangents[i].x, AiMesh->mTangents[i].y, AiMesh->mTangents[i].z);

		vertices.push_back(Vertex);
	}
}
//-----------------------------------------------------------------------------
void GModel::processIndices(const aiMesh* AiMesh, std::vector<uint32_t>& indices)
{
	_ASSERT(AiMesh);
	GLint NumFaces = AiMesh->mNumFaces;
	for (GLint i = 0; i < NumFaces; ++i)
	{
		aiFace AiFace = AiMesh->mFaces[i];
		GLint NumIndices = AiFace.mNumIndices;
		for (GLint k = 0; k < NumIndices; ++k)
			indices.push_back(AiFace.mIndices[k]);
	}
}
//-----------------------------------------------------------------------------
void GModel::processTextures(const aiMesh* AiMesh, std::vector<GMeshTexture>& textures)
{
	_ASSERT(AiMesh);
	if (AiMesh->mMaterialIndex < 0)
		return;
	aiMaterial* pAiMat = m_scene->mMaterials[AiMesh->mMaterialIndex];
	GTexture Texture2DInSRGBSpace;
	Texture2DInSRGBSpace.isSRGBSpace = GL_TRUE;
	GTexture Texture2D4Roughness;
	Texture2D4Roughness.Type4MinFilter = GL_LINEAR_MIPMAP_LINEAR;
	loadTextureFromMaterial(aiTextureType_DIFFUSE, pAiMat, DIFFUSE_TEX, textures/*, Texture2DInSRGBSpace*/);
	loadTextureFromMaterial(aiTextureType_SPECULAR, pAiMat, SPECULAR_TEX, textures);
	loadTextureFromMaterial(aiTextureType_HEIGHT, pAiMat, NORMAL_TEX, textures);
	loadTextureFromMaterial(aiTextureType_SHININESS, pAiMat, ROUGHNESS_TEX, textures/*, Texture2D4Roughness*/);
	loadTextureFromMaterial(aiTextureType_AMBIENT, pAiMat, METALLIC_TEX, textures);
}
//-----------------------------------------------------------------------------
void GModel::processMatProperties(const aiMesh* AiMesh, GMeshMatProperties& meshMatProperties)
{
	_ASSERT(AiMesh);
	if (AiMesh->mMaterialIndex < 0)
		return;
	aiMaterial* pAiMat = m_scene->mMaterials[AiMesh->mMaterialIndex];
	aiColor3D AmbientColor, DiffuseColor, SpecularColor;
	float Shininess = 0.0f, Refracti = 0.0f;
	pAiMat->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor);
	pAiMat->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor);
	pAiMat->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor);
	pAiMat->Get(AI_MATKEY_SHININESS, Shininess);
	pAiMat->Get(AI_MATKEY_REFRACTI, Refracti);
	meshMatProperties.ambientColor = { AmbientColor.r, AmbientColor.g, AmbientColor.b };
	meshMatProperties.diffuseColor = { DiffuseColor.r, DiffuseColor.g, DiffuseColor.b };
	meshMatProperties.specularColor = { SpecularColor.r, SpecularColor.g, SpecularColor.b };
	meshMatProperties.shininess = Shininess;
	meshMatProperties.refracti = Refracti;
}
//-----------------------------------------------------------------------------
void GModel::loadTextureFromMaterial(aiTextureType textureType, const aiMaterial* mat, const std::string& textureNamePrefix, std::vector<GMeshTexture>& textures, std::shared_ptr<GTexture> vTexture2D)
{
	_ASSERT(mat);
	GLint TextureCount = mat->GetTextureCount(textureType);
	int TextureIndex = -1;
	if (TextureCount <= 0)
	{
		GMeshTexture MeshTexture;
		textures.push_back(MeshTexture);
	}
	for (GLint i = 0; i < TextureCount; ++i)
	{
		aiString Str;
		mat->GetTexture(textureType, i, &Str);
		std::string TextureName = Str.C_Str();
		std::string TexturePath = m_directory + "/" + TextureName;
		GLboolean Skip = GL_FALSE;
		GLint LoadedTextureCount = static_cast<int>(m_loadedTextures.size());
		for (int k = 0; k < LoadedTextureCount; ++k)
		{
			if (TexturePath == m_loadedTextures[k].texturePath)
			{
				Skip = GL_TRUE;
				m_loadedTextures[k].textureUniformName = textureNamePrefix + std::to_string(++TextureIndex);
				textures.push_back(m_loadedTextures[k]);
				break;
			}
		}
		if (!Skip)
		{
#if 0 // OLD
			GMeshTexture MeshTexture;
			loadTextureFromFile(TexturePath, vTexture2D);
			MeshTexture.ID = vTexture2D->TextureID;
			MeshTexture.texturePath = TexturePath;
			MeshTexture.textureUniformName = textureNamePrefix + std::to_string(++TextureIndex);
			textures.push_back(MeshTexture);
			m_loadedTextures.push_back(MeshTexture);
#else
			GMeshTexture MeshTexture;
			MeshTexture.texture = m_systems.render->CreateTexture2DFromFile(TexturePath, STBI_rgb_alpha, true);
			MeshTexture.texturePath = TexturePath;
			MeshTexture.textureUniformName = textureNamePrefix + std::to_string(++TextureIndex);
			textures.push_back(MeshTexture);
			m_loadedTextures.push_back(MeshTexture);
#endif

		}
	}
}
//-----------------------------------------------------------------------------