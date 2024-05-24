#pragma once

class LogSystem;
class WindowSystem;
class RenderSystem;
class GraphicsSystem;
class MainGUISystem;

struct Systems final
{
	LogSystem* log = nullptr;
	WindowSystem* window = nullptr;
	RenderSystem* render = nullptr;
	GraphicsSystem* graphics = nullptr;
	MainGUISystem* mainGUI = nullptr;
};