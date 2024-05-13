#include "stdafx.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
LogSystem* gLogSystem = nullptr;
//-----------------------------------------------------------------------------
void Exit(); // ==> Core.h
//-----------------------------------------------------------------------------
LogSystem::LogSystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.log = this;
	gLogSystem = this;
}
//-----------------------------------------------------------------------------
LogSystem::~LogSystem()
{
	Destroy();
}
//-----------------------------------------------------------------------------
bool LogSystem::Create(const LogCreateInfo& createInfo) noexcept
{
#if PLATFORM_DESKTOP
	errno_t fileErr = fopen_s(&m_logFile, createInfo.fileName.c_str(), "w");
	if (fileErr != 0 || !m_logFile)
	{
		m_logFile = nullptr;
		Error("LogCreate() failed!!!");
	}
#endif
	return true;
}
//-----------------------------------------------------------------------------
void LogSystem::Destroy() noexcept
{
#if PLATFORM_DESKTOP
	if (m_logFile)
	{
		fclose(m_logFile);
		m_logFile = nullptr;
	}
#endif
}
//-----------------------------------------------------------------------------
void LogSystem::Print(const std::string& message) noexcept
{
	puts(message.c_str());
#if PLATFORM_DESKTOP
	if (m_logFile) fputs((message + "\n").c_str(), m_logFile);
#endif
}
//-----------------------------------------------------------------------------
void LogSystem::Warning(const std::string& message) noexcept
{
	Print("WARNING: " + message);
}
//-----------------------------------------------------------------------------
void LogSystem::Error(const std::string& message) noexcept
{
	Print("ERROR: " + message);
}
//-----------------------------------------------------------------------------
void LogSystem::Fatal(const std::string& message) noexcept
{
	Print("FATAL: " + message);
	Exit();
}
//-----------------------------------------------------------------------------
void LogPrint(const std::string& message) noexcept
{
	assert(gLogSystem);
	gLogSystem->Print(message);
}
//-----------------------------------------------------------------------------
void LogWarning(const std::string& message) noexcept
{
	assert(gLogSystem);
	gLogSystem->Warning(message);
}
//-----------------------------------------------------------------------------
void LogError(const std::string& message) noexcept
{
	assert(gLogSystem);
	gLogSystem->Error(message);
}
//-----------------------------------------------------------------------------
void LogFatal(const std::string& message) noexcept
{
	assert(gLogSystem);
	gLogSystem->Fatal(message);
}
//-----------------------------------------------------------------------------