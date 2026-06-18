#pragma once
#include <string>
#include <atomic>
#include <mutex>
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/AABB2.hpp"

class BitmapFont;
class Timer;
class Renderer;
class EventSystem;
class NamedStrings;
typedef NamedStrings EventArgs;

struct DevConsoleConfig
{
	bool m_isEnabled = true;
	Renderer* m_renderer = nullptr;
	EventSystem* m_eventSystem = nullptr;
	std::string m_fontName = "Data/Fonts/SquirrelFixedFont";
	float m_fontAspect = 0.7f;
	int m_linesOnScreen = 40;
	int m_maxCommandHistory = 128;
	bool m_startOpen = false;
	bool m_disableInput = false;
};

struct DevConsoleLine
{
	Rgba8 color;
	std::string text;
};

enum class DevConsoleMode
{
	HIDDEN,
	OPEN_FULL,
};

//------------------------------------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole(DevConsoleConfig const& config);
	~DevConsole();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Execute(std::string const& consoleCommandText);
	void AddLine(Rgba8 const& color, std::string const& text);

	void Render(AABB2 const& bounds) const;

	DevConsoleMode GetMode() const;
	void SetMode(DevConsoleMode mode);
	void ToggleOpen();

public:
	bool IsOpen();

	static const Rgba8 COLOR_ERROR;
	static const Rgba8 COLOR_WARNING;
	static const Rgba8 COLOR_INFO_MAJOR;
	static const Rgba8 COLOR_INFO_MINOR;
	static const Rgba8 COLOR_COMMAND_ECHO;
	static const Rgba8 COLOR_INPUT_TEXT;
	static const Rgba8 COLOR_INPUT_INSERTION_POINT;

	BitmapFont* m_font = nullptr;

	static bool Event_KeyPressed(EventArgs& args);
	static bool Event_CharInput(EventArgs& args);
	static bool Command_Clear(EventArgs& args);
	static bool Command_Echo(EventArgs& args);
	static bool Command_Help(EventArgs& args);

protected:
	void Render_OpenFull(AABB2 const& bounds, BitmapFont& font, float fontAspect = 1.f) const;

protected:
	DevConsoleConfig const	m_config;
	
	std::mutex m_mutex;
	std::atomic<bool> m_isOpen = false;
	std::string m_inputText;
	int m_insertionPointPosition = 0;
	bool m_insertionPointVisible = true;
	Timer* m_insertionPointBlinkTimer;
	std::vector<std::string> m_commandHistory;
	int m_historyIndex = -1;

	DevConsoleMode			m_mode = DevConsoleMode::HIDDEN; // also OPEN_FULL, and eventually others
	std::vector<DevConsoleLine> m_lines; // TODO: support a max limited # of lines (e.g. fixed circular buffer)
	int						m_frameNumber = 0;
};