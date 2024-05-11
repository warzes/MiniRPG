#include "stdafx.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
LogSystem* thisLogSystem = nullptr;
//-----------------------------------------------------------------------------
void Exit(); // ==> Core.h
//-----------------------------------------------------------------------------
LogSystem::LogSystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.log = this;
	thisLogSystem = this;
}
//-----------------------------------------------------------------------------
void LogSystem::Print(const std::string& message)
{
	puts(message.c_str());
}
//-----------------------------------------------------------------------------
void LogSystem::Warning(const std::string& message)
{
	Print("WARNING: " + message);
}
//-----------------------------------------------------------------------------
void LogSystem::Error(const std::string& message)
{
	Print("ERROR: " + message);
}
//-----------------------------------------------------------------------------
void LogSystem::Fatal(const std::string& message)
{
	Print("FATAL: " + message);
	Exit();
}
//-----------------------------------------------------------------------------
void LogPrint(const std::string& message)
{
	assert(thisLogSystem);
	thisLogSystem->Print(message);
}
//-----------------------------------------------------------------------------
void LogWarning(const std::string& message)
{
	assert(thisLogSystem);
	thisLogSystem->Warning(message);
}
//-----------------------------------------------------------------------------
void LogError(const std::string& message)
{
	assert(thisLogSystem);
	thisLogSystem->Error(message);
}
//-----------------------------------------------------------------------------
void LogFatal(const std::string& message)
{
	assert(thisLogSystem);
	thisLogSystem->Fatal(message);
}
//-----------------------------------------------------------------------------