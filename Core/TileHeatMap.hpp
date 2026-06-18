#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>

struct Vertex;

class TileHeatMap
{
public:
	TileHeatMap(IntVec2 const& dimensions);
	~TileHeatMap() {
		delete[] m_values;
		m_values = nullptr;
	}

	void SetAllValues(float heat);
	float GetTileHeatValue(IntVec2 const& tileCoord);
	void SetTileHeatValue(IntVec2 const& tileCoord, float heatValue);
	void AddHeatValueToTile(IntVec2 const& tileCoord, float heatValue);

	void AddVertsForDebugDraw(
		std::vector<Vertex>& verts,
		AABB2 totalBounds,
		FloatRange valueRange = FloatRange(0.f, 1.f),
		Rgba8 lowColor = Rgba8(0, 0, 0, 100),
		Rgba8 highColor = Rgba8(255, 255, 255, 100),
		float specialValue = 999999.f,
		Rgba8 specialColor = Rgba8(255, 0, 255));

public:
	IntVec2 m_dimensions;
	float* m_values = nullptr;
	int numTiles;

};