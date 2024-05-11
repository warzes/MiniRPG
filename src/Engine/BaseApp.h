#pragma once

class BaseApp
{
public:
	virtual ~BaseApp() = default;

	// Filesystem

	// Log system

	// Time system

	// Event system

	// Input system

	// Window system

	// Main Loop


private:
	BaseApp() = delete;
	BaseApp(const BaseApp&) = delete;
	BaseApp(BaseApp&&) = delete;
	BaseApp& operator=(const BaseApp&) = delete;
	BaseApp& operator=(BaseApp&&) = delete;
};