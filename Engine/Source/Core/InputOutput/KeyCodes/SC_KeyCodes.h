#pragma once

enum class SC_KeyCode
{
	Zero,
	One,
	Two,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,

	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	F16,
	F17,
	F18,
	F19,
	F20,
	F21,
	F22,
	F23,
	F24,

	LeftArrow,
	UpArrow,
	RightArrow,
	DownArrow,

	Insert,
	Delete,
	Home,
	End,
	PageUp,
	PageDown,

	Escape,
	Tab,
	CapsLock,
	LeftShift,
	LeftControl,
	LeftAlt,

	Space,
	PrintScreen,
	ScrollLock,
	PauseBreak,

	Backspace,
	Enter,
	RightShift,
	RightContol,
	RightAlt,

	Shift,
	Control,

	NumPad0,
	NumPad1,
	NumPad2,
	NumPad3,
	NumPad4,
	NumPad5,
	NumPad6,
	NumPad7,
	NumPad8,
	NumPad9,
	NumLock,
	Multiply,
	Add,
	Seperator,
	Subtract,
	Decimal,
	Divide,

	Tilde,
	Paragraph,

	COUNT,
	Any
};

enum class SC_MouseKeyCode
{
	Left,
	Right,
	Middle,

	COUNT,
	Any
};

