#pragma once
#include "Engine/Renderer/Texture.hpp"

enum class SpriteAnimPlaybackType
{
	ONCE,
	LOOP,
	PINGPONG,
};

class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition(SpriteSheet const& sheet,
		int startSpriteIndex,
		int endSpriteIndex,
		float framesPerSecond,
		SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP);

	SpriteDef const& GetSpriteDefAtTime(float seconds) const;
	SpriteAnimPlaybackType GetPlaybackTypeFromString(std::string const& text);

private:
	SpriteSheet const& m_spriteSheet;
	int                       m_startSpriteIndex = -1;
	int                       m_endSpriteIndex = -1;
	float                     m_framesPerSecond = 1.f;
	SpriteAnimPlaybackType    m_playbackType = SpriteAnimPlaybackType::LOOP;
};