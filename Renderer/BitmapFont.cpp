#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"

BitmapFont::BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture)
	: m_fontFilePathNameWithNoExtension(fontFilePathNameWithNoExtension)
	, m_fontGlyphsSpriteSheet(fontTexture, IntVec2(16, 16))
{
	m_fontDefaultAspect = 1.f;
}

Texture& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}

float BitmapFont::GetGlyphAspect([[maybe_unused]]int glyphUnicode) const
{
	return m_fontDefaultAspect;
}

float BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspectScale)
{
	float cellWidth = cellHeight * GetGlyphAspect(0) * cellAspectScale;
	return cellWidth * (float)text.size();
}

void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex>& verts, std::string const& text, AABB2 const& box,
	float cellHeight, Rgba8 tint, float cellAspectScale, Vec2 alignment, TextBoxMode mode, int maxGlyphsToDraw)
{
	Strings splitStrings = SplitStringOnDelimiter(text, '\n');
	Vec2 boxDimensions = box.GetDimensions();

	float textWidth = 0.f;
	for (int stringIndex = 0; stringIndex < static_cast<int>(splitStrings.size()); ++stringIndex)
	{
		float lineWidth = GetTextWidth(cellHeight, splitStrings[stringIndex], cellAspectScale);
		if (lineWidth > textWidth)
		{
			textWidth = lineWidth;
		}
	}

	float paragraphHeight = cellHeight * static_cast<float>(splitStrings.size());

	if (mode == SHRINK_TO_FIT)
	{
		if (textWidth > boxDimensions.x)
		{
			float scale = boxDimensions.x / textWidth;
			cellHeight *= scale;
			textWidth *= scale;
		}

		if (paragraphHeight > boxDimensions.y)
		{
			float scale = boxDimensions.y / paragraphHeight;
			cellHeight *= scale;
			textWidth *= scale;
			paragraphHeight = boxDimensions.y;
		}
	}

	if (mode == WORD_WRAP)
	{
		Strings wrappedLines;

		float cellWidth = cellHeight * GetGlyphAspect(0) * cellAspectScale;
		int maxCharsPerLine = (int)(boxDimensions.x / cellWidth);

		for (std::string const& sourceLine : splitStrings)
		{
			std::string currentLine;
			std::string currentWord;

			for (int i = 0; i <= (int)sourceLine.size(); ++i)
			{
				char c = i < (int)sourceLine.size() ? sourceLine[i] : ' ';

				if (c == ' ' || i == (int)sourceLine.size())
				{
					if (!currentWord.empty())
					{
						int spaceNeeded = currentLine.empty() ? 0 : 1;
						int proposedLength = (int)currentLine.size() + spaceNeeded + (int)currentWord.size();

						if (proposedLength > maxCharsPerLine && !currentLine.empty())
						{
							wrappedLines.push_back(currentLine);
							currentLine.clear();
						}

						if (!currentLine.empty())
						{
							currentLine += ' ';
						}

						currentLine += currentWord;
						currentWord.clear();
					}
				}
				else
				{
					currentWord += c;
				}
			}

			if (!currentLine.empty())
			{
				wrappedLines.push_back(currentLine);
			}
		}

		splitStrings = wrappedLines;

		int glyphsDrawn = 0;

		for (int stringIndex = 0; stringIndex < (int)splitStrings.size(); ++stringIndex)
		{
			std::string const& line = splitStrings[stringIndex];

			int glyphsRemaining = maxGlyphsToDraw - glyphsDrawn;
			if (glyphsRemaining <= 0)
			{
				break;
			}

			std::string visibleLine = line;
			if ((int)visibleLine.size() > glyphsRemaining)
			{
				visibleLine = visibleLine.substr(0, glyphsRemaining);
			}

			Vec2 linePos;
			linePos.x = box.m_mins.x;
			linePos.y = box.m_maxs.y - cellHeight - ((float)stringIndex * cellHeight);

			AddVertsForText2D(
				verts,
				linePos,
				cellHeight,
				visibleLine,
				tint,
				cellAspectScale
			);

			glyphsDrawn += (int)visibleLine.size();
		}

		return;
	}

	Vec2 offset;
	offset.x = (boxDimensions.x - textWidth) * alignment.x;
	offset.y = (boxDimensions.y - paragraphHeight) * alignment.y;
	Vec2 startPos = box.m_mins + offset;

	int glyphsDrawn = 0;
	int splitStringSize = static_cast<int>(splitStrings.size()) - 1;
	for (int stringIndex = 0; stringIndex <= splitStringSize; ++stringIndex)
	{
		Vec2 linePos = startPos;
		linePos.y += cellHeight * (splitStringSize - stringIndex);

		if (glyphsDrawn <= maxGlyphsToDraw)
		{
			int lengthNextString = static_cast<int>(splitStrings[stringIndex].length());
			if (lengthNextString + glyphsDrawn < maxGlyphsToDraw)
			{
				float lineWidth = GetTextWidth(cellHeight, splitStrings[stringIndex], cellAspectScale);
				linePos.x = box.m_mins.x + (boxDimensions.x - lineWidth) * alignment.x;
				AddVertsForText2D(verts, linePos, cellHeight, splitStrings[stringIndex], tint, cellAspectScale);
				glyphsDrawn += static_cast<int>(splitStrings[stringIndex].length());
			}
			else
			{
				break;
			}
		}
	}
}

