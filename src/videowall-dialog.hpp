#pragma once

#include <QDialog>
#include <QTreeWidget>

#include "videowall-preview.hpp"

class QSpinBox;
class QPushButton;

class VideoWallDialog : public QDialog {
	Q_OBJECT

public:
	explicit VideoWallDialog(QWidget *parent = nullptr);
	void refreshSceneTree();
	void loadSettings();
	void saveSettings();


private:
	QSpinBox *rowsSpin;
	QSpinBox *colsSpin;

	QPushButton *applyButton;
	QPushButton *fitWallButton;

	QTreeWidget *sceneTree;
	QString selectedSceneName;
	QString selectedSourceName;

	VideoWallPreview *preview;
};