#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <vector>

class Image
{
	friend class Renderer;

public:
	Image();
	~Image();
	Image( char const* imageFilePath);
	Image( IntVec2 size, Rgba8 color);

	Rgba8	GetTexelColor(int x, int y) const;
	void	SetTexelColor(int x, int y, Rgba8 const& newColor);

	IntVec2	GetDimensions() const;
	std::string const& GetImageFilePath() const;
	const void* GetRawData() const;
	
protected:
	IntVec2				m_dimensions;
	std::vector<Rgba8>	m_texelArray;
	std::string			m_imageFilePath;
};