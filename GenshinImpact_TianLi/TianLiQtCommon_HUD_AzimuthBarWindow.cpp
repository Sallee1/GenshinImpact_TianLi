﻿#include "TianLiQtCommon_HUD_AzimuthBarWindow.h"
#include <QTimer>
#include <QPainter>
#include <QCloseEvent>
#include <QStyleOption>

#include "..\GenshinImpact_TianLi_Map\GenshinImpact_TianLi_Map.h"
#pragma comment(lib,"GenshinImpact_TianLi_Map.lib")

TianLiQtCommon_HUD_AzimuthBarWindow::TianLiQtCommon_HUD_AzimuthBarWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_QuitOnClose, false);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow | Qt::WindowStaysOnTopHint);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	
	SetWindowLong((HWND)winId(), GWL_EXSTYLE, GetWindowLong((HWND)winId(), GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED );
	
	//SetWindowBlurBehind(HWND(winId()));
	
	this->setMaximumSize(680, 138);
	this->setMinimumSize(680, 138);
	
	//this->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0, radius:0.5, fx:0.5, fy:0, stop:0 rgba(0, 0, 0, 20), stop:1 transparent);");

	timer=new QTimer;
	timer->setInterval(42);
	static int i = 0;
	connect(timer, &QTimer::timeout,this,&TianLiQtCommon_HUD_AzimuthBarWindow::slot_update);
	timer->start();
}

TianLiQtCommon_HUD_AzimuthBarWindow::~TianLiQtCommon_HUD_AzimuthBarWindow()
{
	delete timer;
}