inline SC_KeyCode SC_ConvertToKeyCode(uint32 aKeyId)
{
#if IS_WINDOWS_PLATFORM
	switch (aKeyId)
	{
	case 0x30:
		return SC_KeyCode::Zero;
	case 0x31:
		return SC_KeyCode::One;
	case 0x32:
		return SC_KeyCode::Two;
	case 0x33:
		return SC_KeyCode::Three;
	case 0x34:
		return SC_KeyCode::Four;
	case 0x35:
		return SC_KeyCode::Five;
	case 0x36:
		return SC_KeyCode::Six;
	case 0x37:
		return SC_KeyCode::Seven;
	case 0x38:
		return SC_KeyCode::Eight;
	case 0x39:
		return SC_KeyCode::Nine;
	case 0x41:
		return SC_KeyCode::A;
	case 0x42:
		return SC_KeyCode::B;
	case 0x43:
		return SC_KeyCode::C;
	case 0x44:
		return SC_KeyCode::D;
	case 0x45:
		return SC_KeyCode::E;
	case 0x46:
		return SC_KeyCode::F;
	case 0x47:
		return SC_KeyCode::G;
	case 0x48:
		return SC_KeyCode::H;
	case 0x49:
		return SC_KeyCode::I;
	case 0x4A:
		return SC_KeyCode::J;
	case 0x4B:
		return SC_KeyCode::K;
	case 0x4C:
		return SC_KeyCode::L;
	case 0x4D:
		return SC_KeyCode::M;
	case 0x4E:
		return SC_KeyCode::N;
	case 0x4F:
		return SC_KeyCode::O;
	case 0x50:
		return SC_KeyCode::P;
	case 0x51:
		return SC_KeyCode::Q;
	case 0x52:
		return SC_KeyCode::R;
	case 0x53:
		return SC_KeyCode::S;
	case 0x54:
		return SC_KeyCode::T;
	case 0x55:
		return SC_KeyCode::U;
	case 0x56:
		return SC_KeyCode::V;
	case 0x57:
		return SC_KeyCode::W;
	case 0x58:
		return SC_KeyCode::X;
	case 0x59:
		return SC_KeyCode::Y;
	case 0x5A:
		return SC_KeyCode::Z;
	case VK_NUMPAD0:
		return SC_KeyCode::NumPad0;
	case VK_NUMPAD1:
		return SC_KeyCode::NumPad1;
	case VK_NUMPAD2:
		return SC_KeyCode::NumPad2;
	case VK_NUMPAD3:
		return SC_KeyCode::NumPad3;
	case VK_NUMPAD4:
		return SC_KeyCode::NumPad4;
	case VK_NUMPAD5:
		return SC_KeyCode::NumPad5;
	case VK_NUMPAD6:
		return SC_KeyCode::NumPad6;
	case VK_NUMPAD7:
		return SC_KeyCode::NumPad7;
	case VK_NUMPAD8:
		return SC_KeyCode::NumPad8;
	case VK_NUMPAD9:
		return SC_KeyCode::NumPad9;
	case VK_MULTIPLY:
		return SC_KeyCode::Multiply;
	case VK_ADD:
		return SC_KeyCode::Add;
	case VK_SEPARATOR:
		return SC_KeyCode::Seperator;
	case VK_SUBTRACT:
		return SC_KeyCode::Subtract;
	case VK_DECIMAL:
		return SC_KeyCode::Decimal;
	case VK_DIVIDE:
		return SC_KeyCode::Divide;
	case VK_F1:
		return SC_KeyCode::F1;
	case VK_F2:
		return SC_KeyCode::F2;
	case VK_F3:
		return SC_KeyCode::F3;
	case VK_F4:
		return SC_KeyCode::F4;
	case VK_F5:
		return SC_KeyCode::F5;
	case VK_F6:
		return SC_KeyCode::F6;
	case VK_F7:
		return SC_KeyCode::F7;
	case VK_F8:
		return SC_KeyCode::F8;
	case VK_F9:
		return SC_KeyCode::F9;
	case VK_F10:
		return SC_KeyCode::F10;
	case VK_F12:
		return SC_KeyCode::F12;
	case VK_F13:
		return SC_KeyCode::F13;
	case VK_F14:
		return SC_KeyCode::F14;
	case VK_F15:
		return SC_KeyCode::F15;
	case VK_F16:
		return SC_KeyCode::F16;
	case VK_F17:
		return SC_KeyCode::F17;
	case VK_F18:
		return SC_KeyCode::F18;
	case VK_F19:
		return SC_KeyCode::F19;
	case VK_F20:
		return SC_KeyCode::F20;
	case VK_F21:
		return SC_KeyCode::F21;
	case VK_F22:
		return SC_KeyCode::F22;
	case VK_F23:
		return SC_KeyCode::F23;
	case VK_F24:
		return SC_KeyCode::F24;
	case VK_LSHIFT:
		return SC_KeyCode::LeftShift;
	case VK_RSHIFT:
		return SC_KeyCode::RightShift;
	case VK_SHIFT:
		return SC_KeyCode::Shift;
	case VK_LCONTROL:
		return SC_KeyCode::LeftControl;
	case VK_RCONTROL:
		return SC_KeyCode::RightContol;
	case VK_CONTROL:
		return SC_KeyCode::Control;
	case VK_ESCAPE:
		return SC_KeyCode::Escape;
	case VK_EXECUTE:
	case VK_RETURN:
		return SC_KeyCode::Enter;
	case VK_BACK:
		return SC_KeyCode::Backspace;
	case VK_INSERT:
		return SC_KeyCode::Insert;
	case VK_DELETE:
		return SC_KeyCode::Delete;
	case VK_HOME:
		return SC_KeyCode::Home;
	case VK_END:
		return SC_KeyCode::End;
	case VK_PRIOR:
		return SC_KeyCode::PageUp;
	case VK_NEXT:
		return SC_KeyCode::PageDown;
	case VK_TAB:
		return SC_KeyCode::Tab;
	case VK_SNAPSHOT:
		return SC_KeyCode::PrintScreen;
	case VK_PAUSE:
		return SC_KeyCode::PauseBreak;
	case VK_SPACE:
		return SC_KeyCode::Space;
	case VK_OEM_5:
		return SC_KeyCode::Paragraph;
	case VK_OEM_3:
		return SC_KeyCode::Tilde;
	default:
		return SC_KeyCode::COUNT;
	}
#endif
}