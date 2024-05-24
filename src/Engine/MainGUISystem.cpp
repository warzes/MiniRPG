#include "stdafx.h"
#include "MainGUISystem.h"
#include "WindowSystem.h"
//-----------------------------------------------------------------------------
MainGUISystem::MainGUISystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.mainGUI = this;
}
//-----------------------------------------------------------------------------
MainGUISystem::~MainGUISystem()
{
}
//-----------------------------------------------------------------------------
bool MainGUISystem::Create(const MainGUISystemCreateInfo& createInfo)
{
	m_enable = createInfo.enable;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_io = &ImGui::GetIO();
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	m_io->IniFilename = nullptr;

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_InitForOpenGL(m_systems.window->GetNativeHandle());
	ImGui_ImplOpenGL3_Init("#version 430");

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	m_defaultFont = m_io->Fonts->AddFontFromFileTTF("Data/fonts/roboto-regular.ttf", 18.0f, nullptr, m_io->Fonts->GetGlyphRangesCyrillic());

	ImFontConfig IconsConfig;
	IconsConfig.MergeMode = true;
	m_io->Fonts->AddFontFromFileTTF("Data/fonts/fontawesome-webfont.ttf", 16.0f, &IconsConfig, m_iconRanges.data());

	return true;
}
//-----------------------------------------------------------------------------
void MainGUISystem::Destroy()
{
	m_io->Fonts->Clear();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	m_enable = false;
}
//-----------------------------------------------------------------------------
bool MainGUISystem::Update()
{
	if (!m_enable) return false;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetColorEditOptions(ImGuiColorEditFlags_PickerHueWheel);

	return false;
}
//-----------------------------------------------------------------------------
void MainGUISystem::Draw()
{
	if (!m_enable) return;

	ImGui::Render();
	glViewport(0, 0, m_systems.window->GetWidth(), m_systems.window->GetHeight());
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
//-----------------------------------------------------------------------------