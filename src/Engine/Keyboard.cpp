#include "stdafx.h"
#include "Keyboard.h"
#include "EnumArray.h"
//-----------------------------------------------------------------------------
namespace
{
	EnumArray<Keyboard::Key, Keyboard::Scancode, Keyboard::KeyCount> keyToScancodeMapping; // Mapping from Key to Scancode
	EnumArray<Keyboard::Scancode, Keyboard::Key, Keyboard::ScancodeCount> scancodeToKeyMapping; // Mapping from Scancode to Key

	Keyboard::Key virtualKeyToSfKey(UINT virtualKey)
	{
		switch (virtualKey)
		{
		case 'A':           return Keyboard::Key::A;
		case 'B':           return Keyboard::Key::B;
		case 'C':           return Keyboard::Key::C;
		case 'D':           return Keyboard::Key::D;
		case 'E':           return Keyboard::Key::E;
		case 'F':           return Keyboard::Key::F;
		case 'G':           return Keyboard::Key::G;
		case 'H':           return Keyboard::Key::H;
		case 'I':           return Keyboard::Key::I;
		case 'J':           return Keyboard::Key::J;
		case 'K':           return Keyboard::Key::K;
		case 'L':           return Keyboard::Key::L;
		case 'M':           return Keyboard::Key::M;
		case 'N':           return Keyboard::Key::N;
		case 'O':           return Keyboard::Key::O;
		case 'P':           return Keyboard::Key::P;
		case 'Q':           return Keyboard::Key::Q;
		case 'R':           return Keyboard::Key::R;
		case 'S':           return Keyboard::Key::S;
		case 'T':           return Keyboard::Key::T;
		case 'U':           return Keyboard::Key::U;
		case 'V':           return Keyboard::Key::V;
		case 'W':           return Keyboard::Key::W;
		case 'X':           return Keyboard::Key::X;
		case 'Y':           return Keyboard::Key::Y;
		case 'Z':           return Keyboard::Key::Z;
		case '0':           return Keyboard::Key::Num0;
		case '1':           return Keyboard::Key::Num1;
		case '2':           return Keyboard::Key::Num2;
		case '3':           return Keyboard::Key::Num3;
		case '4':           return Keyboard::Key::Num4;
		case '5':           return Keyboard::Key::Num5;
		case '6':           return Keyboard::Key::Num6;
		case '7':           return Keyboard::Key::Num7;
		case '8':           return Keyboard::Key::Num8;
		case '9':           return Keyboard::Key::Num9;
		case VK_ESCAPE:     return Keyboard::Key::Escape;
		case VK_LCONTROL:   return Keyboard::Key::LControl;
		case VK_LSHIFT:     return Keyboard::Key::LShift;
		case VK_LMENU:      return Keyboard::Key::LAlt;
		case VK_LWIN:       return Keyboard::Key::LSystem;
		case VK_RCONTROL:   return Keyboard::Key::RControl;
		case VK_RSHIFT:     return Keyboard::Key::RShift;
		case VK_RMENU:      return Keyboard::Key::RAlt;
		case VK_RWIN:       return Keyboard::Key::RSystem;
		case VK_APPS:       return Keyboard::Key::Menu;
		case VK_OEM_4:      return Keyboard::Key::LBracket;
		case VK_OEM_6:      return Keyboard::Key::RBracket;
		case VK_OEM_1:      return Keyboard::Key::Semicolon;
		case VK_OEM_COMMA:  return Keyboard::Key::Comma;
		case VK_OEM_PERIOD: return Keyboard::Key::Period;
		case VK_OEM_7:      return Keyboard::Key::Apostrophe;
		case VK_OEM_2:      return Keyboard::Key::Slash;
		case VK_OEM_5:      return Keyboard::Key::Backslash;
		case VK_OEM_3:      return Keyboard::Key::Grave;
		case VK_OEM_PLUS:   return Keyboard::Key::Equal;
		case VK_OEM_MINUS:  return Keyboard::Key::Hyphen;
		case VK_SPACE:      return Keyboard::Key::Space;
		case VK_RETURN:     return Keyboard::Key::Enter;
		case VK_BACK:       return Keyboard::Key::Backspace;
		case VK_TAB:        return Keyboard::Key::Tab;
		case VK_PRIOR:      return Keyboard::Key::PageUp;
		case VK_NEXT:       return Keyboard::Key::PageDown;
		case VK_END:        return Keyboard::Key::End;
		case VK_HOME:       return Keyboard::Key::Home;
		case VK_INSERT:     return Keyboard::Key::Insert;
		case VK_DELETE:     return Keyboard::Key::Delete;
		case VK_ADD:        return Keyboard::Key::Add;
		case VK_SUBTRACT:   return Keyboard::Key::Subtract;
		case VK_MULTIPLY:   return Keyboard::Key::Multiply;
		case VK_DIVIDE:     return Keyboard::Key::Divide;
		case VK_LEFT:       return Keyboard::Key::Left;
		case VK_RIGHT:      return Keyboard::Key::Right;
		case VK_UP:         return Keyboard::Key::Up;
		case VK_DOWN:       return Keyboard::Key::Down;
		case VK_NUMPAD0:    return Keyboard::Key::Numpad0;
		case VK_NUMPAD1:    return Keyboard::Key::Numpad1;
		case VK_NUMPAD2:    return Keyboard::Key::Numpad2;
		case VK_NUMPAD3:    return Keyboard::Key::Numpad3;
		case VK_NUMPAD4:    return Keyboard::Key::Numpad4;
		case VK_NUMPAD5:    return Keyboard::Key::Numpad5;
		case VK_NUMPAD6:    return Keyboard::Key::Numpad6;
		case VK_NUMPAD7:    return Keyboard::Key::Numpad7;
		case VK_NUMPAD8:    return Keyboard::Key::Numpad8;
		case VK_NUMPAD9:    return Keyboard::Key::Numpad9;
		case VK_F1:         return Keyboard::Key::F1;
		case VK_F2:         return Keyboard::Key::F2;
		case VK_F3:         return Keyboard::Key::F3;
		case VK_F4:         return Keyboard::Key::F4;
		case VK_F5:         return Keyboard::Key::F5;
		case VK_F6:         return Keyboard::Key::F6;
		case VK_F7:         return Keyboard::Key::F7;
		case VK_F8:         return Keyboard::Key::F8;
		case VK_F9:         return Keyboard::Key::F9;
		case VK_F10:        return Keyboard::Key::F10;
		case VK_F11:        return Keyboard::Key::F11;
		case VK_F12:        return Keyboard::Key::F12;
		case VK_F13:        return Keyboard::Key::F13;
		case VK_F14:        return Keyboard::Key::F14;
		case VK_F15:        return Keyboard::Key::F15;
		case VK_PAUSE:      return Keyboard::Key::Pause;
		default:            return Keyboard::Key::Unknown;
		}
	}

