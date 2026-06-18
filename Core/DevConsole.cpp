#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------
// Static color definitions
//--------------------------------------------------------------------------------------
const Rgba8 DevConsole::COLOR_ERROR = Rgba8(255, 0, 0);
const Rgba8 DevConsole::COLOR_WARNING = Rgba8(255, 255, 0);
const Rgba8 DevConsole::COLOR_INFO_MAJOR = Rgba8(0, 255, 0);
const Rgba8 DevConsole::COLOR_INFO_MINOR = Rgba8(255, 180, 180);

DevConsole::DevConsole(DevConsoleConfig const& config)
	: m_config(config)
{
	/*m_config.m_renderer = g_engine->m_render;*/
}

DevConsole::~DevConsole()
{
}

void DevConsole::Startup()
{
	// Load font from config (whatever your engine uses)
	/*m_font = m_config.m_renderer->CreateOrGetBitmapFont(m_config.m_fontName.c_str());*/
	m_font = g_engine->m_render->CreateOrGetBitmapFont(m_config.m_fontName.c_str());

	m_isOpen = m_config.m_startOpen;

	// Blink timer (0.5s is typical; adjust if your assignment says otherwise)
	m_insertionPointBlinkTimer = new Timer(0.5f);
	m_insertionPointBlinkTimer->Start();

	// Subscribe to input events
// 	if (m_config.m_eventSystem)
// 	{
// 		m_config.m_eventSystem->SubscribeEventCallbackFunction("KeyPressed", &DevConsole::Event_KeyPressed);
// 		m_config.m_eventSystem->SubscribeEventCallbackFunction("CharInput", &DevConsole::Event_CharInput);
// 
// 		// Commands
// 		m_config.m_eventSystem->SubscribeEventCallbackFunction("clear", &DevConsole::Command_Clear);
// 		m_config.m_eventSystem->SubscribeEventCallbackFunction("echo", &DevConsole::Command_Echo);
// 		m_config.m_eventSystem->SubscribeEventCallbackFunction("help", &DevConsole::Command_Help);

		g_engine->m_event->SubscribeEventCallbackFunction("KeyPressed", &DevConsole::Event_KeyPressed);
		g_engine->m_event->SubscribeEventCallbackFunction("CharInput", &DevConsole::Event_CharInput);

		// Commands
		g_engine->m_event->SubscribeEventCallbackFunction("clear", &DevConsole::Command_Clear);
		g_engine->m_event->SubscribeEventCallbackFunction("echo", &DevConsole::Command_Echo);
		g_engine->m_event->SubscribeEventCallbackFunction("help", &DevConsole::Command_Help);
		/*}*/

	AddLine(COLOR_INFO_MINOR, "DevConsole ready. Press ` to toggle.");
}

void DevConsole::Shutdown()
{
	m_lines.clear();
	delete m_insertionPointBlinkTimer;
	m_insertionPointBlinkTimer = nullptr;
}

void DevConsole::BeginFrame()
{
	if (!m_isOpen)
		return;

	if (m_insertionPointBlinkTimer && m_insertionPointBlinkTimer->DecrementPeriodIfElapsed())
	{
		m_insertionPointVisible = !m_insertionPointVisible;
	}
}

void DevConsole::EndFrame()
{
}

void DevConsole::Execute(std::string const& consoleCommandText)
{
	// Add raw input line to console history
	AddLine(COLOR_INFO_MAJOR, consoleCommandText);

	// Split input into tokens
	Strings commandStrings = SplitStringOnDelimiter(consoleCommandText, ' ');

	if (commandStrings.size() == 0)
		return;

	std::string eventName = commandStrings[0];

	EventArgs args;
	for (int i = 1; i < commandStrings.size(); ++i)
	{
		std::string key = Stringf("arg%d", i);
		args.SetValue(key, commandStrings[i]);
	}

	g_engine->m_event->FireEvent(eventName, args);
}


void DevConsole::AddLine(Rgba8 const& color, std::string const& text)
{
	DevConsoleLine line;
	line.text = text;
	line.color = color;
	m_lines.push_back(line);
}

DevConsoleMode DevConsole::GetMode() const
{
	return m_mode;
}

void DevConsole::SetMode(DevConsoleMode mode)
{
	m_mode = mode;
}

// void DevConsole::ToggleMode(DevConsoleMode mode)
// {
// 	if (m_mode == mode)
// 		m_mode = DevConsoleMode::HIDDEN;
// 	else
// 		m_mode = mode;
// }

//======================================================================================
// RENDER
//======================================================================================
void DevConsole::Render(AABB2 const& bounds) const
{
	if (!m_isOpen)
	{
		return;
	}

	if (m_mode == DevConsoleMode::OPEN_FULL)
	{
		Render_OpenFull(bounds, *m_font, 1.f);
	}
}

