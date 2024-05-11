#pragma once

#include "Systems.h"

class LogSystem final
{
public:
	LogSystem(Systems& systems);
	// Log system
	void Print(const std::string& message);
	void Warning(const std::string& message);
	void Error(const std::string& message);
	void Fatal(const std::string& message);

private:
	Systems& m_systems;
};

// TODO: временно, потом убрть
void LogPrint(const std::string& message);
void LogWarning(const std::string& message);
void LogError(const std::string& message);
void LogFatal(const std::string& message);