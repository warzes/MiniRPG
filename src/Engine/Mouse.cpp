#include "stdafx.h"
#include "Mouse.h"
#include "WindowSystem.h"
//-----------------------------------------------------------------------------
bool Mouse::IsButtonPressed(Button button)
{
	int virtualKey = 0;
	switch (button)
	{
	case Mouse::Button::Left:
		virtualKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
		break;
	case Mouse::Button::Right:
		virtualKey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
		break;
	case Mouse::Button::Middle:
		virtualKey = VK_MBUTTON;
		break;
	case Mouse::Button::Extra1:
		virtualKey = VK_XBUTTON1;
		break;
	case Mouse::Button::Extra2:
		virtualKey = VK_XBUTTON2;
		break;
	default:
		virtualKey = 0;
		break;
	}

	return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}
//-----------------------------------------------------------------------------
glm::ivec2 Mouse::GetPosition()
{
	POINT point;
	GetCursorPos(&point);
	return { point.x, point.y };
}
//-----------------------------------------------------------------------------
glm::ivec2 Mouse::GetPosition(const WindowSystem& relativeTo)
{
	auto handle = relativeTo.GetNativeHandle();
	if (handle)
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(handle, &point);
		return { point.x, point.y };
	}
	else
	{
		return {};
	}
}
//-----------------------------------------------------------------------------
void Mouse::SetPosition(const glm::ivec2& position)
{
	SetCursorPos(position.x, position.y);
}
//-----------------------------------------------------------------------------
void Mouse::SetPosition(const glm::ivec2& position, const WindowSystem& relativeTo)
{
	auto handle = relativeTo.GetNativeHandle();
	if (handle)
	{
		POINT point = { position.x, position.y };
		ClientToScreen(handle, &point);
		SetCursorPos(point.x, point.y);
	}
}
//-----------------------------------------------------------------------------