void BitmapFont::AddVertsForText2D(
	std::vector<Vertex>& verts,
	Vec2 textMins,
	float cellHeight,
	std::string const& text,
	Rgba8 tint,
	float cellAspectScale)
{
	float cellWidth = cellHeight * GetGlyphAspect(0) * cellAspectScale;
	Vec2 cursor = textMins;

	for (char c : text)
	{
		int glyphIndex = (int)c;
		AABB2 glyphBounds(cursor, cursor + Vec2(cellWidth, cellHeight));

		SpriteDef spriteDef = m_fontGlyphsSpriteSheet.GetSpriteDef(glyphIndex);
		Vec2 uvMins, uvMaxs;
		spriteDef.GetUVs(uvMins, uvMaxs);

		AddVertsForAABB2(verts, glyphBounds, tint, uvMins, uvMaxs);

		cursor.x += cellWidth;
	}
}

void BitmapFont::AddVertsForText3DAtOriginXForward(std::vector<Vertex>& verts, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.0f*/, Vec2 const& alignment /*= Vec2(0.5f, 0.5f)*/, [[maybe_unused]]int maxGlyphsToDraw /*= 999*/)
{
	std::vector<Vertex> localVerts;

	AddVertsForText2D(localVerts, Vec2(0.f, 0.f), cellHeight, text, tint, cellAspect);

	if (localVerts.empty())
	{
		return;
	}

	AABB2 bounds = GetVertexBounds2D(localVerts);
	Vec2 dims = bounds.GetDimensions();

	Vec2 offset;
	offset.x = -bounds.m_mins.x - alignment.x * dims.x;
	offset.y = -bounds.m_mins.y - alignment.y * dims.y;

	for (Vertex& vertex : localVerts)
	{
		vertex.m_position.x += offset.x;
		vertex.m_position.y += offset.y;
	}

	for (Vertex& vertex : localVerts)
	{
		float oldX = vertex.m_position.x;
		float oldY = vertex.m_position.y;

		vertex.m_position = Vec3(0.f, -oldX, oldY);
	}

	verts.insert(verts.end(), localVerts.begin(), localVerts.end());
}

Strings BitmapFont::WrapText(std::string const& text, float maxWidth, float cellHeight, float cellAspectScale) const
{
	Strings wrappedLines;

	float cellWidth = cellHeight * GetGlyphAspect(0) * cellAspectScale;
	int maxCharsPerLine = (int)(maxWidth / cellWidth);

	if (maxCharsPerLine <= 0)
	{
		wrappedLines.push_back(text);
		return wrappedLines;
	}

	std::string currentLine;
	std::string currentWord;

	for (int i = 0; i <= (int)text.size(); ++i)
	{
		char c = i < (int)text.size() ? text[i] : ' ';

		if (c == '\n')
		{
			if (!currentWord.empty())
			{
				if ((int)(currentLine.size() + currentWord.size()) > maxCharsPerLine)
				{
					wrappedLines.push_back(currentLine);
					currentLine.clear();
				}

				currentLine += currentWord;
				currentWord.clear();
			}

			wrappedLines.push_back(currentLine);
			currentLine.clear();
			continue;
		}

		if (c == ' ' || i == (int)text.size())
		{
			if (!currentWord.empty())
			{
				int extraSpace = currentLine.empty() ? 0 : 1;
				int proposedLength = (int)currentLine.size() + extraSpace + (int)currentWord.size();

				if (proposedLength > maxCharsPerLine && !currentLine.empty())
				{
					wrappedLines.push_back(currentLine);
					currentLine.clear();
				}

				if (!currentLine.empty())
				{
					currentLine += ' ';
				}

				currentLine += currentWord;
				currentWord.clear();
			}
		}
		else
		{
			currentWord += c;
		}
	}

	if (!currentLine.empty())
	{
		wrappedLines.push_back(currentLine);
	}

	return wrappedLines;
}