	int keyToVirtualKey(Keyboard::Key key)
	{
		switch (key)
		{
		case Keyboard::Key::A:          return 'A';
		case Keyboard::Key::B:          return 'B';
		case Keyboard::Key::C:          return 'C';
		case Keyboard::Key::D:          return 'D';
		case Keyboard::Key::E:          return 'E';
		case Keyboard::Key::F:          return 'F';
		case Keyboard::Key::G:          return 'G';
		case Keyboard::Key::H:          return 'H';
		case Keyboard::Key::I:          return 'I';
		case Keyboard::Key::J:          return 'J';
		case Keyboard::Key::K:          return 'K';
		case Keyboard::Key::L:          return 'L';
		case Keyboard::Key::M:          return 'M';
		case Keyboard::Key::N:          return 'N';
		case Keyboard::Key::O:          return 'O';
		case Keyboard::Key::P:          return 'P';
		case Keyboard::Key::Q:          return 'Q';
		case Keyboard::Key::R:          return 'R';
		case Keyboard::Key::S:          return 'S';
		case Keyboard::Key::T:          return 'T';
		case Keyboard::Key::U:          return 'U';
		case Keyboard::Key::V:          return 'V';
		case Keyboard::Key::W:          return 'W';
		case Keyboard::Key::X:          return 'X';
		case Keyboard::Key::Y:          return 'Y';
		case Keyboard::Key::Z:          return 'Z';
		case Keyboard::Key::Num0:       return '0';
		case Keyboard::Key::Num1:       return '1';
		case Keyboard::Key::Num2:       return '2';
		case Keyboard::Key::Num3:       return '3';
		case Keyboard::Key::Num4:       return '4';
		case Keyboard::Key::Num5:       return '5';
		case Keyboard::Key::Num6:       return '6';
		case Keyboard::Key::Num7:       return '7';
		case Keyboard::Key::Num8:       return '8';
		case Keyboard::Key::Num9:       return '9';
		case Keyboard::Key::Escape:     return VK_ESCAPE;
		case Keyboard::Key::LControl:   return VK_LCONTROL;
		case Keyboard::Key::LShift:     return VK_LSHIFT;
		case Keyboard::Key::LAlt:       return VK_LMENU;
		case Keyboard::Key::LSystem:    return VK_LWIN;
		case Keyboard::Key::RControl:   return VK_RCONTROL;
		case Keyboard::Key::RShift:     return VK_RSHIFT;
		case Keyboard::Key::RAlt:       return VK_RMENU;
		case Keyboard::Key::RSystem:    return VK_RWIN;
		case Keyboard::Key::Menu:       return VK_APPS;
		case Keyboard::Key::LBracket:   return VK_OEM_4;
		case Keyboard::Key::RBracket:   return VK_OEM_6;
		case Keyboard::Key::Semicolon:  return VK_OEM_1;
		case Keyboard::Key::Comma:      return VK_OEM_COMMA;
		case Keyboard::Key::Period:     return VK_OEM_PERIOD;
		case Keyboard::Key::Apostrophe: return VK_OEM_7;
		case Keyboard::Key::Slash:      return VK_OEM_2;
		case Keyboard::Key::Backslash:  return VK_OEM_5;
		case Keyboard::Key::Grave:      return VK_OEM_3;
		case Keyboard::Key::Equal:      return VK_OEM_PLUS;
		case Keyboard::Key::Hyphen:     return VK_OEM_MINUS;
		case Keyboard::Key::Space:      return VK_SPACE;
		case Keyboard::Key::Enter:      return VK_RETURN;
		case Keyboard::Key::Backspace:  return VK_BACK;
		case Keyboard::Key::Tab:        return VK_TAB;
		case Keyboard::Key::PageUp:     return VK_PRIOR;
		case Keyboard::Key::PageDown:   return VK_NEXT;
		case Keyboard::Key::End:        return VK_END;
		case Keyboard::Key::Home:       return VK_HOME;
		case Keyboard::Key::Insert:     return VK_INSERT;
		case Keyboard::Key::Delete:     return VK_DELETE;
		case Keyboard::Key::Add:        return VK_ADD;
		case Keyboard::Key::Subtract:   return VK_SUBTRACT;
		case Keyboard::Key::Multiply:   return VK_MULTIPLY;
		case Keyboard::Key::Divide:     return VK_DIVIDE;
		case Keyboard::Key::Left:       return VK_LEFT;
		case Keyboard::Key::Right:      return VK_RIGHT;
		case Keyboard::Key::Up:         return VK_UP;
		case Keyboard::Key::Down:       return VK_DOWN;
		case Keyboard::Key::Numpad0:    return VK_NUMPAD0;
		case Keyboard::Key::Numpad1:    return VK_NUMPAD1;
		case Keyboard::Key::Numpad2:    return VK_NUMPAD2;
		case Keyboard::Key::Numpad3:    return VK_NUMPAD3;
		case Keyboard::Key::Numpad4:    return VK_NUMPAD4;
		case Keyboard::Key::Numpad5:    return VK_NUMPAD5;
		case Keyboard::Key::Numpad6:    return VK_NUMPAD6;
		case Keyboard::Key::Numpad7:    return VK_NUMPAD7;
		case Keyboard::Key::Numpad8:    return VK_NUMPAD8;
		case Keyboard::Key::Numpad9:    return VK_NUMPAD9;
		case Keyboard::Key::F1:         return VK_F1;
		case Keyboard::Key::F2:         return VK_F2;
		case Keyboard::Key::F3:         return VK_F3;
		case Keyboard::Key::F4:         return VK_F4;
		case Keyboard::Key::F5:         return VK_F5;
		case Keyboard::Key::F6:         return VK_F6;
		case Keyboard::Key::F7:         return VK_F7;
		case Keyboard::Key::F8:         return VK_F8;
		case Keyboard::Key::F9:         return VK_F9;
		case Keyboard::Key::F10:        return VK_F10;
		case Keyboard::Key::F11:        return VK_F11;
		case Keyboard::Key::F12:        return VK_F12;
		case Keyboard::Key::F13:        return VK_F13;
		case Keyboard::Key::F14:        return VK_F14;
		case Keyboard::Key::F15:        return VK_F15;
		case Keyboard::Key::Pause:      return VK_PAUSE;
		default:                        return 0;
		}
	}

