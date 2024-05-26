#include "stdafx.h"
#include "RenderPass.h"
//-----------------------------------------------------------------------------
IRenderPass::IRenderPass(Systems& systems)
	: m_systems(systems)
{
}
//-----------------------------------------------------------------------------
IRenderPass::IRenderPass(Systems& systems, const std::string& passName, int excutionOrder)
	: m_systems(systems)
	, m_passName(passName)
	, m_executionOrder(excutionOrder)
{
}
//-----------------------------------------------------------------------------
IRenderPass::IRenderPass(Systems& systems, const std::string& passName, int excutionOrder, RenderPassType type)
	: m_systems(systems)
	, m_passName(passName)
	, m_executionOrder(excutionOrder)
	, m_type(type)
{
}
//-----------------------------------------------------------------------------
bool IRenderPass::operator<(const IRenderPass& otherPass) const
{
	return m_executionOrder <= otherPass.GetExecutionOrder();
}
//-----------------------------------------------------------------------------
RenderPassType IRenderPass::GetPassType()
{
	return m_type;
}
//-----------------------------------------------------------------------------
void IRenderPass::SetPassType(RenderPassType type)
{
	m_type = type;
}
//-----------------------------------------------------------------------------
int IRenderPass::GetExecutionOrder()
{
	return m_executionOrder;
}
//-----------------------------------------------------------------------------
void IRenderPass::FinishExecute()
{
	m_executionOrder = -1;
}
//-----------------------------------------------------------------------------