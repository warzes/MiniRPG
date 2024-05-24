#pragma once

#include "Systems.h"

// TODO: возможно рендерить в текстуру, а ее уже использовать

struct MainGUISystemCreateInfo final
{
	bool enable = true;
};

class MainGUISystem
{
public:
	MainGUISystem(Systems& systems);
	~MainGUISystem();
	bool Create(const MainGUISystemCreateInfo& createInfo);
	void Destroy();

	ImGuiIO* GetIO() const { return m_io; }

	bool Update();
	void Draw();

	void SetEnable(bool enable) { m_enable = enable; }
	bool IsEnable() const { return m_enable; }

private:
	Systems& m_systems;

	ImGuiIO* m_io = nullptr;
	ImFont* m_defaultFont = nullptr;
	std::vector<uint16_t> m_iconRanges = { 0xf000, 0xf3ff, 0 };

	bool m_enable = false;
};