	WORD scanToWinScan(Keyboard::Scancode code)
	{
		// Convert an SFML scancode to a Windows scancode
		// Reference: https://msdn.microsoft.com/en-us/library/aa299374(v=vs.60).aspx
		switch (code)
		{
		case Keyboard::Scancode::A: return 0x1E;
		case Keyboard::Scancode::B: return 0x30;
		case Keyboard::Scancode::C: return 0x2E;
		case Keyboard::Scancode::D: return 0x20;
		case Keyboard::Scancode::E: return 0x12;
		case Keyboard::Scancode::F: return 0x21;
		case Keyboard::Scancode::G: return 0x22;
		case Keyboard::Scancode::H: return 0x23;
		case Keyboard::Scancode::I: return 0x17;
		case Keyboard::Scancode::J: return 0x24;
		case Keyboard::Scancode::K: return 0x25;
		case Keyboard::Scancode::L: return 0x26;
		case Keyboard::Scancode::M: return 0x32;
		case Keyboard::Scancode::N: return 0x31;
		case Keyboard::Scancode::O: return 0x18;
		case Keyboard::Scancode::P: return 0x19;
		case Keyboard::Scancode::Q: return 0x10;
		case Keyboard::Scancode::R: return 0x13;
		case Keyboard::Scancode::S: return 0x1F;
		case Keyboard::Scancode::T: return 0x14;
		case Keyboard::Scancode::U: return 0x16;
		case Keyboard::Scancode::V: return 0x2F;
		case Keyboard::Scancode::W: return 0x11;
		case Keyboard::Scancode::X: return 0x2D;
		case Keyboard::Scancode::Y: return 0x15;
		case Keyboard::Scancode::Z: return 0x2C;

		case Keyboard::Scancode::Num1: return 0x02;
		case Keyboard::Scancode::Num2: return 0x03;
		case Keyboard::Scancode::Num3: return 0x04;
		case Keyboard::Scancode::Num4: return 0x05;
		case Keyboard::Scancode::Num5: return 0x06;
		case Keyboard::Scancode::Num6: return 0x07;
		case Keyboard::Scancode::Num7: return 0x08;
		case Keyboard::Scancode::Num8: return 0x09;
		case Keyboard::Scancode::Num9: return 0x0A;
		case Keyboard::Scancode::Num0: return 0x0B;

		case Keyboard::Scancode::Enter:      return 0x1C;
		case Keyboard::Scancode::Escape:     return 0x01;
		case Keyboard::Scancode::Backspace:  return 0x0E;
		case Keyboard::Scancode::Tab:        return 0x0F;
		case Keyboard::Scancode::Space:      return 0x39;
		case Keyboard::Scancode::Hyphen:     return 0x0C;
		case Keyboard::Scancode::Equal:      return 0x0D;
		case Keyboard::Scancode::LBracket:   return 0x1A;
		case Keyboard::Scancode::RBracket:   return 0x1B;
		case Keyboard::Scancode::Backslash:  return 0x2B;
		case Keyboard::Scancode::Semicolon:  return 0x27;
		case Keyboard::Scancode::Apostrophe: return 0x28;
		case Keyboard::Scancode::Grave:      return 0x29;
		case Keyboard::Scancode::Comma:      return 0x33;
		case Keyboard::Scancode::Period:     return 0x34;
		case Keyboard::Scancode::Slash:      return 0x35;

		case Keyboard::Scancode::F1:  return 0x3B;
		case Keyboard::Scancode::F2:  return 0x3C;
		case Keyboard::Scancode::F3:  return 0x3D;
		case Keyboard::Scancode::F4:  return 0x3E;
		case Keyboard::Scancode::F5:  return 0x3F;
		case Keyboard::Scancode::F6:  return 0x40;
		case Keyboard::Scancode::F7:  return 0x41;
		case Keyboard::Scancode::F8:  return 0x42;
		case Keyboard::Scancode::F9:  return 0x43;
		case Keyboard::Scancode::F10: return 0x44;
		case Keyboard::Scancode::F11: return 0x57;
		case Keyboard::Scancode::F12: return 0x58;
		case Keyboard::Scancode::F13: return 0x64;
		case Keyboard::Scancode::F14: return 0x65;
		case Keyboard::Scancode::F15: return 0x66;
		case Keyboard::Scancode::F16: return 0x67;
		case Keyboard::Scancode::F17: return 0x68;
		case Keyboard::Scancode::F18: return 0x69;
		case Keyboard::Scancode::F19: return 0x6A;
		case Keyboard::Scancode::F20: return 0x6B;
		case Keyboard::Scancode::F21: return 0x6C;
		case Keyboard::Scancode::F22: return 0x6D;
		case Keyboard::Scancode::F23: return 0x6E;
		case Keyboard::Scancode::F24: return 0x76;

		case Keyboard::Scancode::CapsLock:    return 0x3A;
		case Keyboard::Scancode::PrintScreen: return 0xE037;
		case Keyboard::Scancode::ScrollLock:  return 0x46;
		case Keyboard::Scancode::Pause:       return 0x45;
		case Keyboard::Scancode::Insert:      return 0xE052;
		case Keyboard::Scancode::Home:        return 0xE047;
		case Keyboard::Scancode::PageUp:      return 0xE049;
		case Keyboard::Scancode::Delete:      return 0xE053;
		case Keyboard::Scancode::End:         return 0xE04F;
		case Keyboard::Scancode::PageDown:    return 0xE051;
		case Keyboard::Scancode::Right:       return 0xE04D;
		case Keyboard::Scancode::Left:        return 0xE04B;
		case Keyboard::Scancode::Down:        return 0xE050;
		case Keyboard::Scancode::Up:          return 0xE048;
		case Keyboard::Scancode::NumLock:     return 0xE045;

		case Keyboard::Scancode::NumpadDivide:   return 0xE035;
		case Keyboard::Scancode::NumpadMultiply: return 0x37;
		case Keyboard::Scancode::NumpadMinus:    return 0x4A;
		case Keyboard::Scancode::NumpadPlus:     return 0x4E;
		case Keyboard::Scancode::NumpadEqual:    return 0x7E;
		case Keyboard::Scancode::NumpadEnter:    return 0xE01C;
		case Keyboard::Scancode::NumpadDecimal:  return 0x53;

		case Keyboard::Scancode::Numpad1:        return 0x4F;
		case Keyboard::Scancode::Numpad2:        return 0x50;
		case Keyboard::Scancode::Numpad3:        return 0x51;
		case Keyboard::Scancode::Numpad4:        return 0x4B;
		case Keyboard::Scancode::Numpad5:        return 0x4C;
		case Keyboard::Scancode::Numpad6:        return 0x4D;
		case Keyboard::Scancode::Numpad7:        return 0x47;
		case Keyboard::Scancode::Numpad8:        return 0x48;
		case Keyboard::Scancode::Numpad9:        return 0x49;
		case Keyboard::Scancode::Numpad0:        return 0x52;

		case Keyboard::Scancode::NonUsBackslash: return 0x56;
		// No known scancode for Keyboard::Scan::Application
		// No known scancode for Keyboard::Scan::Execute
		// No known scancode for Keyboard::Scan::ModeChange
		case Keyboard::Scancode::Help:           return 0xE061;
		case Keyboard::Scancode::Menu:           return 0xE05D;
		case Keyboard::Scancode::Select:         return 0xE01E;
		// No known scancode for Keyboard::Scan::Redo
		// No known scancode for Keyboard::Scan::Undo
		// No known scancode for Keyboard::Scan::Cut
		// No known scancode for Keyboard::Scan::Copy
		// No known scancode for Keyboard::Scan::Paste

		case Keyboard::Scancode::VolumeMute:         return 0xE020;
		case Keyboard::Scancode::VolumeUp:           return 0xE02E;
		case Keyboard::Scancode::VolumeDown:         return 0xE02C;
		case Keyboard::Scancode::MediaPlayPause:     return 0xE022;
		case Keyboard::Scancode::MediaStop:          return 0xE024;
		case Keyboard::Scancode::MediaNextTrack:     return 0xE019;
		case Keyboard::Scancode::MediaPreviousTrack: return 0xE010;

		case Keyboard::Scancode::LControl: return 0x1D;
		case Keyboard::Scancode::LShift:   return 0x2A;
		case Keyboard::Scancode::LAlt:     return 0x38;
		case Keyboard::Scancode::LSystem:  return 0xE05B;
		case Keyboard::Scancode::RControl: return 0xE01D;
		case Keyboard::Scancode::RShift:   return 0x36;
		case Keyboard::Scancode::RAlt:     return 0xE038;
		case Keyboard::Scancode::RSystem:  return 0xE05C;

		case Keyboard::Scancode::Back:      return 0xE06A;
		case Keyboard::Scancode::Forward:   return 0xE069;
		case Keyboard::Scancode::Refresh:   return 0xE067;
		case Keyboard::Scancode::Stop:      return 0xE068;
		case Keyboard::Scancode::Search:    return 0xE065;
		case Keyboard::Scancode::Favorites: return 0xE066;
		case Keyboard::Scancode::HomePage:  return 0xE030;

		case Keyboard::Scancode::LaunchApplication1: return 0xE06B;
		case Keyboard::Scancode::LaunchApplication2: return 0xE021;
		case Keyboard::Scancode::LaunchMail:         return 0xE06C;
		case Keyboard::Scancode::LaunchMediaSelect:  return 0xE06D;

		// Unable to map to a scancode
		default: return 0x0;
		}
	}

