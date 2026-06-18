#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"

SpriteAnimDefinition::SpriteAnimDefinition(SpriteSheet const& sheet,
	int startSpriteIndex,
	int endSpriteIndex,
	float framesPerSecond,
	SpriteAnimPlaybackType playbackType)
	: m_spriteSheet(sheet)
	, m_startSpriteIndex(startSpriteIndex)
	, m_endSpriteIndex(endSpriteIndex)
	, m_framesPerSecond(framesPerSecond)
	, m_playbackType(playbackType)
{
}

SpriteDef const& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{
	int frameCount = (m_endSpriteIndex - m_startSpriteIndex + 1);

	float frameF = seconds * m_framesPerSecond;

	int frameIndex = 0;

	switch (m_playbackType)
	{
		case SpriteAnimPlaybackType::ONCE:
		{
			int finalFrame = frameCount - 1;
			if (frameF < 0.f)
				frameIndex = 0;
			else if (frameF >= (float)finalFrame)
				frameIndex = finalFrame;
			else
				frameIndex = (int)frameF;
			break;
		}

		case SpriteAnimPlaybackType::LOOP:
		{
			frameIndex = (int)fmodf(frameF, (float)frameCount);
			if (frameIndex < 0)
				frameIndex += frameCount;
			break;
		}

		case SpriteAnimPlaybackType::PINGPONG:
		{
			int forward = frameCount;
			int backward = frameCount - 2;
			int cycleLength = forward + backward;

			float timeInCycle = fmodf(frameF, (float)cycleLength);
			if (timeInCycle < 0.f)
				timeInCycle += (float)cycleLength;

			int rawIndex = (int)timeInCycle;

			if (rawIndex < forward)
			{
				frameIndex = rawIndex;
			}
			else
			{
				frameIndex = cycleLength - rawIndex;
			}

			break;
		}
	}

	int spriteIndex = m_startSpriteIndex + frameIndex;
	return m_spriteSheet.GetSpriteDef(spriteIndex);
}