void TianLiQtCommon_HUD_AzimuthBarWindow::paintEvent(QPaintEvent* event)
{
	static QImage base_line(":/TianLiQtCommon_HUD_AzimuthBarWindow/resource/TianLiCommon/HUD/AzimuthBarWindow/Base Line.png");
	static QImage base_arrow_center(":/TianLiQtCommon_HUD_AzimuthBarWindow/resource/TianLiCommon/HUD/AzimuthBarWindow/Base Arrow Center.png");
	
	static QImage flag_star(":/TianLiQtCommon_HUD_AzimuthBarWindow/resource/TianLiCommon/HUD/AzimuthBarWindow/Flag Star.png");
	
	static QImage flag_N(":/TianLiQtCommon_HUD_AzimuthBarWindow/resource/TianLiCommon/HUD/AzimuthBarWindow/Flag N.png");
	static QImage flag_E(":/TianLiQtCommon_HUD_AzimuthBarWindow/resource/TianLiCommon/HUD/AzimuthBarWindow/Flag E.png");
	static QImage flag_S(":/TianLiQtCommon_HUD_AzimuthBarWindow/resource/TianLiCommon/HUD/AzimuthBarWindow/Flag S.png");
	static QImage flag_W(":/TianLiQtCommon_HUD_AzimuthBarWindow/resource/TianLiCommon/HUD/AzimuthBarWindow/Flag W.png");
	
	
	const QRect avatar_arrow_rect(316, 90, 48, 48);
	const int x = width();
	const int y = height();
	const int main_r = 16;
	const int main_2r = 32;
	const int min_r = 2;
	const int min_2r = 4;
	const double x_c = avatar_arrow_rect.x() + 24;
	const double y_c = avatar_arrow_rect.y() + 24;

	
	double avatar_rotate_deg = avatar_rotate_deg_atom;
	double avatar_rotate_rad = avatar_rotate_deg / 180 * 3.14;
	

	QPainter painter(this);

	//painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	//painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	//// 底层椭圆渐变阴影
	//QRadialGradient radial_gradient(x, 0, 50);
	//radial_gradient.setColorAt(0, QColor(0, 0, 0, 0));
	//radial_gradient.setColorAt(1, QColor(200,200, 200, 255));
	//painter.setBrush(radial_gradient);
	//painter.setPen(Qt::NoPen);
	//painter.drawEllipse(QPointF(x/2, 0),100, 100);
	
	//	// 反走样 
	//painter.setRenderHint(QPainter::Antialiasing, true);
	//// 设置渐变色 qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.5 rgba(0, 0, 0, 166), stop:1 rgba(255, 255, 255, 255))
	//QRadialGradient radial(QPointF(x/2, 0),x/2,QPointF(x/2,0));
	//radial.setColorAt( 0, QColor(0, 0, 0, 128));
	//radial.setColorAt(1, QColor(0, 0, 0, 0));

	//// 设置显示模式
	////radial.setSpread(QGradient::PadSpread);

	
	//// 设置画笔颜色、宽度
	//painter.setPen(Qt::NoPen);
	//// 设置画刷填充
	//painter.setBrush(radial);
	//// 绘制椭圆
	//painter.drawRect(QRect(0, y-x, x, x));
	//

	// 绘制测试文字字样 详细信息
	painter.setPen(QColor(255, 255, 255, 255));
	//painter.setFont(QFont("Microsoft YaHei", 15, QFont::Normal));
	painter.drawText(QRect(0, 75, 300, 50), Qt::AlignCenter, show_info_text);

	QStyleOption option;
	option.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &option, &p, this);

	painter.setRenderHint(QPainter::Antialiasing);

	// 基础线条和箭头
	painter.drawImage(0, 37, base_line);
	painter.drawImage(332, 10, base_arrow_center);

	// 标点
	double base_point_list_param = 1.0 / 180 * 674;
	double base_point_list_N = static_cast<int>(-(-avatar_rotate_deg) + 180) * base_point_list_param;
	double base_point_det= 30 * base_point_list_param;
	for (int i = 0; i < 9; i += 1)
	{
		if (i % 3 == 0)
		{
			continue;
		}
		double base_point_list_x = static_cast<int>(base_point_list_N + i * base_point_det)%(674);
		painter.drawImage(base_point_list_x, 35, flag_star);
	}
	
	// avatar_rotate_deg = -180; x = 0
	// avatar_rotate_deg = 0; x = 315
	// avatar_rotate_deg = 180; x = 630
	
	double param_det = 1.0 / 180.0 * 630.0;
	
	double det_x_N = static_cast<int>(-(-avatar_rotate_deg)-90 + 180) % 360 * param_det ;
	painter.drawImage(det_x_N, 30, flag_N);
	
	double det_x_E = static_cast<int>(-(-avatar_rotate_deg - 90) - 90 + 180) % 360 * param_det;
	painter.drawImage(det_x_E, 30, flag_E);
	
	double det_x_S = static_cast<int>(-(-avatar_rotate_deg - 180) - 90 + 180) % 360 * param_det;
	painter.drawImage(det_x_S, 30, flag_S);
	
	double det_x_W = static_cast<int>(-(-avatar_rotate_deg - 270) - 90 + 180) % 360 * param_det;
	painter.drawImage(det_x_W, 30, flag_W);
	
		


	QPainter paint(this);

	//警告	C4996	'QPainter::HighQualityAntialiasing': Use Antialiasing instead	GenshinImpact_TianLi	C : \Users\GengG\source\repos\GenshinImpact_TianLi\GenshinImpact_TianLi\TianLiQtCommon_HUD_AzimuthBarWindow.cpp	153
	//paint.setRenderHint(QPainter::HighQualityAntialiasing);
	
	//paint.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	paint.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	//paint.fillRect(this.rect(), Qt::NoBrush);
	paint.fillRect(avatar_arrow_rect, Qt::transparent);
	paint.setCompositionMode(QPainter::CompositionMode_SourceOver);

	// 基础大圆
	paint.setPen(Qt::NoPen);//设置画笔形式 
	paint.setBrush(QBrush(Qt::white, Qt::SolidPattern));//设置画刷形式 
	paint.drawEllipse(x_c - main_r, y_c - main_r, main_2r, main_2r);

	if (object_map.size() > 0)
	{
		for (auto& object : object_map)
		{
			
		}
	}
	//if (ObjectList.size() > 0)
	//{
	//	for (int i = 0; i < ObjectList.size(); i++)
	//	{
	//		// 目标弧线
	//		paint.setPen(QPen(QColor(230, 185, 69), 2, Qt::SolidLine));
	//		int startAngle = 90 * 16;
	//		int spanAngle = ObjectList[i] * 16;
	//		QRectF R_C_arc(4.0, 4.0, 40.0, 40.0);
	//		paint.drawArc(R_C_arc, startAngle, spanAngle);

	//		// 目标小圆
	//		double x, y;
	//		paint.setPen(Qt::NoPen);//设置画笔形式 
	//		paint.setBrush(QColor(230, 185, 69));//设置画刷形式 
	//		x = sin(deg2rad(ObjectList[i])) * 20;
	//		y = cos(deg2rad(ObjectList[i])) * 20;
	//		QRectF R_C_Avatar(x_c - x - min_r, y_c - y - min_r, min_2r, min_2r);
	//		paint.drawEllipse(R_C_Avatar);
	//	}

	//}


	// 基础自身朝向小圆
	paint.setPen(Qt::NoPen);//设置画笔形式 
	paint.setBrush(Qt::white);//设置画刷形式 
	QRectF R_C_baseN(x_c - min_r, avatar_arrow_rect.y()+2, min_2r, min_2r);
	paint.drawEllipse(R_C_baseN);

	// 北方小圆
	//double x, y;
	//paint.setPen(Qt::NoPen);//设置画笔形式 
	//paint.setBrush(QColor(57, 255, 255));//设置画刷形式 
	//x = sin(avatar_rotate_rad) * 20;
	//y = cos(avatar_rotate_rad) * 20;
	//QRectF R_C_Avatar(x_c - x - min_r, y_c - y - min_r, min_2r, min_2r);
	//paint.drawEllipse(R_C_Avatar);

	//QPainterPath AvatarArrow;

	// 目标箭头
	double x_a, y_a;
	double x_b, y_b;
	x_a = sin(-avatar_rotate_rad) * 12;
	y_a = cos(-avatar_rotate_rad) * 12;
	x_b = sin((-avatar_rotate_deg + 90) / 180 * 3.14) * 4;
	y_b = cos((-avatar_rotate_deg + 90) / 180 * 3.14) * 4;

	QVector<QPointF> AvatarArrowPoints;
	AvatarArrowPoints.append(QPointF(x_c + x_b, y_c + y_b));
	AvatarArrowPoints.append(QPointF(x_c + x_a, y_c + y_a));
	AvatarArrowPoints.append(QPointF(x_c - x_b, y_c - y_b));

	paint.setPen(Qt::NoPen);
	paint.setBrush(Qt::transparent);
	paint.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	paint.drawPolygon(AvatarArrowPoints);
	paint.setCompositionMode(QPainter::CompositionMode_SourceOver);

	QVector<QPointF> NorthArrowPoints;
	NorthArrowPoints.append(QPointF(x_c + x_b, y_c + y_b));
	NorthArrowPoints.append(QPointF(x_c - x_b, y_c - y_b));
	NorthArrowPoints.append(QPointF(x_c - x_a, y_c - y_a));

	paint.setPen(Qt::NoPen);
	paint.setBrush(QColor(42, 255, 217));
	paint.drawPolygon(NorthArrowPoints);
	paint.setCompositionMode(QPainter::CompositionMode_SourceOver);

}

