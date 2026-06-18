#pragma once 

struct IntVec2
{
	public:
		int x = 0;
		int y = 0;

	public:
		~IntVec2() {}
		IntVec2() {}
		IntVec2( const IntVec2& copyFrom );
		explicit IntVec2( int initialX, int initialY );
		void SetFromText(char const* text);

		static const IntVec2 ZERO;
		float GetLength() const;
		int GetTaxicabLength() const;
		int GetLengthSquared() const;
		float GetOrientationRadians() const;
		float GetOrientationDegrees() const;
		IntVec2 const GetRotatedBy90Degrees() const;
		IntVec2 const GetRotatedByMinus90Degrees() const;

		void Rotate90Degrees();
		void RotateMinus90Degrees();

		void operator =( const IntVec2& copyFrom );
		IntVec2 const operator+(IntVec2 const& vecToAdd) const;
		IntVec2 const operator-(IntVec2 const& vecToSubtract) const;


};