#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/MathUtils.hpp"

const Rgba8 Rgba8::WHITE = Rgba8(255, 255, 255, 255);
const Rgba8 Rgba8::BLACK = Rgba8(0, 0, 0, 255);
const Rgba8 Rgba8::RED = Rgba8(255, 0, 0, 255);

Rgba8::Rgba8( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte )
	:r ( redByte )
	,g ( greenByte )
	,b ( blueByte )
	,a ( alphaByte )
{
}

void Rgba8::SetFromText(char const* text)
{
	Strings parts = SplitStringOnDelimiter(text, ',');

	if (parts.size() != 3 && parts.size() != 4)
	{
		ERROR_AND_DIE("Rgba8::SetFromText() - Expected 2 or 3 commas (r,g,b or r,g,b,a)");
	}

	r = static_cast<unsigned char>(atoi(parts[0].c_str()));
	g = static_cast<unsigned char>(atoi(parts[1].c_str()));
	b = static_cast<unsigned char>(atoi(parts[2].c_str()));

	if (parts.size() == 4)
		a = static_cast<unsigned char>(atoi(parts[3].c_str()));
	else
		a = 255;
}

// float NormalizeByte(unsigned char byteValue)
// {
// 	return (float)byteValue / 255.0f;
// }
// 
// unsigned char DenormalizeByte(float normalizedValue)
// {
// 	normalizedValue = GetClamped(normalizedValue, 0.0f, 1.0f);
// 	return (unsigned char)(normalizedValue * 255.0f);
// }

Rgba8 Interpolate(Rgba8 start, Rgba8 end, float fractionOfEnd)
{
	float r = Interpolate(NormalizeByte(start.r), NormalizeByte(end.r), fractionOfEnd);
	float g = Interpolate(NormalizeByte(start.g), NormalizeByte(end.g), fractionOfEnd);
	float b = Interpolate(NormalizeByte(start.b), NormalizeByte(end.b), fractionOfEnd);
	float a = Interpolate(NormalizeByte(start.a), NormalizeByte(end.a), fractionOfEnd);

	return Rgba8(
		DenormalizeByte(r),
		DenormalizeByte(g),
		DenormalizeByte(b),
		DenormalizeByte(a)
	);
}

void Rgba8::GetAsFloats(float* colorAsFloats) const
{
	colorAsFloats[0] = RangeMapClamped((float)r, 0.f, 255.f, 0.f, 1.f);
	colorAsFloats[1] = RangeMapClamped((float)g, 0.f, 255.f, 0.f, 1.f);
	colorAsFloats[2] = RangeMapClamped((float)b, 0.f, 255.f, 0.f, 1.f);
	colorAsFloats[3] = RangeMapClamped((float)a, 0.f, 255.f, 0.f, 1.f);
}
