#include "videowall-preview.hpp"

#include <QPainter>
#include <QPen>

VideoWallPreview::VideoWallPreview(QWidget *parent) : QWidget(parent)
{
	setMinimumSize(400, 300);
}

void VideoWallPreview::setGrid(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;

	m_layout.generate(rows, cols, m_canvasWidth, m_canvasHeight);

	update();
}

void VideoWallPreview::setCanvasSize(float width, float height)
{
	m_canvasWidth = width;
	m_canvasHeight = height;

	m_layout.generate(m_rows, m_cols, m_canvasWidth, m_canvasHeight);

	update();
}

int VideoWallPreview::selectedRegion() const
{
	return m_selectedRegion;
}

const WallLayout &VideoWallPreview::layout() const
{
	return m_layout;
}

void VideoWallPreview::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);

	painter.fillRect(rect(), QColor(20, 20, 20));

	float scaleX = width() / m_canvasWidth;

	float scaleY = height() / m_canvasHeight;

	float scale = std::min(scaleX, scaleY);

	float offsetX = (width() - (m_canvasWidth * scale)) / 2.0f;

	float offsetY = (height() - (m_canvasHeight * scale)) / 2.0f;

	for (const auto &region : m_layout.regions()) {

		QRectF rect(offsetX + (region.x * scale), offsetY + (region.y * scale), region.width * scale,
			    region.height * scale);

		if (region.index == m_selectedRegion) {

			painter.fillRect(rect, QColor(80, 120, 255, 80));
		}

		QPen pen(QColor(180, 180, 180));
		pen.setWidth(2);

		painter.setPen(pen);

		painter.drawRect(rect);
	}
}

void VideoWallPreview::mousePressEvent(QMouseEvent *event)
{
	float scaleX = width() / m_canvasWidth;

	float scaleY = height() / m_canvasHeight;

	float scale = std::min(scaleX, scaleY);

	float offsetX = (width() - (m_canvasWidth * scale)) / 2.0f;

	float offsetY = (height() - (m_canvasHeight * scale)) / 2.0f;

	QPointF pos = event->position();

	for (const auto &region : m_layout.regions()) {

		QRectF rect(offsetX + (region.x * scale), offsetY + (region.y * scale), region.width * scale,
			    region.height * scale);

		if (rect.contains(pos)) {

			m_selectedRegion = region.index;

			update();

			break;
		}
	}
}