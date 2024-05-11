#pragma once

class LogSystem;
class WindowSystem;

struct Systems final
{
	LogSystem* log = nullptr;
	WindowSystem* window = nullptr;
};