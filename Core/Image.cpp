#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "ThirdParty/stb/stb_image.h"

Image::Image()
{
}

Image::~Image()
{
}

Image::Image(IntVec2 size, Rgba8 color)
{
	m_dimensions = size;
	m_imageFilePath = "";

	size_t pixelCount = (size_t)size.x * (size_t)size.y;
	m_texelArray.resize(pixelCount, color);
}

Image::Image(char const* imageFilePath)
{
	stbi_set_flip_vertically_on_load(1);
	int numComponents = 0;
	unsigned char* imageDataBytes = stbi_load(imageFilePath, &m_dimensions.x, &m_dimensions.y, &numComponents, 0);


	GUARANTEE_OR_DIE( imageDataBytes != nullptr, Stringf("Failed to load image %s", imageFilePath));
	GUARANTEE_OR_DIE(numComponents == 3 || numComponents == 4, 
	Stringf("Can't load image from file %s; has %i components; we only support 3 or 4",
		imageFilePath, numComponents));

	int numTexels = m_dimensions.x * m_dimensions.y;
	m_texelArray.resize( numTexels );

	for (int texelIndex = 0; texelIndex < numTexels; ++texelIndex)
	{
		Rgba8& texel = m_texelArray[texelIndex];
		texel.r = imageDataBytes[(texelIndex * numComponents) + 0];
		texel.g = imageDataBytes[(texelIndex * numComponents) + 1];
		texel.b = imageDataBytes[(texelIndex * numComponents) + 2];
/*		texel.a = imageDataBytes[(texelIndex * numComponents) + 3];*/

		if (numComponents == 3)
		{
			texel.a = 255;
		}
		else
		{
			texel.a = imageDataBytes[(texelIndex * numComponents) + 3];
		}
	}

	stbi_image_free( imageDataBytes);

	m_imageFilePath = imageFilePath;
}

Rgba8 Image::GetTexelColor(int x, int y) const
{
	int index = y * m_dimensions.x + x;
	GUARANTEE_OR_DIE(index >= 0 && index < m_texelArray.size(),
		"GetTexelColor - OUT OF BOUNDS");
	return m_texelArray[index];
}


//-----------------------------------------------------------------------------------------------
void Image::SetTexelColor(int x, int y, Rgba8 const& newColor)
{
	int index = y * m_dimensions.x + x;
	GUARANTEE_OR_DIE(index >= 0 && index < m_texelArray.size(),
		"SetTexelColor - OUT OF BOUNDS");
	m_texelArray[index] = newColor;
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

std::string const& Image::GetImageFilePath() const
{
	return m_imageFilePath; 
}

void const* Image::GetRawData() const
{
	if (m_texelArray.empty())
	{
		return nullptr;
	}
	return (void const*)m_texelArray.data();
}
