#pragma once

#include <QWidget>
#include <QMouseEvent>
#include "wall-region.hpp"


class VideoWallPreview : public QWidget {
	Q_OBJECT

public:
	explicit VideoWallPreview(QWidget *parent = nullptr);

	void setGrid(int rows, int cols);
	void setCanvasSize(float width, float height);

	int selectedRegion() const;
	const WallLayout &layout() const;

protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

private:
	int m_rows = 2;
	int m_cols = 2;

	float m_canvasWidth = 1920.0f;
	float m_canvasHeight = 1080.0f;

	int m_selectedRegion = -1;
	WallLayout m_layout;
};