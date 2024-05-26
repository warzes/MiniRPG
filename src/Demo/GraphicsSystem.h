#pragma once

#include "GraphicsResources.h"
#include "MVector.h"
#include "RenderPass.h"

struct GraphicsSystemCreateInfo final
{
};

class GraphicsSystem
{
public:
	GraphicsSystem(Systems& systems);
	~GraphicsSystem();
	bool Create(const GraphicsSystemCreateInfo& createInfo);
	void Destroy();

	void Update();
	void Frame(bool clearMainScreen);

	void RegisterObject(const std::shared_ptr<IGraphicsObject>& object);
	void RegisterRenderPass(const std::shared_ptr<IRenderPass>& renderPass);

	const std::shared_ptr<GModel>& GetModel(const std::string& modelPath);


private:
	Systems& m_systems;
	MVector<std::shared_ptr<IGraphicsObject>> m_objects;
	MVector<std::shared_ptr<IRenderPass>> m_renderPassSet;
	std::unordered_map<std::string, std::shared_ptr<GModel>> m_modelMap;
	std::unordered_map<std::string, std::any> m_sharedDataMap;
	RenderCamera m_camera; // TODO: сделать
};