void TianLiQtCommon_HUD_AzimuthBarWindow::closeEvent(QCloseEvent* event)
{
	event->accept();
	emit signal_close_finished();
}

void TianLiQtCommon_HUD_AzimuthBarWindow::slot_update()
{
	//avatar_rotate_deg_atom = -avatar_rotate_deg;
	//TrackResult traack_res = Core.GetTrack().GetResult();
	auto& track_res = CoreMap.Core.GetTrack().GetResult();
	if (track_res.is_find_paimon)
	{
		this->show();
		avatar_rotate_deg_atom = track_res.viewer_angle;
		slot_update_move(track_res.client_rect);
		update();
	}
	else
	{
		if (!is_visible)
		{
			this->hide();
		}
	}
	//update();
}

void TianLiQtCommon_HUD_AzimuthBarWindow::slot_show()
{
	is_visible = true;
	this->show();
	timer->start();
}

void TianLiQtCommon_HUD_AzimuthBarWindow::slot_hide()
{
	is_visible = false;
	this->hide();
	timer->stop();
}

void TianLiQtCommon_HUD_AzimuthBarWindow::slot_update_avatar_rotate(double avatar_rotate)
{
	avatar_rotate_deg_atom = avatar_rotate;
	update();
}

void TianLiQtCommon_HUD_AzimuthBarWindow::slot_update_move(RECT& gi_client_rect)
{
	static RECT gi_client_rect_old;
	if (gi_client_rect_old.left != gi_client_rect.left || gi_client_rect_old.top != gi_client_rect.top
		|| gi_client_rect_old.right != gi_client_rect.right || gi_client_rect_old.bottom != gi_client_rect.bottom)
	{
		gi_client_rect_old = gi_client_rect;
		// 移动到 水平上对齐，垂直居中对齐 的位置
		int move_x = gi_client_rect.left + (gi_client_rect.right - gi_client_rect.left - width()) / 2;
		int move_y = gi_client_rect.top;
		move(move_x, move_y);
		update();
	}
}

void TianLiQtCommon_HUD_AzimuthBarWindow::slot_update_object()
{
}

void TianLiQtCommon_HUD_AzimuthBarWindow::slot_update_show_info(QString text)
{
	show_info_text = text;
	update();
}
