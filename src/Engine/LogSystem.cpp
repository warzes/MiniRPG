#include "stdafx.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
void Exit(); // ==> Core.h
//-----------------------------------------------------------------------------
LogSystem::LogSystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.log = this;
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