	WORD scanToWinScanExtended(Keyboard::Scancode code)
	{
		// Convert an SFML scancode to a Windows scancode
		// Reference: https://msdn.microsoft.com/en-us/library/aa299374(v=vs.60).aspx
		switch (code)
		{
		case Keyboard::Scancode::PrintScreen:        return 55 | 0xE100;
		case Keyboard::Scancode::Insert:             return 82 | 0xE100;
		case Keyboard::Scancode::Home:               return 71 | 0xE100;
		case Keyboard::Scancode::PageUp:             return 73 | 0xE100;
		case Keyboard::Scancode::Delete:             return 83 | 0xE100;
		case Keyboard::Scancode::End:                return 79 | 0xE100;
		case Keyboard::Scancode::PageDown:           return 81 | 0xE100;
		case Keyboard::Scancode::Right:              return 77 | 0xE100;
		case Keyboard::Scancode::Left:               return 75 | 0xE100;
		case Keyboard::Scancode::Down:               return 80 | 0xE100;
		case Keyboard::Scancode::Up:                 return 72 | 0xE100;
		case Keyboard::Scancode::NumLock:            return 69 | 0xE100;
		case Keyboard::Scancode::NumpadEnter:        return 28 | 0xE100;
		case Keyboard::Scancode::NumpadDivide:       return 53 | 0xE100;
		case Keyboard::Scancode::Help:               return 97 | 0xE100;
		case Keyboard::Scancode::Menu:               return 93 | 0xE100;
		case Keyboard::Scancode::Select:             return 30 | 0xE100;
		case Keyboard::Scancode::VolumeMute:         return 32 | 0xE100;
		case Keyboard::Scancode::VolumeUp:           return 46 | 0xE100;
		case Keyboard::Scancode::VolumeDown:         return 44 | 0xE100;
		case Keyboard::Scancode::MediaPlayPause:     return 34 | 0xE100;
		case Keyboard::Scancode::MediaStop:          return 36 | 0xE100;
		case Keyboard::Scancode::MediaNextTrack:     return 25 | 0xE100;
		case Keyboard::Scancode::MediaPreviousTrack: return 16 | 0xE100;
		case Keyboard::Scancode::LSystem:            return 91 | 0xE100;
		case Keyboard::Scancode::RControl:           return 29 | 0xE100;
		case Keyboard::Scancode::RAlt:               return 56 | 0xE100;
		case Keyboard::Scancode::RSystem:            return 92 | 0xE100;
		case Keyboard::Scancode::Back:               return 106 | 0xE100;
		case Keyboard::Scancode::Forward:            return 105 | 0xE100;
		case Keyboard::Scancode::Refresh:            return 103 | 0xE100;
		case Keyboard::Scancode::Stop:               return 104 | 0xE100;
		case Keyboard::Scancode::Search:             return 101 | 0xE100;
		case Keyboard::Scancode::Favorites:          return 102 | 0xE100;
		case Keyboard::Scancode::HomePage:           return 48 | 0xE100;
		case Keyboard::Scancode::LaunchApplication1: return 107 | 0xE100;
		case Keyboard::Scancode::LaunchApplication2: return 33 | 0xE100;
		case Keyboard::Scancode::LaunchMail:         return 108 | 0xE100;
		case Keyboard::Scancode::LaunchMediaSelect:  return 109 | 0xE100;

		// Use non-extended mapping
		default: return scanToWinScan(code);
		}
	}

