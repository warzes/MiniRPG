#pragma once

#include "Keyboard.h"
#include "Mouse.h"

class Event
{
public:
	struct Empty {};
	struct Closed {};

	struct Resized
	{
		glm::uvec2 size; // New size, in pixels
	};

	struct FocusLost {};
	struct FocusGained {};

	struct TextEntered
	{
		std::uint32_t unicode{}; // UTF-32 Unicode value of the character
	};

	struct KeyChanged
	{
		Keyboard::Key      code{};     // Code of the key
		Keyboard::Scancode scancode{}; // Physical code of the key
		bool               alt{};      // Is the Alt key pressed?
		bool               control{};  // Is the Control key pressed?
		bool               shift{};    // Is the Shift key pressed?
		bool               system{};   // Is the System key pressed?
	};
	struct KeyPressed : KeyChanged {};
	struct KeyReleased : KeyChanged {};

	struct MouseWheelScrolled
	{
		Mouse::Wheel wheel{}; // Which wheel (for mice with multiple ones)
		float        delta{}; // Wheel offset (positive is up/left, negative is down/right). High-precision mice may use non-integral offsets.
		glm::ivec2  position; // Position of the mouse pointer, relative to the top left of the owner window
	};

	struct MouseButtonChanged
	{
		Mouse::Button button{}; // Code of the button that has been pressed
		glm::ivec2    position; // Position of the mouse pointer, relative to the top left of the owner window
	};
	struct MouseButtonPressed : MouseButtonChanged {};
	struct MouseButtonReleased : MouseButtonChanged {};

	struct MouseMoved
	{
		glm::ivec2 position; // Position of the mouse pointer, relative to the top left of the owner window
	};

	struct MouseEntered {};
	struct MouseLeft {};

	Event() = default;
	template <typename T>
	Event(const T& t);

	template <typename T>
	[[nodiscard]] bool Is() const;

	template <typename T>
	[[nodiscard]] const T* GetIf() const;

	[[nodiscard]] explicit operator bool() const
	{
		return !Is<Empty>();
	}

private:
	std::variant <Empty,
				Closed,
				Resized,
				FocusLost,
				FocusGained,
				TextEntered,
				KeyPressed,
				KeyReleased,
				MouseWheelScrolled,
				MouseButtonPressed,
				MouseButtonReleased,
				MouseMoved,
				MouseEntered,
				MouseLeft>
		m_data; // Event data

	template <typename T, typename... Ts>
	static constexpr bool isInParameterPack(const std::variant<Ts...>&)
	{
		return (std::is_same_v<T, Ts> || ...);
	}

	template <typename T>
	static constexpr bool isEventType = isInParameterPack<T>(decltype(m_data)());
};

template <typename T>
Event::Event(const T& t)
{
	static_assert(isEventType<T>, "T must be a subtype of sf::Event");
	if constexpr (isEventType<T>)
		m_data = t;
}

template <typename T>
bool Event::Is() const
{
	static_assert(isEventType<T>, "T must be a subtype of sf::Event");
	if constexpr (isEventType<T>)
		return std::holds_alternative<T>(m_data);
}

template <typename T>
const T* Event::GetIf() const
{
	static_assert(isEventType<T>, "T must be a subtype of sf::Event");
	if constexpr (isEventType<T>)
		return std::get_if<T>(&m_data);
}