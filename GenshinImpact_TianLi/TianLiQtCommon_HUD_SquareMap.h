#pragma once
#include <opencv2/opencv.hpp>

#include <QWidget>
#include "ui_TianLiQtCommon_HUD_SquareMap.h"
#include <windows.h>

class GenshinImpact_TianLi;

class TianLiQtCommon_HUD_SquareMap : public QWidget
{
	Q_OBJECT

public:
	TianLiQtCommon_HUD_SquareMap(QWidget *parent = Q_NULLPTR);
	~TianLiQtCommon_HUD_SquareMap();

private:
	Ui::TianLiQtCommon_HUD_SquareMap ui;

private:
	QPoint move_press;
	QPoint move_value;
	bool is_left_clicked = false;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
// UI�ػ�	
private:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
	void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
public:
	void setMainPage(GenshinImpact_TianLi* tianli);
private:
	GenshinImpact_TianLi* main_tianli{};
private:
	QTimer* timer = nullptr;
	bool is_visible = false;	// �Ƿ�ɼ�
	
	cv::Point render_map_pos = cv::Point(1428, 2937);
	double render_map_scale = 1.0;
	
	QImage render_map_image;
	cv::Mat gi_map;
	cv::Mat render_map_mask;
	bool is_need_rerender = false;
public slots:
	void slot_update();
	void slot_show();
	void slot_hide();
public:
	void slot_update_move(RECT& gi_minimap_rect);
signals:
	void signal_close_finished();
	
};

