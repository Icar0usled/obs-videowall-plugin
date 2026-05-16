#include "videowall-dialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include "videowall-preview.hpp"
#include <obs-frontend-api.h>
#include <obs.h>
#include <util/config-file.h>
#include <QSplitter>
#include <QTreeWidgetItem>

VideoWallDialog::VideoWallDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle("VideoWall Layout");

	rowsSpin = new QSpinBox(this);
	rowsSpin->setMinimum(1);
	rowsSpin->setMaximum(10);
	
	colsSpin = new QSpinBox(this);
	colsSpin->setMinimum(1);
	colsSpin->setMaximum(10);
	

	applyButton = new QPushButton("Apply", this);
	fitWallButton = new QPushButton("Fit Selected Source To Wall", this);

	preview = new VideoWallPreview(this);

	loadSettings();
	sceneTree = new QTreeWidget(this);

	sceneTree->setHeaderLabel("Scenes / Sources");

	sceneTree->setMinimumWidth(250);

	refreshSceneTree();

	connect(sceneTree, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem *item, int column) {
		UNUSED_PARAMETER(column);

		if (!item->parent())
			return;

		selectedSceneName = item->parent()->text(0);

		selectedSourceName = item->text(0);

		blog(LOG_INFO, "[VideoWall] Selected scene: %s source: %s", selectedSceneName.toUtf8().constData(),
		     selectedSourceName.toUtf8().constData());
	});

	obs_video_info ovi;
	obs_get_video_info(&ovi);

	preview->setCanvasSize((float)ovi.base_width, (float)ovi.base_height);

	auto *layout = new QVBoxLayout(this);

	auto *rowsLayout = new QHBoxLayout();
	rowsLayout->addWidget(new QLabel("Rows:"));
	rowsLayout->addWidget(rowsSpin);

	auto *colsLayout = new QHBoxLayout();
	colsLayout->addWidget(new QLabel("Columns:"));
	colsLayout->addWidget(colsSpin);

	layout->addLayout(rowsLayout);
	layout->addLayout(colsLayout);
	layout->addWidget(applyButton);
	connect(applyButton, &QPushButton::clicked, this, [this]() {
		obs_video_info ovi;
		obs_get_video_info(&ovi);

		preview->setCanvasSize((float)ovi.base_width, (float)ovi.base_height);

		preview->setGrid(rowsSpin->value(), colsSpin->value());
		saveSettings();
	});
	layout->addWidget(fitWallButton);
	connect(fitWallButton, &QPushButton::clicked, this, [this]() {
		obs_source_t *currentSceneSource = obs_frontend_get_current_scene();

		if (!currentSceneSource)
			return;

		obs_scene_t *scene = obs_scene_from_source(currentSceneSource);

		if (!scene)
			return;

		obs_sceneitem_t *item = nullptr;

		struct SearchData {
			QString sourceName;
			obs_sceneitem_t *item;
		};

		SearchData data;
		data.sourceName = selectedSourceName;
		data.item = nullptr;

		obs_scene_enum_items(
			scene,
			[](obs_scene_t *scene, obs_sceneitem_t *sceneItem, void *param) {
				UNUSED_PARAMETER(scene);

				SearchData *data = (SearchData *)param;

				obs_source_t *source = obs_sceneitem_get_source(sceneItem);

				if (!source)
					return true;

				const char *name = obs_source_get_name(source);

				if (data->sourceName == QString(name)) {

					data->item = sceneItem;

					return false;
				}

				return true;
			},
			&data);

		item = data.item;

		if (!item) {

			blog(LOG_WARNING, "[VideoWall] No source selected");

			return;
		}

		obs_video_info ovi;
		obs_get_video_info(&ovi);

		vec2 position;

		const WallRegion *region = nullptr;

		int selectedRegion = preview->selectedRegion();

		if (selectedRegion != -1) {

			region = preview->layout().regionByIndex(selectedRegion);
		}

		if (region) {

			position.x = region->x;
			position.y = region->y;

		} else {

			position.x = 0;
			position.y = 0;
		}

		obs_sceneitem_set_pos(item, &position);

		vec2 bounds;

		if (region) {

			bounds.x = region->width;
			bounds.y = region->height;

		} else {

			bounds.x = (float)ovi.base_width;
			bounds.y = (float)ovi.base_height;
		}

		obs_sceneitem_set_bounds_type(item, OBS_BOUNDS_STRETCH);

		obs_sceneitem_set_bounds(item, &bounds);

		vec2 scale;
		scale.x = 1.0f;
		scale.y = 1.0f;

		obs_sceneitem_set_scale(item, &scale);

		obs_sceneitem_force_update_transform(item);

		blog(LOG_INFO, "[VideoWall] Source fitted to wall");
	});
	auto *splitter = new QSplitter(this);

	splitter->addWidget(sceneTree);
	splitter->addWidget(preview);

	splitter->setStretchFactor(1, 1);

	layout->addWidget(splitter);

	setLayout(layout);

	resize(500, 500);
}

void VideoWallDialog::refreshSceneTree()
{
	sceneTree->clear();

	obs_frontend_source_list scenes = {};

	obs_frontend_get_scenes(&scenes);

	for (size_t i = 0; i < scenes.sources.num; i++) {

		obs_source_t *sceneSource = scenes.sources.array[i];

		const char *sceneName = obs_source_get_name(sceneSource);

		QTreeWidgetItem *sceneItem = new QTreeWidgetItem(sceneTree);

		sceneItem->setText(0, sceneName);

		obs_scene_t *scene = obs_scene_from_source(sceneSource);

		if (!scene)
			continue;

		obs_scene_enum_items(
			scene,
			[](obs_scene_t *scene, obs_sceneitem_t *item, void *param) {
				UNUSED_PARAMETER(scene);

				QTreeWidgetItem *sceneTreeItem = (QTreeWidgetItem *)param;

				obs_source_t *source = obs_sceneitem_get_source(item);

				if (!source)
					return true;

				const char *sourceName = obs_source_get_name(source);

				QTreeWidgetItem *sourceItem = new QTreeWidgetItem();

				sourceItem->setText(0, sourceName);

				sceneTreeItem->addChild(sourceItem);

				return true;
			},
			sceneItem);
	}

	obs_frontend_source_list_free(&scenes);

	sceneTree->expandAll();
}

void VideoWallDialog::loadSettings()
{
	config_t *config = obs_frontend_get_user_config();

	int rows = config_get_int(config, "VideoWall", "rows");

	int cols = config_get_int(config, "VideoWall", "cols");

	if (rows < 1)
		rows = 2;

	if (cols < 1)
		cols = 2;

	rowsSpin->setValue(rows);
	colsSpin->setValue(cols);

	preview->setGrid(rows, cols);
}

void VideoWallDialog::saveSettings()
{
	config_t *config = obs_frontend_get_user_config();

	config_set_int(config, "VideoWall", "rows", rowsSpin->value());

	config_set_int(config, "VideoWall", "cols", colsSpin->value());

	config_save(config);
}