#include "stdafx.h"
#include "GraphicsSystem.h"
//-----------------------------------------------------------------------------
GraphicsSystem::GraphicsSystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.graphics = this;
}
//-----------------------------------------------------------------------------
GraphicsSystem::~GraphicsSystem()
{
}
//-----------------------------------------------------------------------------
bool GraphicsSystem::Create(const GraphicsSystemCreateInfo& createInfo)
{
	return true;
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Destroy()
{
	m_objects.clear();
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Update()
{
	for (size_t i = 0; i < m_objects.size(); i++)
	{
		m_objects[i]->Update();
	}
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Frame(bool clearMainScreen)
{
	if (clearMainScreen)
		glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < m_renderPassSet.size(); i++)
	{
		if (m_renderPassSet[i]->GetExecutionOrder() == -1) continue;
		switch (m_renderPassSet[i]->GetPassType())
		{
		case RenderPassType::RenderPassType_Once:
		{
			m_renderPassSet[i]->Update();
			m_renderPassSet[i]->FinishExecute();
			break;
		}
		case RenderPassType::RenderPassType_Parallel:
		{
			int j = i;
			while (j < m_renderPassSet.size())
			{
				if (m_renderPassSet[i]->GetExecutionOrder() != m_renderPassSet[j]->GetExecutionOrder())break;
				m_renderPassSet[j++]->Update();
			}
			i = j - 1;
			break;
		}
		case RenderPassType::RenderPassType_ParallelOnce:
		{
			int j = i;
			while (j < m_renderPassSet.size())
			{
				if (m_renderPassSet[i]->GetExecutionOrder() != m_renderPassSet[j]->GetExecutionOrder())break;

				m_renderPassSet[j++]->Update();
			}
			while (i < j)
			{
				m_renderPassSet[i++]->FinishExecute();
			}
			i--;
			break;

		}
		case RenderPassType::RenderPassType_Delay:
			m_renderPassSet[i]->SetPassType(RenderPassType::RenderPassType_Normal);
			break;
		default:
			m_renderPassSet[i]->Update();
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void GraphicsSystem::RegisterObject(const std::shared_ptr<IGraphicsObject>& object)
{
	m_objects.push_back(object->GetName(), object, 1);
	m_objects[m_objects.size() - 1]->Init();
}
//-----------------------------------------------------------------------------
void GraphicsSystem::RegisterRenderPass(const std::shared_ptr<IRenderPass>& renderPass)
{
	m_renderPassSet.push_back(renderPass->GetPassName(), renderPass, 1);
	m_renderPassSet[m_renderPassSet.size() - 1]->Init();
}
//-----------------------------------------------------------------------------
const std::shared_ptr<GModel>& GraphicsSystem::GetModel(const std::string& modelPath)
{
	std::vector<std::string> SplitStringSet;
	if (m_modelMap.find(modelPath) == m_modelMap.end())
		m_modelMap[modelPath] = std::make_shared<GModel>(m_systems, modelPath);
	return m_modelMap[modelPath];
}
//-----------------------------------------------------------------------------