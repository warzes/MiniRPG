#pragma once

class LogSystem;
class WindowSystem;
class RenderSystem;

struct Systems final
{
	LogSystem* log = nullptr;
	WindowSystem* window = nullptr;
	RenderSystem* render = nullptr;
};