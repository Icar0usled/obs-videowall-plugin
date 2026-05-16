#pragma once

#include <vector>

struct WallRegion {
	int index;

	float x;
	float y;

	float width;
	float height;
};

class WallLayout {
public:
	void generate(int rows, int cols, float canvasWidth, float canvasHeight);

	const std::vector<WallRegion> &regions() const;

	const WallRegion *regionByIndex(int index) const;

private:
	std::vector<WallRegion> m_regions;
};