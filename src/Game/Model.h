#pragma once

#include "Shader.h"
#include "Material.h"
#include "t1Camera.h"
#include "Matrices.h"
#include "Light.h"
#include "Mesh.h"
#include "PhysicsManager.h"
#include "t1Renderer.h"
#include "Node.h"
#include "t1Animation.h"


class t1Model : public t1Node
{
public:
	t1Model(bool defaultCubeMesh = false);
	t1Model(t1Model* model);
	t1Model(t1Shader* shader);
	t1Model(std::vector<std::shared_ptr<t1Mesh>> meshes, t1Shader* shader = nullptr);
	t1Model(std::string filename, std::vector<t1Material*> mat, unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenBoundingBoxes,
		t1PhysicsManager* man = nullptr, t1Shader* shader = nullptr, t1Matrices* m = nullptr);
	~t1Model();

	void Load(std::string filename, unsigned int flags = 0);
	void Load();
	void Unload(bool unloadTextures);

	void Draw() override;
	void DrawSkybox();

	void SetTransform(const rp3d::Transform& transform) override;
	void SetPosition(const rp3d::Vector3& position);
	void SetOrientation(const rp3d::Quaternion& orientation);
	void SetSize(const rp3d::Vector3& size) override;
	void SetMaterial(std::vector<t1Material*> mat, bool loadTextures = true);
	void SetMaterialSlot(t1Material* mat, unsigned int slot = 0);
	void SetShader(t1Shader* shader, bool temp = false);
	void SetPhysicsManager(t1PhysicsManager* man);
	void SetFilename(const std::string& filename);
	void SetIsDrawable(bool drawable);
	void SetIsLoadingImmediatelly(bool imm);
	void SetIsMaterialsEnabled(bool materialsEnabled);
	void SetShadowBias(float bias);
	void AddChild(t1Node* child) override;
	void DefaultPose();

	void CreateRigidBody();

	void Move(const rp3d::Vector3& position);
	void Rotate(const rp3d::Quaternion& orientation);
	void Expand(const rp3d::Vector3& size);

	void CreateBoxShape(unsigned int mesh = 0, rp3d::Transform tr = rp3d::Transform::identity());
	void CreateSphereShape(unsigned int mesh = 0, rp3d::Transform tr = rp3d::Transform::identity());
	void CreateCapsuleShape(unsigned int mesh = 0, rp3d::Transform tr = rp3d::Transform::identity());
	void CreateConcaveShape(unsigned int mesh = 0, rp3d::Transform tr = rp3d::Transform::identity());
	void CreateConvexShape(unsigned int mesh = 0, rp3d::Transform tr = rp3d::Transform::identity());

	int GetMeshesCount();

	bool IsLoaded();
	bool IsDrawable();
	bool IsLoadingImmediatelly();

	rp3d::Transform UpdateTransform();

	rp3d::Transform GetTransform() override;
	rp3d::Vector3 GetPosition();
	rp3d::Quaternion GetOrientation();
	rp3d::Vector3 GetSize() override;

	t1Shader* GetShader();
	rp3d::RigidBody* GetRigidBody() override;

	std::vector<std::shared_ptr<t1Animation>> GetAnimations();
	std::vector<std::shared_ptr<t1Bone>> GetBones();
	std::vector<glm::mat4>& GetPose();

	std::string GetFilename();

	std::vector<t1Material*>& GetMaterial();

	Json::Value Serialize();
	void Deserialize(Json::Value data, bool storeData = false);

private:
	enum class CollisionShapeType
	{
		Nothing,
		Box,
		Sphere,
		Capsule,
		Concave,
		Convex
	};

	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, aiNode* node, aiNode* mnode);
	void LoadAnimations(const aiScene* scene);
	void FindBoneNodes(aiNode* node, std::vector<std::shared_ptr<t1Bone>>& bones);
	void CalculatePose(t1Bone* bone);

	bool ProcessBone(aiNode* node, std::shared_ptr<t1Bone>& out);

	bool drawable = true, immLoad = true, loaded = false, materialsEnabled = true;

	float shadowBias = 0.0000001;

	std::vector<std::shared_ptr<t1Mesh>> meshes;
	std::vector<std::shared_ptr<t1Bone>> bones;
	std::vector<std::shared_ptr<t1Bone>> bonesChildren;
	std::vector<glm::mat4> pose;
	std::unordered_map<std::string, std::pair<int, glm::mat4>> bonemap;

	t1Matrices* m = t1Renderer::GetInstance().GetMatrices();
	t1Shader* shader = t1Renderer::GetInstance().GetShader(t1Renderer::ShaderType::Deferred);
	t1Shader* tempShader = nullptr;

	Json::Value data;

	std::vector<t1Material*> mat;
	std::vector<std::shared_ptr<t1Animation>> anims;

	t1PhysicsManager* man;

	glm::mat4 globalInverseTransform;

	std::vector<std::variant<rp3d::BoxShape*,
		rp3d::SphereShape*,
		rp3d::CapsuleShape*,
		rp3d::ConcaveMeshShape*,
		rp3d::ConvexMeshShape*>> shapes;
	std::vector<rp3d::Collider*> colliders;
	std::unordered_map<t1Node*, std::vector<rp3d::Collider*>> childrenColliders;
	rp3d::RigidBody* body = nullptr;
	CollisionShapeType cstype = CollisionShapeType::Nothing;

	rp3d::TriangleVertexArray* triangles = nullptr;
	rp3d::TriangleMesh* tmesh = nullptr;

	rp3d::PolygonVertexArray::PolygonFace* faces = nullptr;
	rp3d::PolygonVertexArray* polygons = nullptr;
	rp3d::PolyhedronMesh* pmesh = nullptr;

	std::string filename;

	rp3d::Vector3 size{ 1, 1, 1 };
	rp3d::Transform transform;
};
