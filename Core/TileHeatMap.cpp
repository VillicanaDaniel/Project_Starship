#include "Engine/Core/TileHeatMap.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"


TileHeatMap::TileHeatMap(IntVec2 const& dimensions)
	:m_dimensions(dimensions)
{
	numTiles = dimensions.x * dimensions.y;
	m_values = new float[numTiles]; 
}

void TileHeatMap::SetAllValues(float heat)
{
	for (int i = 0; i < numTiles; ++i)
	{
		m_values[i] = heat;
	}
}

float TileHeatMap::GetTileHeatValue(IntVec2 const& tileCoord)
{
	int tileIndex = (tileCoord.y * m_dimensions.x) + tileCoord.x;
	return m_values[tileIndex];
}

void TileHeatMap::SetTileHeatValue(IntVec2 const& tileCoord, float heatValue)
{
	int tileIndex = (tileCoord.y * m_dimensions.x) + tileCoord.x;
	m_values[tileIndex] = heatValue;
}

void TileHeatMap::AddHeatValueToTile(IntVec2 const& tileCoord, float heatValue)
{
	int tileIndex = (tileCoord.y * m_dimensions.x ) + tileCoord.x;
	m_values[tileIndex] += heatValue;
}

void TileHeatMap::AddVertsForDebugDraw(std::vector<Vertex>& verts, AABB2 totalBounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 specialColor)
{
	float tileWidth = totalBounds.GetDimensions().x / (float)m_dimensions.x;
	float tileHeight = totalBounds.GetDimensions().y / (float)m_dimensions.y;

	for (int y = 0; y < m_dimensions.y; y++)
	{
		for (int x = 0; x < m_dimensions.x; x++)
		{
			int index = y * m_dimensions.x + x;
			float value = m_values[index];

			// Compute tile AABB
			float minX = totalBounds.m_mins.x + x * tileWidth;
			float minY = totalBounds.m_mins.y + y * tileHeight;
			float maxX = minX + tileWidth;
			float maxY = minY + tileHeight;

			AABB2 tileBounds(Vec2(minX, minY), Vec2(maxX, maxY));

			// Determine color
			Rgba8 tileColor;

			if (value == specialValue)
			{
				tileColor = specialColor;
			}
			else
			{
				float t = RangeMapClamped(
					value,
					valueRange.m_min,
					valueRange.m_max,
					0.f, 1.f
				);

				tileColor = Interpolate( lowColor, highColor, t );
			}

			AddVertsForAABB2(verts, tileBounds, tileColor, Vec2(0.f,0.f), Vec2(1.f,1.f));
		}
	}
}

