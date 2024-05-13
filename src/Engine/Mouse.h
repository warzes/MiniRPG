#pragma once

class WindowSystem;

namespace Mouse
{
	enum class Button
	{
		Left,   // The left mouse button
		Right,  // The right mouse button
		Middle, // The middle (wheel) mouse button
		Extra1, // The first extra mouse button
		Extra2  // The second extra mouse button
	};

	static constexpr unsigned int ButtonCount{ 5 }; // The total number of mouse buttons

	enum class Wheel
	{
		Vertical,  // The vertical mouse wheel
		Horizontal // The horizontal mouse wheel
	};

	// Check if a mouse button is pressed
	bool IsButtonPressed(Button button);

	// Get the current position of the mouse in desktop coordinates
	glm::ivec2 GetPosition();
	// Get the current position of the mouse in window coordinates
	glm::ivec2 GetPosition(const WindowSystem& relativeTo);

	// Set the current position of the mouse in desktop coordinates
	void SetPosition(const glm::ivec2& position);
	// Set the current position of the mouse in window coordinates
	void SetPosition(const glm::ivec2& position, const WindowSystem& relativeTo);
	void SetPositionCentre(const WindowSystem& relativeTo);

}