	UINT scanToVirtualKey(Keyboard::Scancode code)
	{
		const WORD winScancode = scanToWinScan(code);

		// Manually map non-extended key codes
		switch (code)
		{
		case Keyboard::Scancode::Numpad0:       return VK_NUMPAD0;
		case Keyboard::Scancode::Numpad1:       return VK_NUMPAD1;
		case Keyboard::Scancode::Numpad2:       return VK_NUMPAD2;
		case Keyboard::Scancode::Numpad3:       return VK_NUMPAD3;
		case Keyboard::Scancode::Numpad4:       return VK_NUMPAD4;
		case Keyboard::Scancode::Numpad5:       return VK_NUMPAD5;
		case Keyboard::Scancode::Numpad6:       return VK_NUMPAD6;
		case Keyboard::Scancode::Numpad7:       return VK_NUMPAD7;
		case Keyboard::Scancode::Numpad8:       return VK_NUMPAD8;
		case Keyboard::Scancode::Numpad9:       return VK_NUMPAD9;
		case Keyboard::Scancode::NumpadMinus:   return VK_SUBTRACT;
		case Keyboard::Scancode::NumpadDecimal: return VK_DECIMAL;
		case Keyboard::Scancode::NumpadDivide:  return VK_DIVIDE;
		case Keyboard::Scancode::Pause:         return VK_PAUSE;
		case Keyboard::Scancode::RControl:      return VK_RCONTROL;
		case Keyboard::Scancode::RAlt:          return VK_RMENU;
		default:                                return MapVirtualKey(winScancode, MAPVK_VSC_TO_VK_EX);
		}
	}

