#pragma once

class LogSystem;
class WindowSystem;
class RenderSystem;
class MainGUISystem;

struct Systems final
{
	LogSystem* log = nullptr;
	WindowSystem* window = nullptr;
	RenderSystem* render = nullptr;
	MainGUISystem* mainGUI = nullptr;
};