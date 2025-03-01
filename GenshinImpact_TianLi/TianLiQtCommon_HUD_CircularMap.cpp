#include "TianLiQtCommon_HUD_CircularMap.h"
#include <QTimer>
#include <QPainter>
#include <QCloseEvent>

#include "TianLiQtCommon_Logger.h"
#include "TianLiQtCommon_Utils.h"


#include "..\GenshinImpact_TianLi_Map\GenshinImpact_TianLi_Map.h"
#pragma comment(lib,"GenshinImpact_TianLi_Map.lib")

TianLiQtCommon_HUD_CircularMap::TianLiQtCommon_HUD_CircularMap(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_QuitOnClose, false);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow | Qt::WindowStaysOnTopHint);
	this->setAttribute(Qt::WA_TranslucentBackground, true);

	SetWindowLong((HWND)winId(), GWL_EXSTYLE, GetWindowLong((HWND)winId(), GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);

	timer = new QTimer;
	timer->setInterval(42);
	connect(timer, &QTimer::timeout, this, &TianLiQtCommon_HUD_CircularMap::slot_update);
	timer->start();
}

TianLiQtCommon_HUD_CircularMap::~TianLiQtCommon_HUD_CircularMap()
{
	delete timer;
}

void TianLiQtCommon_HUD_CircularMap::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	// 清空
	painter.setCompositionMode(QPainter::CompositionMode_Clear);
	painter.fillRect(this->rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	const int w = width();
	const int h = height();

	static cv::Mat mapMatRect;
	static cv::Point old_map_center_pos;
	static double old_map_scale = 0;

	// 如果 old pos 和 old scale 与当前的一样，就不用重新计算了
	if (old_map_center_pos != render_map_pos || old_map_scale != render_map_scale || is_need_rerender)
	{
		is_need_rerender = false;
		old_map_center_pos = render_map_pos;
		old_map_scale = render_map_scale;

		//cv::Rect viewer_rect;
		//mapMatRect = TianLi::Utils::get_view_map(gi_map, cv::Size(width(), height()), render_map_pos, render_map_scale, viewer_rect);
		//CoreMap.map_info.is_overlay = true;

		if (Core.GetResource().GIMAP_OVERLAY_RECT.contains(render_map_pos))
		{
			CoreMap.map_info.is_show_map = false;
		}
		CoreMap.map_info.center_x = render_map_pos.x;
		CoreMap.map_info.center_y = render_map_pos.y;
		CoreMap.map_info.viewer_width = this->width();
		CoreMap.map_info.viewer_height = this->height();

		//CoreMap.map_info.map_rect = map_rect;
		CoreMap.map_info.scale_form_gimap = 1.0;
		// 渲染图例
		CoreMap.render_legend(mapMatRect);
		
		std::vector<cv::Mat> mv;
		cv::split(mapMatRect, mv);
		mv[3] = render_map_mask&mv[3];
		cv::merge(mv, mapMatRect);

		render_map_image = QImage((uchar*)(mapMatRect.data), mapMatRect.cols, mapMatRect.rows, mapMatRect.cols * (mapMatRect.channels()), QImage::Format_ARGB32);

	}
	painter.drawImage(0, 0, render_map_image);
}

void TianLiQtCommon_HUD_CircularMap::closeEvent(QCloseEvent* event)
{
	event->accept();
	emit signal_close_finished();
}

void TianLiQtCommon_HUD_CircularMap::resizeEvent(QResizeEvent* event)
{
	const int w = event->size().width();
	const int h = event->size().height();
	render_map_mask = TianLi::Utils::create_circular_mask(w, h, 20);
	is_need_rerender = true;
}

void TianLiQtCommon_HUD_CircularMap::slot_update()
{
	if (is_visible)
	{
		if (Core.GetTrack().GetResult().is_find_paimon)
		{
			this->show();
			render_map_pos = cv::Point(Core.GetTrack().GetResult().position_x, Core.GetTrack().GetResult().position_y);
			//LogInfo(std::to_string(render_map_pos.x).c_str());

			RECT gi_minimap_rect = Core.GetTrack().GetResult().minimap_rect;
			slot_update_move(gi_minimap_rect);
			update();
		}
		else
		{
			if (!is_visible)
			{
				this->hide();
			}
		}
	}
}

void TianLiQtCommon_HUD_CircularMap::slot_show()
{
	is_visible = true;
	this->show();
	timer->start();
}

void TianLiQtCommon_HUD_CircularMap::slot_hide()
{
	is_visible = false;
	this->hide();
	timer->stop();
}

void TianLiQtCommon_HUD_CircularMap::slot_update_move(RECT& gi_minimap_rect)
{
	static RECT gi_paimon_rect_old;
	if (gi_paimon_rect_old.left != gi_minimap_rect.left || gi_paimon_rect_old.top != gi_minimap_rect.top
		|| gi_paimon_rect_old.right != gi_minimap_rect.right || gi_paimon_rect_old.bottom != gi_minimap_rect.bottom)
	{
		gi_paimon_rect_old = gi_minimap_rect;

		int move_x = gi_minimap_rect.left;
		int move_y = gi_minimap_rect.top;
		int resize_w = gi_minimap_rect.right - gi_minimap_rect.left;
		int resize_h = gi_minimap_rect.bottom - gi_minimap_rect.top;
		this->setGeometry(move_x, move_y, resize_w, resize_h);

	}
}

