#pragma once

enum class RenderPassType
{
	RenderPassType_Normal,
	RenderPassType_Once,
	RenderPassType_Parallel,
	RenderPassType_ParallelOnce,
	RenderPassType_Delay,
};

class IRenderPass
{
public:
	IRenderPass(Systems& systems);
	IRenderPass(Systems& systems, const std::string& passName, int excutionOrder);
	IRenderPass(Systems& systems, const std::string& passName, int excutionOrder, RenderPassType type);
	virtual ~IRenderPass() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;

	bool operator<(const IRenderPass& otherPass) const;

	const std::string& GetPassName() const { return m_passName; }
	int GetExecutionOrder() const { return m_executionOrder; }

	void SetPassName(const std::string& passName) { m_passName = passName; }
	void SetExecutionOrder(int executionOrder) { m_executionOrder = executionOrder; }
	RenderPassType GetPassType();
	void SetPassType(RenderPassType type);
	int GetExecutionOrder();
	void FinishExecute();

protected:
	Systems& m_systems;
	GLProgramPipelineRef m_program;

private:
	std::string m_passName;
	RenderPassType m_type = RenderPassType::RenderPassType_Normal;
	int m_executionOrder = -1;
};