	std::optional<String> scanToConsumerKeyName(Keyboard::Scancode code)
	{
		// Convert an SFML scancode to a Windows consumer keyboard key name
		// Reference: https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-messages
		switch (code)
		{
		case Keyboard::Scancode::MediaNextTrack:     return "Next Track";
		case Keyboard::Scancode::MediaPreviousTrack: return "Previous Track";
		case Keyboard::Scancode::MediaStop:          return "Stop";
		case Keyboard::Scancode::MediaPlayPause:     return "Play/Pause";
		case Keyboard::Scancode::VolumeMute:         return "Mute";
		case Keyboard::Scancode::VolumeUp:           return "Volume Increment";
		case Keyboard::Scancode::VolumeDown:         return "Volume Decrement";
		case Keyboard::Scancode::LaunchMediaSelect:  return "Consumer Control Configuration";
		case Keyboard::Scancode::LaunchMail:         return "Email Reader";
		case Keyboard::Scancode::LaunchApplication2: return "Calculator";
		case Keyboard::Scancode::LaunchApplication1: return "Local Machine Browser";
		case Keyboard::Scancode::Search:             return "Search";
		case Keyboard::Scancode::HomePage:           return "Home";
		case Keyboard::Scancode::Back:               return "Back";
		case Keyboard::Scancode::Forward:            return "Forward";
		case Keyboard::Scancode::Stop:               return "Stop";
		case Keyboard::Scancode::Refresh:            return "Refresh";
		case Keyboard::Scancode::Favorites:          return "Bookmarks";

		// Not a consumer key
		default: return std::nullopt;
		}
	}