void DevConsole::Render_OpenFull(AABB2 const& bounds, BitmapFont& font, float fontAspect) const
{
	std::vector<Vertex> textVerts;
	std::vector<Vertex> boxVerts;

	AddVertsForAABB2(boxVerts, bounds, Rgba8(0, 0, 0, 200), Vec2(0.f, 0.f), Vec2(1.f, 1.f));

	//Draw Command History
	int linesOnScreen = m_config.m_linesOnScreen;
	if (linesOnScreen < 2) linesOnScreen = 2;

	float lineHeight = bounds.GetDimensions().y / (float)linesOnScreen;

	int maxHistoryLinesVisible = linesOnScreen - 1;
	if (maxHistoryLinesVisible < 1) maxHistoryLinesVisible = 1;

	int lastIndex = (int)m_lines.size() - 1;
	int firstIndex = lastIndex - maxHistoryLinesVisible + 1;
	if (firstIndex < 0) firstIndex = 0;

	float yBottom = bounds.m_mins.y + lineHeight;

	for (int i = lastIndex; i >= firstIndex; --i)
	{
		AABB2 lineBounds(
			Vec2(bounds.m_mins.x + 0.2f, yBottom),
			Vec2(bounds.m_maxs.x - 0.2f, yBottom + lineHeight)
		);

		font.AddVertsForTextInBox2D(
			textVerts,
			m_lines[i].text,
			lineBounds,
			lineHeight,
			m_lines[i].color,
			fontAspect,
			Vec2(0.f, 0.5f),
			TextBoxMode::SHRINK_TO_FIT,
			99999999
		);

		yBottom += lineHeight;
		if (yBottom + lineHeight > bounds.m_maxs.y)
			break;
	}

	//Draw Input Text
	AABB2 inputBounds(
		Vec2(bounds.m_mins.x + 0.2f, bounds.m_mins.y),
		Vec2(bounds.m_maxs.x - 0.2f, bounds.m_mins.y + lineHeight)
	);

	std::string prefix = "> ";
	std::string inputLine = prefix + m_inputText;

	font.AddVertsForTextInBox2D(
		textVerts,
		inputLine,
		inputBounds,
		lineHeight,
		Rgba8(0, 255, 255, 255),
		fontAspect,
		Vec2(0.f, 0.5f),
		TextBoxMode::SHRINK_TO_FIT,
		99999999
	);

	// Draw caret as the '|' character
	if (m_insertionPointVisible)
	{
		int ins = m_insertionPointPosition;
		if (ins < 0) ins = 0;
		int maxIns = (int)m_inputText.size();
		if (ins > maxIns) ins = maxIns;

		float glyphW = lineHeight * fontAspect;

		int prefixCount = 2;

		Vec2 caretMins;
		caretMins.x = inputBounds.m_mins.x + glyphW * (float)(prefixCount + ins);
		caretMins.y = inputBounds.m_mins.y;

		font.AddVertsForText2D(
			textVerts,
			caretMins,
			lineHeight,
			"|",
			Rgba8(255, 255, 255, 255),
			fontAspect
		);
	}

	g_engine->m_render->BindTexture(nullptr);
	g_engine->m_render->DrawVertexArray((int)boxVerts.size(), boxVerts.data());

	g_engine->m_render->BindTexture(&font.GetTexture());
	g_engine->m_render->DrawVertexArray((int)textVerts.size(), textVerts.data());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//SD2 Additions (Reorganize Later)
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ToggleOpen()
{
	m_isOpen = !m_isOpen;

	if (m_isOpen)
	{
		m_mode = DevConsoleMode::OPEN_FULL;
		m_insertionPointVisible = true;

		if (m_insertionPointBlinkTimer)
			m_insertionPointBlinkTimer->Start();
	}
	else
	{
		m_mode = DevConsoleMode::HIDDEN;
	}
}

bool DevConsole::IsOpen()
{
	return m_isOpen;
}

bool DevConsole::Event_KeyPressed(EventArgs& args)
{
	DevConsole* dc = g_engine ? g_engine->m_devConsole : nullptr;
	if (!dc)
		return false;

	if (dc->m_config.m_disableInput)
		return false;

	int keyCode = args.GetValue("keyCode", -1);

#ifndef VK_OEM_3
#define VK_OEM_3 0xC0
#endif
	if (keyCode == VK_OEM_3)
	{
		dc->ToggleOpen();
		return true;
	}

	if (!dc->IsOpen())
		return false;

	if (keyCode == KEYCODE_ESC)
	{
		dc->ToggleOpen();
		return true;
	}

	if (keyCode == KEYCODE_ENTER)
	{
		if (!dc->m_inputText.empty())
		{
			// push into history
			dc->m_commandHistory.push_back(dc->m_inputText);
			if ((int)dc->m_commandHistory.size() > dc->m_config.m_maxCommandHistory)
				dc->m_commandHistory.erase(dc->m_commandHistory.begin());

			dc->m_historyIndex = (int)dc->m_commandHistory.size();

			dc->Execute(dc->m_inputText);
			dc->m_inputText.clear();
			dc->m_insertionPointPosition = 0;
		}
		return true;
	}

	if (keyCode == KEYCODE_BACKSPACE)
	{
		if (dc->m_insertionPointPosition > 0 && !dc->m_inputText.empty())
		{
			dc->m_inputText.erase(dc->m_inputText.begin() + (dc->m_insertionPointPosition - 1));
			dc->m_insertionPointPosition--;
		}
		return true;
	}

	if (keyCode == KEYCODE_DELETE)
	{
		if (dc->m_insertionPointPosition < (int)dc->m_inputText.size())
		{
			dc->m_inputText.erase(dc->m_inputText.begin() + dc->m_insertionPointPosition);
		}
		return true;
	}

	if (keyCode == KEYCODE_HOME)
	{
		dc->m_insertionPointPosition = 0;
		return true;
	}

	if (keyCode == KEYCODE_END)
	{
		dc->m_insertionPointPosition = (int)dc->m_inputText.size();
		return true;
	}

	if (keyCode == KEYCODE_LEFT)
	{
		if (dc->m_insertionPointPosition > 0)
			dc->m_insertionPointPosition--;
		return true;
	}

	if (keyCode == KEYCODE_RIGHT)
	{
		if (dc->m_insertionPointPosition < (int)dc->m_inputText.size())
			dc->m_insertionPointPosition++;
		return true;
	}

	if (keyCode == KEYCODE_UP)
	{
		if (!dc->m_commandHistory.empty() && dc->m_historyIndex > 0)
		{
			dc->m_historyIndex--;
			dc->m_inputText = dc->m_commandHistory[dc->m_historyIndex];
			dc->m_insertionPointPosition = (int)dc->m_inputText.size();
		}
		return true;
	}

	if (keyCode == KEYCODE_DOWN)
	{
		if (!dc->m_commandHistory.empty() && dc->m_historyIndex < (int)dc->m_commandHistory.size())
		{
			dc->m_historyIndex++;

			if (dc->m_historyIndex == (int)dc->m_commandHistory.size())
			{
				dc->m_inputText.clear();
				dc->m_insertionPointPosition = 0;
			}
			else
			{
				dc->m_inputText = dc->m_commandHistory[dc->m_historyIndex];
				dc->m_insertionPointPosition = (int)dc->m_inputText.size();
			}
		}
		return true;
	}

	return true; // console is open: consume key presses by default
}

bool DevConsole::Event_CharInput(EventArgs& args)
{
	DevConsole* dc = g_engine ? g_engine->m_devConsole : nullptr;
	if (!dc) return false;
	if (dc->m_config.m_disableInput) return false;
	if (!dc->IsOpen()) return false;

	unsigned long long code = args.GetValue("char", 0ull);
	if (code == 0ULL) return true;

	unsigned int c = (unsigned int)code;

	if (c < 32 || c == 127) return true;
	if (c == '`' || c == '~') return true;

	if (dc->m_insertionPointPosition < 0) dc->m_insertionPointPosition = 0;
	if (dc->m_insertionPointPosition > (int)dc->m_inputText.size())
		dc->m_insertionPointPosition = (int)dc->m_inputText.size();

	dc->m_inputText.insert(dc->m_inputText.begin() + dc->m_insertionPointPosition, (char)c);
	dc->m_insertionPointPosition++;

	return true;
}

bool DevConsole::Command_Clear(EventArgs& args)
{
	(void)args;

	DevConsole* dc = g_engine ? g_engine->m_devConsole : nullptr;
	if (!dc)
		return false;

	dc->m_lines.clear();
	dc->AddLine(COLOR_INFO_MINOR, "Console cleared.");
	return false;
}

bool DevConsole::Command_Echo(EventArgs& args)
{
	DevConsole* dc = g_engine ? g_engine->m_devConsole : nullptr;
	if (!dc)
		return false;

	std::string out;
	for (int i = 1; ; ++i)
	{
		std::string key = Stringf("arg%d", i);
		std::string part = args.GetValue(key, std::string(""));

		if (part.empty())
			break;

		if (!out.empty())
			out += " ";
		out += part;
	}

	// If user typed just "echo" with no args
	if (out.empty())
	{
		dc->AddLine(COLOR_INFO_MINOR, "(echo)");
	}
	else
	{
		dc->AddLine(COLOR_INFO_MAJOR, out);
	}

	return true;
}

bool DevConsole::Command_Help(EventArgs& args)
{
	(void)args;

	DevConsole* dc = g_engine ? g_engine->m_devConsole : nullptr;
	if (!dc)
		return false;

	dc->AddLine(COLOR_INFO_MAJOR, "Commands:");
	dc->AddLine(COLOR_INFO_MINOR, "  clear  - clears the console output");
	dc->AddLine(COLOR_INFO_MINOR, "  echo X - prints X back to the console");
	dc->AddLine(COLOR_INFO_MINOR, "  help   - shows this help text");

	return true;
}
