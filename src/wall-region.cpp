#include "wall-region.hpp"

void WallLayout::generate(int rows, int cols, float canvasWidth, float canvasHeight)
{
	m_regions.clear();

	if (rows <= 0 || cols <= 0)
		return;

	float cellWidth = canvasWidth / cols;
	float cellHeight = canvasHeight / rows;

	int index = 1;

	for (int y = 0; y < rows; y++) {

		for (int x = 0; x < cols; x++) {

			WallRegion region;

			region.index = index++;

			region.x = x * cellWidth;
			region.y = y * cellHeight;

			region.width = cellWidth;
			region.height = cellHeight;

			m_regions.push_back(region);
		}
	}
}

const std::vector<WallRegion> &WallLayout::regions() const
{
	return m_regions;
}

const WallRegion *WallLayout::regionByIndex(int index) const
{
	for (const auto &region : m_regions) {

		if (region.index == index)
			return &region;
	}

	return nullptr;
}