	// Ensure the mappings are generated from/to Key and Scancode.
	void ensureMappings()
	{
		static bool isMappingInitialized = false;

		if (isMappingInitialized)
			return;

		// Phase 1: Initialize mappings with default values
		keyToScancodeMapping.fill(Keyboard::Scancode::Unknown);
		scancodeToKeyMapping.fill(Keyboard::Key::Unknown);

		// Phase 2: Translate scancode to virtual code to key names
		for (unsigned int i = 0; i < Keyboard::ScancodeCount; ++i)
		{
			const auto              scan = static_cast<Keyboard::Scancode>(i);
			const UINT              virtualKey = scanToVirtualKey(scan);
			const Keyboard::Key key = virtualKeyToSfKey(virtualKey);
			if (key != Keyboard::Key::Unknown && keyToScancodeMapping[key] == Keyboard::Scancode::Unknown)
				keyToScancodeMapping[key] = scan;
			scancodeToKeyMapping[scan] = key;
		}

		isMappingInitialized = true;
	}

	bool isValidScancode(Keyboard::Scancode code)
	{
		return code > Keyboard::Scancode::Unknown && static_cast<unsigned int>(code) < Keyboard::ScancodeCount;
	}

	bool isValidKey(Keyboard::Key key)
	{
		return key > Keyboard::Key::Unknown && static_cast<unsigned int>(key) < Keyboard::KeyCount;
	}

} // namespace
//-----------------------------------------------------------------------------
bool Keyboard::IsKeyPressed(Key key)
{
	const int virtualKey = keyToVirtualKey(key);
	return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}
