#pragma once 

struct Rgba8
{
public:
	static const Rgba8 WHITE;
	static const Rgba8 BLACK;
	static const Rgba8 RED;
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

public:
	// Construction/Destruction
	~Rgba8() {}												// destructor (do nothing)
	Rgba8() {}												// default constructor (do nothing)
	Rgba8(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255); // unsigned char
	void SetFromText(char const* text);
	void GetAsFloats(float* colorAsFloats) const;

};
// 	float NormalizeByte(unsigned char value);
// 	unsigned char DenormalizeByte(float normalizedValue);

	Rgba8 Interpolate(Rgba8 start, Rgba8 end, float fractionOfEnd);