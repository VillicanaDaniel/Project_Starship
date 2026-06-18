#pragma once
#include <string>
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/StringUtils.hpp"

enum TextBoxMode
{
	SHRINK_TO_FIT,
	OVERRUN,
	WORD_WRAP
};

class BitmapFont
{
	friend class Renderer; // Only the Renderer can create new BitmapFont objects!

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture);

public:
	Texture& GetTexture();

	void AddVertsForText2D(std::vector<Vertex>& vertexArray, Vec2 textMins,
		float cellHeight, std::string const& text, Rgba8 tint = Rgba8(255,255,255), float cellAspectScale = 1.f);

	float GetTextWidth(float cellHeight, std::string const& text, float cellAspectScale = 1.f);

	void AddVertsForTextInBox2D(std::vector<Vertex>& verts, std::string const& text, AABB2 const& box, float cellHeight, Rgba8 tint = Rgba8(255,255,255),
		float cellAspectScale = 1.f, Vec2 alignment = Vec2(.5f, .5f), TextBoxMode mode = TextBoxMode::SHRINK_TO_FIT, int maxGlyphsToDraw = 99999999);
	void AddVertsForText3DAtOriginXForward(std::vector<Vertex>& verts, float cellHeight,
		std::string const& text, Rgba8 const& tint = Rgba8::WHITE,
		float cellAspect = 1.0f, Vec2 const& alignment = Vec2(0.5f, 0.5f), int maxGlyphsToDraw = 999);

	Strings WrapText(std::string const& text, float maxWidth, float cellHeight, float cellAspectScale) const;


protected:
	float GetGlyphAspect(int glyphUnicode) const; // For now this will always return m_fontDefaultAspect

protected:
	std::string	m_fontFilePathNameWithNoExtension;
	SpriteSheet	m_fontGlyphsSpriteSheet;
	float		m_fontDefaultAspect = 1.0f; // For basic (tier 1) fonts, set this to the aspect of the sprite sheet texture
};