//-----------------------------------------------------------------------------
bool Keyboard::IsKeyPressed(Scancode code)
{
	const UINT virtualKey = scanToVirtualKey(code);
	return (GetAsyncKeyState(static_cast<int>(virtualKey)) & KF_UP) != 0;
}
//-----------------------------------------------------------------------------
Keyboard::Key Keyboard::Localize(Scancode code)
{
	if (!isValidScancode(code))
		return Keyboard::Key::Unknown;

	ensureMappings();

	return scancodeToKeyMapping[code];
}
//-----------------------------------------------------------------------------
Keyboard::Scancode Keyboard::Delocalize(Key key)
{
	if (!isValidKey(key))
		return Keyboard::Scancode::Unknown;

	ensureMappings();

	return keyToScancodeMapping[key];
}
//-----------------------------------------------------------------------------
String Keyboard::GetDescription(Scancode code)
{
	// Try to translate the scan code to a consumer key
	if (const auto consumerKeyName = scanToConsumerKeyName(code))
		return *consumerKeyName;

	WORD      winCode = scanToWinScanExtended(code);
	const int bufSize = 1024;
	WCHAR     name[bufSize];

	// Remap F13-F23 to values supported by GetKeyNameText
	if ((winCode >= 0x64) && (winCode <= 0x6E))
		winCode += 0x18;
	// Remap F24 to value supported by GetKeyNameText
	if (winCode == 0x76)
		winCode = 0x87;

	const int result = GetKeyNameText(winCode << 16, name, bufSize);
	if (result > 0)
	{
		return name;
	}
	return "Unknown";
}
//-----------------------------------------------------------------------------