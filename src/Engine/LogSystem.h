#pragma once

#include "Systems.h"

struct LogCreateInfo final
{
	std::string fileName = "log.txt";
};

class LogSystem final
{
public:
	LogSystem(Systems& systems);
	~LogSystem();

	bool Create(const LogCreateInfo& createInfo) noexcept;
	void Destroy() noexcept;

	// Log system
	void Print(const std::string& message) noexcept;
	void Warning(const std::string& message) noexcept;
	void Error(const std::string& message) noexcept;
	void Fatal(const std::string& message) noexcept;

private:
	LogSystem(LogSystem&&) = delete;
	LogSystem(const LogSystem&) = delete;
	LogSystem& operator=(LogSystem&&) = delete;
	LogSystem& operator=(const LogSystem&) = delete;

	Systems& m_systems;

#if PLATFORM_DESKTOP
	FILE* m_logFile = nullptr;
#endif
};

// TODO: временно, потом убрть
void LogPrint(const std::string& message) noexcept;
void LogWarning(const std::string& message) noexcept;
void LogError(const std::string& message) noexcept;
void LogFatal(const std::string& message) noexcept;