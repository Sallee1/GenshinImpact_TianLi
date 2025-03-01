﻿#pragma execution_character_set("utf-8")

#include "GenshinImpact_TianLi.h"

#include "TianLiQtCommon_MapRect.h"
#include "TianLiQtCommon_CardRect.h"
#include "TianLiQtCommon_ScrollCardRect.h"
#include "TianLiQtCommon_SelectedItemButton.h"
#include "TianLiQtCommon_PickedItemButton.h"
#include "TianLiQtCommon_NearbyItemButton.h"
#include "TianLiQtCommon_TypeGroupButton.h"
#include "TianLiQtCommon_SwitchButton.h"

#include "TianLiQtCommon_Logger.h"

#include "TianLiQtCommon_HUD_CircularMap.h"
#include "TianLiQtCommon_HUD_SquareMap.h"
#include "TianLiQtCommon_HUD_AzimuthBarWindow.h"

#include "TianLiQtCommon_HookKeyBoard.h"
#include "TianLiQtCommon_ListenKeyBoard.h"

#include "TianLiQtCommon_Utils.h"

#include <QFontDatabase>
#include <QMouseEvent>
#include <QTimer>
#include <QFile>
#include <QTextStream>


#include "..\GenshinImpact_TianLi_Map\GenshinImpact_TianLi_Map.h"
#pragma comment(lib,"GenshinImpact_TianLi_Map.lib")

#include "..\GenshinImpact_TianLi_Data\GenshinImpact_TianLi_Data.h"
#pragma comment(lib,"GenshinImpact_TianLi_Data.lib")

#include "Logger/TianLi.Logger/TianLi.Logger.h"
#pragma comment(lib, "TianLi.Logger.lib")

using namespace TianLi;

GenshinImpact_TianLi::GenshinImpact_TianLi(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	TianLi_Logger;
	// 从qrc中加载 qrc:/Version/resource/Version/version.ver
	QFile file(":/Version/resource/Version/version.ver");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QString line = in.readLine();
		LogInfo(line.toStdString().c_str());
		//this->setWindowTitle(QString("天理辅助 %1").arvg(line));
		file.close();
	}
	

	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	
	mainShadow = new QGraphicsDropShadowEffect(this);
	mainShadow->setOffset(0, 0);
	mainShadow->setColor(QColor(0, 0, 0, 255));
	mainShadow->setBlurRadius(15);

	ui.label_Main->setGraphicsEffect(mainShadow);


	hud_square_map = new TianLiQtCommon_HUD_SquareMap(NULL);
	hud_square_map->hide();
	hud_square_map->setMainPage(this);
	hud_circular_map = new TianLiQtCommon_HUD_CircularMap(NULL);
	hud_circular_map->hide();
	hud_azimuth_bar_window = new TianLiQtCommon_HUD_AzimuthBarWindow(NULL);
	hud_azimuth_bar_window->hide();
	

	this->addUI_Tab_Map();
	this->addUI_Tab_HUD();
	this->addUI_Tab_3();
	this->addUI_Tab_4();
	this->addUI_Tab_Set();
	ui.stackedWidget_MainTabPages->setCurrentIndex(0);
	
	init_area_list();
	init_type_list();

	// 测试
	{
		if (area_button_index_map.contains("诸法丛林"))
		{
			int id = area_button_index_map["诸法丛林"];
			area_button_group.button(id)->setChecked(true);
			LogInfo(QString(area_button_group.button(id)->text() + " 被选中").toStdString().c_str());
		}
		if (type_button_index_map.contains("特产"))
		{
			int id = type_button_index_map["特产"];
			type_button_group.button(id)->setChecked(true);
			LogInfo(QString(type_button_group.button(id)->text() + " 被选中").toStdString().c_str());
			type_button_group.button(id)->clicked(true);
		}
		if (item_button_index_map.contains("劫波莲"))
		{
			int id = item_button_index_map["劫波莲"];
			item_button_group.button(id)->setChecked(true);
			LogInfo(QString(item_button_group.button(id)->text() + " 被选中").toStdString().c_str());
			item_button_group.button(id)->clicked(true);
		}
		LogInfo("测试结束");
	}
	
	//添加全局快捷键
	// F1 触发 slot_show_or_hide
	//hook_key_board_list.push_back(new TianLiQtCommon_HookKeyBoard("F1", this));
	//connect(hook_key_board_list.back(), &TianLiQtCommon_HookKeyBoard::signal_activated, this, &GenshinImpact_TianLi::slot_show_or_hide);
	
	hook_key_board_list.push_back(new TianLiQtCommon_HookKeyBoard("Alt+T", this));
	connect(hook_key_board_list.back(), &TianLiQtCommon_HookKeyBoard::signal_activated, this, &GenshinImpact_TianLi::slot_auto_track);
	// listen TianLiQtCommon_ListenKeyBoard
	
	TianLiQtCommon_ListenKeyBoard* listen_key_board = new TianLiQtCommon_ListenKeyBoard(this);
	
	
	// listen_key_board->register_key_signal(0x41, this, &GenshinImpact_TianLi::pushButton_Tab_1_clicked);
	// F1 触发 slot_show_or_hide
	listen_key_board->register_key(0x70, this, &GenshinImpact_TianLi::slot_show_or_hide);
	// ESC 触发隐藏 slot_hide
	listen_key_board->register_key(0x1B, this, &GenshinImpact_TianLi::slot_hide);
	
	
	//connect(this, &GenshinImpact_TianLi::show, this, &GenshinImpact_TianLi::slot_show);
	//connect(this, &GenshinImpact_TianLi::hide, this, &GenshinImpact_TianLi::slot_hide);
	//ui.Tab_ButtonGroup->setId();
	connect(ui.Tab_ButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) {
		if (ui.Tab_ButtonGroup->button(id)->isChecked())
		{
			ui.stackedWidget_MainTabPages->setCurrentIndex(-2-id);
			LogInfo((QString::number(id) + " -=> " + QString::number(-2 - id)).toStdString().c_str());
		}
		});
	connect(ui.pushButton_Set, &QPushButton::clicked, [=]() {
			ui.stackedWidget_MainTabPages->setCurrentIndex(4);
		});
}

GenshinImpact_TianLi::~GenshinImpact_TianLi()
{
}

void GenshinImpact_TianLi::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton &&
		ui.label_Main->frameRect().contains(event->globalPos() - this->frameGeometry().topLeft()))
	{
		{
			m_Press = event->globalPos();
			leftBtnClk = true;
		}
	}
	//event->ignore();
}
void GenshinImpact_TianLi::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		leftBtnClk = false;
	}
	//event->ignore();
}
void GenshinImpact_TianLi::mouseMoveEvent(QMouseEvent* event)
{
	if (leftBtnClk) {
		m_Move = event->globalPos();
		this->move(this->pos() + m_Move - m_Press);
		m_Press = m_Move;
	}
	//event->ignore();
}

void GenshinImpact_TianLi::closeEvent(QCloseEvent* event)
{
	if (close_is_mini)
	{
		slot_hide();
		//this->hide();
		event->ignore();
	}
	else
	{
		event->accept();
	}
}

void GenshinImpact_TianLi::init_area_list()
{
	auto widget_ptr = PageTabMap_LeftCardRects[0];
	auto& area = Data.area_group;
	for (auto& [parent_name, childs] : area)
	{
		for (std::string& child_name : childs)
		{
			TianLiQtCommon_TypeGroupButton* new_child_button = new TianLiQtCommon_TypeGroupButton(child_name.c_str(), widget_ptr);
			new_child_button->setParent(widget_ptr);

			// 添加至按钮组
			area_button_group.addButton(new_child_button);
			int button_id = area_button_group.id(new_child_button);
			// 记录在按钮组中的id
			area_button_index_map.insert({ child_name, button_id });

			new_child_button->setGeometry(static_cast<int>((area_button_index_map.size()-1) % 5 * 60), static_cast<int>((area_button_index_map.size()-1) / 5 * 40), 60, 30);

			// 显示按钮
			new_child_button->show();

			//connect(new_child_button, &TianLiQtCommon_TypeGroupButton::clicked, this, &GenshinImpact_TianLi::pushButtonGroup_SelectCountry);
			connect(new_child_button, &TianLiQtCommon_TypeGroupButton::clicked, [=](bool checked) {
				if (checked == true)
				{
					updata_selectable_item(get_selected_area().toStdString(), get_selected_type().toStdString());
				}
				});
		}
	}
}

QString GenshinImpact_TianLi::get_selected_area()
{
	auto button = area_button_group.checkedButton();
	if (button == nullptr)
	{
		button = area_button_group.button(0);
		if (button == nullptr)
		{
			return "";
		}
	}
	return button->text();
}

void GenshinImpact_TianLi::init_type_list()
{
	auto widget_ptr = PageTabMap_LeftCardRects[1];
	auto& type = Data.type_group;
	for (auto& [parent_name, childs] : type)
	{
		for (std::string& child_name : childs)
		{
			TianLiQtCommon_TypeGroupButton* new_child_button = new TianLiQtCommon_TypeGroupButton(child_name.c_str(), widget_ptr);
			new_child_button->setParent(widget_ptr);

			// 添加至按钮组
			type_button_group.addButton(new_child_button);
			int button_id = type_button_group.id(new_child_button);
			// 记录在按钮组中的id
			type_button_index_map.insert({ child_name, button_id });

			new_child_button->setGeometry(static_cast<int>(type_button_index_map.size() % 5 * 60), static_cast <int>(80 + type_button_index_map.size() / 5 * 40), 60, 30);

			// 显示按钮
			new_child_button->show();

			//connect(new_child_button, &TianLiQtCommon_TypeGroupButton::clicked, this, &GenshinImpact_TianLi::pushButtonGroup_SelectType);
			connect(new_child_button, &TianLiQtCommon_TypeGroupButton::clicked, [=](bool checked) {
				if (checked == true)
				{
					updata_selectable_item(get_selected_area().toStdString(), get_selected_type().toStdString());
				}
				});
		}
	}
}

QString GenshinImpact_TianLi::get_selected_type()
{
	auto button = type_button_group.checkedButton();
	if (button == nullptr)
	{
		button = type_button_group.button(0);
		if (button == nullptr)
		{
			return "";
		}
	}
	return button->text();
}

void GenshinImpact_TianLi::updata_selectable_item(std::string area, std::string type)
{
	// using namespace std;
	auto widget_ptr = PageTabMap_LeftCardRects[2];
	auto& items = Data.item_group;
	if (items.contains({ area,type }))
	{
		// 清空原有按钮组
		auto buttonList = item_button_group.buttons();
		for (int i = 0; i < buttonList.size(); ++i)
		{
			buttonList[i]->hide();
			item_button_group.removeButton(buttonList[i]);
		}
		// 清空按钮组id目录
		item_button_index_map.clear();
		
		for (auto& item : items[{ area, type }])
		{			
			item_button_group.setExclusive(false);
			
			TianLiQtCommon_TypeGroupButton* new_child_button = new TianLiQtCommon_TypeGroupButton(item.c_str(), widget_ptr);
			new_child_button->setParent(widget_ptr);

			// 添加至按钮组
			item_button_group.addButton(new_child_button);
			int button_id = item_button_group.id(new_child_button);
			// 记录在按钮组中的id
			item_button_index_map.insert({ item, button_id });

			new_child_button->setGeometry(static_cast<int>(item_button_index_map.size() % 5 * 60), static_cast < int>(130 + item_button_index_map.size() / 5 * 40), 60, 30);
			
			// 显示按钮
			new_child_button->show();
			connect(new_child_button, &TianLiQtCommon_TypeGroupButton::clicked, this, &GenshinImpact_TianLi::pushButtonGroup_SelectItem);

			// 同步按钮状态
			if (item_button_checked_map.contains({ area,type,item }))
			{
				new_child_button->setChecked(item_button_checked_map[{ area, type, item }]);
			}
			else
			{
				item_button_checked_map.insert({ { area, type, item } , false });
			}
		}
	}

}
void GenshinImpact_TianLi::addUI_Tab_Map()
{
	this->addUI_MapTabCardRects();
	this->addUI_MapTabMapRect();
	this->update();
}
void GenshinImpact_TianLi::addUI_Tab_HUD()
{
	this->addUI_HUDTabCardRects();
	this->update();
}
void GenshinImpact_TianLi::addUI_Tab_3()
{

}
void GenshinImpact_TianLi::addUI_Tab_4()
{

}

void GenshinImpact_TianLi::addUI_Tab_Set()
{
	auto button_set_capture_bitblt = new TianLiQtCommon_SwitchButton(ui.page_set, "使用Bitblt");
	auto button_set_capture_driectx = new TianLiQtCommon_SwitchButton(ui.page_set, "使用DriectX");
	auto button_is_mini = new TianLiQtCommon_SwitchButton(ui.page_set, "最小化");
	
	button_set_capture_bitblt->setGeometry(30, 50, 200, 25);
	button_set_capture_driectx->setGeometry(30, 100, 200, 25);

	//button_set_capture_bitblt->move(30, 50);
	//button_set_capture_driectx->move(30, 100);
	button_is_mini->move(30, ui.page_set->height() - 35);
	
	connect(button_set_capture_bitblt, &TianLiQtCommon_SwitchButton::signal_clicked, [=](bool is_checked) {
		if (is_checked)
		{
			auto config = Core.GetTrack().GetConfig();
			config.capture_type = TianLi::Track::CaptureType::Bitblt;
			Core.GetTrack().SetConfig(config);
		}
		else
		{
		}
		});

	connect(button_set_capture_driectx, &TianLiQtCommon_SwitchButton::signal_clicked, [=](bool is_checked) {
		if (is_checked)
		{
			auto config = Core.GetTrack().GetConfig();
			config.capture_type =TianLi::Track::CaptureType::DirectX;
			Core.GetTrack().SetConfig(config);
		}
		else
		{
		}
		});
	connect(dynamic_cast<TianLiQtCommon_SwitchButton*>(button_is_mini), &TianLiQtCommon_SwitchButton::signal_clicked, [=](bool is_checked) {
		if (is_checked)
		{
			this->close_is_mini = true;
		}
		else
		{
			this->close_is_mini = false;
		}
		});

	//QButtonGroup* group_set_capture = new QButtonGroup(ui.page_set);

	//group_set_capture->addButton(button_set_capture_bitblt);
	//group_set_capture->addButton(button_set_capture_driectx);

	//group_set_capture->setExclusive(true);


	//button_set_capture_bitblt->setParent(ui.page_set);
	//button_set_capture_driectx->setParent(ui.page_set);

		
}

void GenshinImpact_TianLi::addUI_MapTabCardRects()
{
	// 地图页面左侧的左栏上的卡片
	PageTabMap_LeftCardRects.append(new TianLiQtCommon_CardRect(this));
	PageTabMap_LeftCardRects[0]->setParent(ui.widget_MapTab_Left);
	PageTabMap_LeftCardRects[0]->setGeometry(36, 28, 302, 148);

	PageTabMap_LeftCardRects.append(new TianLiQtCommon_CardRect(this));
	PageTabMap_LeftCardRects[1]->setParent(ui.widget_MapTab_Left);
	PageTabMap_LeftCardRects[1]->setGeometry(36, 112, 302, 200);
	
	PageTabMap_LeftCardRects.append(new TianLiQtCommon_CardRect(this));
	PageTabMap_LeftCardRects[2]->setParent(ui.widget_MapTab_Left);
	PageTabMap_LeftCardRects[2]->setGeometry(36, 200, 302, 507); 
	
	// 对地图页面左侧的左栏上的卡片进行遮盖排序
	PageTabMap_LeftCardRects[2]->raise();
	PageTabMap_LeftCardRects[1]->raise();
	PageTabMap_LeftCardRects[0]->raise();
	
	// 地图页面左侧的右栏上的卡片
	PageTabMap_RightCardRects.append(new TianLiQtCommon_CardRect(this));
	PageTabMap_RightCardRects[0]->setParent(ui.widget_MapTab_Left);
	PageTabMap_RightCardRects[0]->setGeometry(350, 28, 260, 42);
	
	// 地图页面左侧的右栏上的卡片
	PageTabMap_ScrollCardRect.append(new TianLiQtCommon_ScrollCardRect("选中物品", this));
	PageTabMap_ScrollCardRect[0]->setParent(ui.widget_MapTab_Left);
	PageTabMap_ScrollCardRect[0]->setGeometry(350, 81, 260, 288);

	// 添加一键取消按钮
	QPushButton* all_cancel_button = new QPushButton(PageTabMap_ScrollCardRect[0]);
	all_cancel_button->setParent(PageTabMap_ScrollCardRect[0]);
	all_cancel_button->setGeometry(220, 15, 20, 20);
	connect(all_cancel_button, &QPushButton::clicked, [=]() {
		// 遍历所有select按钮，触发双击事件
		auto buttons = object_button_group.buttons();
		for (int i = 0; i < buttons.size(); i++)
		{
			TianLiQtCommon_SelectedItemButton* button = qobject_cast<TianLiQtCommon_SelectedItemButton*>(buttons[i]);
			if (button != nullptr)
			{
				emit button->signal_double_click(true);
				button->deleteLater();
				object_button_group.removeButton(buttons[i]);
			}
		}
		});
	
	PageTabMap_ScrollCardRect.append(new TianLiQtCommon_ScrollCardRect("附近物品日志", this));
	PageTabMap_ScrollCardRect[1]->setParent(ui.widget_MapTab_Left);
	PageTabMap_ScrollCardRect[1]->setGeometry(350, 380, 260, 160);

	PageTabMap_ScrollCardRect.append(new TianLiQtCommon_ScrollCardRect("捡取物品日志", this));
	PageTabMap_ScrollCardRect[2]->setParent(ui.widget_MapTab_Left);
	PageTabMap_ScrollCardRect[2]->setGeometry(350, 551, 260, 160);

}

void GenshinImpact_TianLi::addUI_MapTabMapRect()
{
	// 地图页面右侧的地图区域
	PageTabMap_MapRect=new TianLiQtCommon_MapRect(this);
	PageTabMap_MapRect->setParent(ui.widget_MapTab_Right);
	PageTabMap_MapRect->setGeometry(10, 10, ui.widget_MapMask->width() - 20, ui.widget_MapMask->height() - 20);
	connect(PageTabMap_MapRect, &TianLiQtCommon_MapRect::singal_updata_pickable_items, this, &GenshinImpact_TianLi::slot_updata_pickable_items);
	// 绑定传输数据到方位条
	connect(PageTabMap_MapRect, &TianLiQtCommon_MapRect::signle_send_mini_object_info_text, hud_azimuth_bar_window, &TianLiQtCommon_HUD_AzimuthBarWindow::slot_update_show_info);
	
	// 添加地图页面 地图区域中 的 定位启用切换开关
	PageTabMap_RightCard_Buttons.append(new TianLiQtCommon_SwitchButton(this,"定位"));
	PageTabMap_RightCard_Buttons[0]->setParent(ui.widget_MapTab_Right);
	PageTabMap_RightCard_Buttons[0]->move(30, PageTabMap_MapRect->height() - 35);
	// 定位启用切换事件
	connect(dynamic_cast<TianLiQtCommon_SwitchButton*>(PageTabMap_RightCard_Buttons[0]), &TianLiQtCommon_SwitchButton::signal_clicked, [](bool is_checked){
		if (is_checked)
		{
			Core.GetTrack().StartServer();
		}
		else
		{
			Core.GetTrack().StopServer();
		}
		});
	// 定位切换按钮与地图双击切换绑定同步
	connect(PageTabMap_MapRect, &TianLiQtCommon_MapRect::signal_double_click, dynamic_cast<TianLiQtCommon_SwitchButton*>(PageTabMap_RightCard_Buttons[0]), &TianLiQtCommon_SwitchButton::slot_clicked);
	
	// 添加地图页面 地图区域中 的 地下显示切换开关
	PageTabMap_RightCard_Buttons.append(new TianLiQtCommon_SwitchButton(this, "地下"));
	PageTabMap_RightCard_Buttons[1]->setParent(ui.widget_MapTab_Right);
	PageTabMap_RightCard_Buttons[1]->move(30, PageTabMap_MapRect->height() - 35 - 25 - 3);
	// 地下显示切换事件
	connect(dynamic_cast<TianLiQtCommon_SwitchButton*>(PageTabMap_RightCard_Buttons[1]), &TianLiQtCommon_SwitchButton::signal_clicked, [=](bool is_checked) {
		if (is_checked)
		{
			CoreMap.map_info.is_overlay = true;
			LogInfo("显示地下");
		}
		else
		{
			CoreMap.map_info.is_overlay = false;
			LogInfo("隐藏地下");
		}
		// 切换后要触发MapRect的强制重绘
		PageTabMap_MapRect->slot_force_update();
		LogInfo("触发重绘地图");
		});
	
	
}

void GenshinImpact_TianLi::addUI_HUDTabCardRects()
{
	// 1259 * 739 三份均分
	// 可用宽度为 1259 - 36*2 = 1223
	// 可用高度为 739 - 28*2 = 671
	// 三分之一宽度为 1223 / 3 = 396


	// 右边
	PageTabHUD_CardRects.append(new TianLiQtCommon_CardRect(this));
	PageTabHUD_CardRects.last()->setParent(ui.widget_HUDTab);
	PageTabHUD_CardRects.last()->setGeometry(36, 28, 396, 671);
	
	// 中间
	PageTabHUD_CardRects.append(new TianLiQtCommon_CardRect(this));
	PageTabHUD_CardRects.last()->setParent(ui.widget_HUDTab);
	PageTabHUD_CardRects.last()->setGeometry(36+396, 28, 396, 671);
	
	// 左边
	PageTabHUD_CardRects.append(new TianLiQtCommon_CardRect(this));
	PageTabHUD_CardRects.last()->setParent(ui.widget_HUDTab);
	PageTabHUD_CardRects.last()->setGeometry(36+396*2, 28, 396, 671);
	
	//添加标题文字 放在中间
	QLabel* page2_label_1_1 = new QLabel(this);
	page2_label_1_1->setParent(PageTabHUD_CardRects[0]);
	page2_label_1_1->setGeometry(0, 0, 396, 100);
	// 设置文字上下左右居中
	page2_label_1_1->setAlignment(Qt::AlignCenter);
	// 设置文字为 悬浮窗
	page2_label_1_1->setText("悬浮窗");
	// 字体设置为 HYWenHei 字号为 32pt
	page2_label_1_1->setFont(QFont("HYWenHei", 32));
	// 设置文字颜色为 棕色
	page2_label_1_1->setStyleSheet("color:rgb(153,102,0)");
	
	

	//添加标题文字 放在中间
	QLabel* page2_label_1_2 = new QLabel(this);
	page2_label_1_2->setParent(PageTabHUD_CardRects[1]);
	page2_label_1_2->setGeometry(0, 0, 396, 100);
	// 设置文字上下左右居中
	page2_label_1_2->setAlignment(Qt::AlignCenter);
	// 设置文字为 导航方位条
	page2_label_1_2->setText("导航方位条");
	// 字体设置为 HYWenHei 字号为 32pt
	page2_label_1_2->setFont(QFont("HYWenHei", 32));
	// 设置文字颜色为 棕色
	page2_label_1_2->setStyleSheet("color:rgb(153,102,0)");



#define Temp
#ifdef Temp


	QPushButton* page2_button_1_1 = new QPushButton(this);
	page2_button_1_1->setParent(PageTabHUD_CardRects[0]);
	page2_button_1_1->setGeometry(10, 120, 376, 80);
	page2_button_1_1->setText("未开启");
	// 字体设置为 HYWenHei 字号为 32pt
	page2_button_1_1->setFont(QFont("HYWenHei", 32));
	// 设置文字颜色为 棕色
	page2_button_1_1->setStyleSheet("color:rgb(153,102,0)");
	// 设置按钮不会弹起
	page2_button_1_1->setAttribute(Qt::WA_Hover, false);
	// 设置按钮不会获取焦点
	page2_button_1_1->setFocusPolicy(Qt::NoFocus);
	// 连接按钮点击事件
	connect(hud_square_map, &TianLiQtCommon_HUD_SquareMap::signal_close_finished, page2_button_1_1, [=]() {
		// 改变按钮文字为 未开启
		page2_button_1_1->setText("未开启");
		// 设置按钮文字颜色为 红色
		page2_button_1_1->setStyleSheet("color:rgb(255,0,0)");

		});
	connect(page2_button_1_1, &QPushButton::clicked, [=]() {
		// 如果按钮文字为 未开启
		if (page2_button_1_1->text() == "未开启")
		{
			// 改变按钮文字为 已开启
			page2_button_1_1->setText("已开启");
			// 设置按钮文字颜色为 绿色
			page2_button_1_1->setStyleSheet("color:rgb(0,255,0)");

			// 显示空白无边框窗口
			hud_square_map->slot_show();

		}
		// 如果按钮文字为 已开启
		else if (page2_button_1_1->text() == "已开启")
		{
			// 改变按钮文字为 未开启
			page2_button_1_1->setText("未开启");
			// 设置按钮文字颜色为 红色
			page2_button_1_1->setStyleSheet("color:rgb(255,0,0)");

			// 隐藏空白无边框窗口
			hud_square_map->slot_hide();
		}
		});
	



	QPushButton* page2_button_1_2 = new QPushButton(this);
	page2_button_1_2->setParent(PageTabHUD_CardRects[0]);
	page2_button_1_2->setGeometry(10, 300 , 376, 80);
	page2_button_1_2->setText("未开启");
	// 字体设置为 HYWenHei 字号为 32pt
	page2_button_1_2->setFont(QFont("HYWenHei", 32));
	// 设置文字颜色为 棕色
	page2_button_1_2->setStyleSheet("color:rgb(153,102,0)");
	// 设置按钮不会弹起
	page2_button_1_2->setAttribute(Qt::WA_Hover, false);
	// 设置按钮不会获取焦点
	page2_button_1_2->setFocusPolicy(Qt::NoFocus);
	// 连接按钮点击事件
	connect(hud_circular_map, &TianLiQtCommon_HUD_CircularMap::signal_close_finished, page2_button_1_2, [=]() {
		// 改变按钮文字为 未开启
		page2_button_1_2->setText("未开启");
		// 设置按钮文字颜色为 红色
		page2_button_1_2->setStyleSheet("color:rgb(255,0,0)");

		});
	connect(page2_button_1_2, &QPushButton::clicked, [=]() {
		// 如果按钮文字为 未开启
		if (page2_button_1_2->text() == "未开启")
		{
			// 改变按钮文字为 已开启
			page2_button_1_2->setText("已开启");
			// 设置按钮文字颜色为 绿色
			page2_button_1_2->setStyleSheet("color:rgb(0,255,0)");

			// 显示空白无边框窗口
			hud_circular_map->slot_show();

		}
		// 如果按钮文字为 已开启
		else if (page2_button_1_2->text() == "已开启")
		{
			// 改变按钮文字为 未开启
			page2_button_1_2->setText("未开启");
			// 设置按钮文字颜色为 红色
			page2_button_1_2->setStyleSheet("color:rgb(255,0,0)");

			// 隐藏空白无边框窗口
			hud_circular_map->slot_hide();
		}
		});




	
	QPushButton* page2_button_1_3 = new QPushButton(this);
	page2_button_1_3->setParent(PageTabHUD_CardRects[1]);
	page2_button_1_3->setGeometry(10, 120, 376, 80);
	page2_button_1_3->setText("未开启");
	// 字体设置为 HYWenHei 字号为 32pt
	page2_button_1_3->setFont(QFont("HYWenHei", 32));
	// 设置文字颜色为 棕色
	page2_button_1_3->setStyleSheet("color:rgb(153,102,0)");
	// 设置按钮不会弹起
	page2_button_1_3->setAttribute(Qt::WA_Hover, false);
	// 设置按钮不会获取焦点
	page2_button_1_3->setFocusPolicy(Qt::NoFocus);
	// 连接按钮点击事件
	connect(hud_azimuth_bar_window, &TianLiQtCommon_HUD_AzimuthBarWindow::signal_close_finished, page2_button_1_3, [=]() {
		// 改变按钮文字为 未开启
		page2_button_1_3->setText("未开启");
		// 设置按钮文字颜色为 红色
		page2_button_1_3->setStyleSheet("color:rgb(255,0,0)");

		});
	connect(page2_button_1_3, &QPushButton::clicked, [=]() {
		// 如果按钮文字为 未开启
		if (page2_button_1_3->text() == "未开启")
		{
			// 改变按钮文字为 已开启
			page2_button_1_3->setText("已开启");
			// 设置按钮文字颜色为 绿色
			page2_button_1_3->setStyleSheet("color:rgb(0,255,0)");

			// 显示空白无边框窗口
			hud_azimuth_bar_window->slot_show();

		}
		// 如果按钮文字为 已开启
		else if (page2_button_1_3->text() == "已开启")
		{
			// 改变按钮文字为 未开启
			page2_button_1_3->setText("未开启");
			// 设置按钮文字颜色为 红色
			page2_button_1_3->setStyleSheet("color:rgb(255,0,0)");

			// 隐藏空白无边框窗口
			hud_azimuth_bar_window->slot_hide();

		}
		});
#endif

	
	//添加标题文字 放在中间
	QLabel* page2_label_1_3 = new QLabel(this);
	page2_label_1_3->setParent(PageTabHUD_CardRects[2]);
	page2_label_1_3->setGeometry(0, 0, 396, 100);
	// 设置文字上下左右居中
	page2_label_1_3->setAlignment(Qt::AlignCenter);
	// 设置文字为 信息提示
	page2_label_1_3->setText("信息提示");
	// 字体设置为 HYWenHei 字号为 32pt
	page2_label_1_3->setFont(QFont("HYWenHei", 32));
	// 设置文字颜色为 棕色
	page2_label_1_3->setStyleSheet("color:rgb(153,102,0)");
	
	
}


void GenshinImpact_TianLi::slot_auto_track()
{
	if (Core.GetTrack().ServerState())
	{
		LogInfo("Track状态为已运行，执行停止");
		Core.GetTrack().StopServer();
	}
	else
	{
		LogInfo("Track状态为未运行，执行启动");
		Core.GetTrack().StartServer();
	}
}

QImage get_image(std::string item)
{
	auto img = Core.GetResource().GetImageBuffer("", "", "", item);
	auto img_qimage = TianLi::Utils::mat_2_qimage(img);
	return img_qimage;
}

void GenshinImpact_TianLi::slot_updata_pickable_items(std::vector<std::string> item_tags)
{
	static std::map<std::string, cv::Mat> item_tags_buf;
	static std::map<std::string, TianLiQtCommon_NearbyItemButton* > item_button_buf;

	std::map<std::string, bool> check_state_map;
	
	for (auto& item : item_tags)
	{
		if (item.size() <= 1)
		{
			continue;
		}
		
		check_state_map[item] = true;
		if (item_tags_buf.contains(item))
		{
			continue;
		}
		auto im = get_image(item);
		TianLiQtCommon_NearbyItemButton* new_pickable_tag = new TianLiQtCommon_NearbyItemButton(QString::fromStdString(item), im);
		PageTabMap_ScrollCardRect[1]->addWidget(new_pickable_tag);
		item_tags_buf[item] = Core.GetResource().GetImageBuffer("", "", "", item);
		item_button_buf[item] = new_pickable_tag;
	}
	for (auto& [item, state] : check_state_map)
	{
		auto is_find = item_tags_buf.contains(item);
		if (is_find)
		{
			continue;
		}
		item_tags_buf.erase(item);
		auto btn = item_button_buf[item];
		// delete
		btn->deleteLater();
		item_button_buf.erase(item);
		
		//LogInfo(item);
	}
	

	
	//for (int i = 0; i < 2; i++)
	//{
	//	QImage im;
	//	im.load(":/Test/resource/Test/Tex_0537_0.png");
	//	TianLiQtCommon_PickedItemButton* asd = new TianLiQtCommon_PickedItemButton(QString::number(i) + "号物品xxx", im);
	//	PageTabMap_ScrollCardRect[2]->addWidget(asd);
	//}
}

void GenshinImpact_TianLi::slot_show()
{
	LogInfo("显示主窗口");
	if (Core.GetTrack().GetResult().is_find_paimon)
	{
		if (main_bebind_widget == nullptr)
		{
			main_bebind_widget = new QWidget();
			RECT gi_client_rect = Core.GetTrack().GetResult().client_rect;
			main_bebind_widget->setGeometry(gi_client_rect.left, gi_client_rect.top, gi_client_rect.right - gi_client_rect.left, gi_client_rect.bottom - gi_client_rect.top);
			main_bebind_widget->setAttribute(Qt::WA_QuitOnClose, false);
			main_bebind_widget->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
			main_bebind_widget->setAttribute(Qt::WA_TranslucentBackground, true);

			LogInfo((QString("创建主窗口后的模糊覆盖 RECT：") + QString::number(gi_client_rect.left) + " " +  QString::number(gi_client_rect.top) + " " + QString::number(gi_client_rect.right) + " " +  QString::number(gi_client_rect.bottom)).toStdString().c_str());
			/*SetWindowLong((HWND)winId(), GWL_EXSTYLE, GetWindowLong((HWND)main_bebind_widget->winId(), GWL_EXSTYLE) |
				WS_EX_TRANSPARENT);*/
			TianLi::Utils::set_window_blur_bebind((HWND)main_bebind_widget->winId());

			// 设置显示时隐藏HUD

		}
		
		main_bebind_widget->show();
		main_bebind_widget->activateWindow();

	}
	this->showNormal();
	this->show();
	// 激活到最前面
	this->activateWindow();	
	is_visible = true;
}

void GenshinImpact_TianLi::slot_show_or_hide()
{
	static bool before_hud_state_square_map = false;
	static bool before_hud_state_circular_map = false;
	static bool before_hud_state_azimuth_bar_window = false;

	if (is_visible)
	{
		// 1. 隐藏主页面
		this->slot_hide();
		//// 2. 如果此前HUD为显示状态，则恢复显示HUD，否则不变
		//if (before_hud_state_square_map==false)
		//{
		//	hud_square_map->show();
		//}
		//if (before_hud_state_circular_map == false)
		//{
		//	hud_circular_map->show();
		//}
		//if (before_hud_state_azimuth_bar_window == false)
		//{
		//	hud_azimuth_bar_window->show();
		//}
	}
	else
	{
		// 1. 显示主界面
		this->slot_show();
		//// 2. 如果正在显示的HUD，记录其状态，并隐藏
		//if (hud_square_map->isHidden() == false)
		//{
		//	hud_square_map->hide();
		//	before_hud_state_square_map = true;
		//}
		//if (hud_circular_map->isHidden() == false)
		//{
		//	hud_circular_map->hide();
		//	before_hud_state_circular_map = true;
		//}
		//if (hud_azimuth_bar_window->isHidden() == false)
		//{
		//	hud_azimuth_bar_window->hide();
		//	before_hud_state_azimuth_bar_window = true;
		//}
	}
}

void GenshinImpact_TianLi::slot_hide()
{
	LogInfo("隐藏主窗口");
	if (main_bebind_widget != nullptr)
	{
		main_bebind_widget->hide();
	}

	this->showMinimized();
	is_visible = false;
	//this->hide();
}

void GenshinImpact_TianLi::pushButtonGroup_SelectItem(bool checked)
{
	if (checked == true)
	{
		// 自身sender
		QPushButton* button = qobject_cast<QPushButton*>(sender());
		// 获取 选中种类文字
		QString str = button->text();
		// 检查 选中种类文字 是否与之前 选中种类 一致

	// 选中种类
		QString selectedStr_Item;
			// 如果不一致 则更新 选中种类
			selectedStr_Item = str;

			ItemsVector itemsItemsVector;
			auto area = get_selected_area().toStdString();
			auto type = get_selected_type().toStdString();
			auto item = selectedStr_Item.toStdString();
			std::tuple<std::string, std::string, std::string> key = { area, type, item };
			int size = 0;
			{

				// 加载该种类下的物品
				Core.GetSqlite().ReadItems(area.c_str(), type.c_str(), item.c_str(), itemsItemsVector);
				// 如果读取到的数据是空的
				size = itemsItemsVector.size;
				if (size == 0)
				{
					return;
				}


				BadgeInfo::BadgeBlock legend_block;
				legend_block.name = itemsItemsVector[0].name;

					// 绘制半透明圆环
					if (legend_block.name != "传送锚点" && legend_block.name != "七天神像")
					{
						int resize = 49;
						int rect_w = 57;
						int rect_h = 64;
						int center_x = 28;
						int center_y = 28;

						legend_block.image = cv::Mat(rect_h, rect_w, CV_8UC4, cv::Scalar(0, 0, 0, 0));//
						cv::Mat image = Core.GetResource().GetImageBuffer("", "", "", itemsItemsVector[0].name).clone();

						int lt = (rect_w - resize) * 0.5;
						double scale = 1.0 * resize / max(image.rows, image.cols);
						cv::Size img_resize = cv::Size(scale * image.cols, scale * image.rows);
						cv::Mat image_roi = legend_block.image(cv::Rect(lt, lt, img_resize.width, img_resize.height));

						cv::resize(image, image_roi, img_resize);

						legend_block.image = TianLi::Utils::draw_object_border(legend_block.image);
						legend_block.center_pos.x = legend_block.image.cols / 2;
						legend_block.center_pos.y = legend_block.image.rows;
					}
					else
					{
						cv::Mat image = Core.GetResource().GetImageBuffer("", "", "", itemsItemsVector[0].name).clone();
						double scale = 48.0 / max(image.rows, image.cols);
						cv::Size img_resize = cv::Size(scale * image.cols, scale * image.rows);
						cv::resize(image, legend_block.image, img_resize);

						legend_block.center_pos.x = legend_block.image.cols / 2;
						legend_block.center_pos.y = legend_block.image.rows / 2;
					}

				for (int i = 0; i < itemsItemsVector.size; i++)
				{
					BadgeInfo::BadgeBlock::Badge legend;
					legend.x = itemsItemsVector[i].x;
					legend.y = itemsItemsVector[i].y;
					legend.z = itemsItemsVector[i].z;
					legend.message = itemsItemsVector[i].msg;

					legend_block.badge_list.push_back(legend);
				}
				CoreMap.badge_info.badge_block_list.insert({ key,legend_block });

			}

			if (item_button_checked_map.contains(key))
			{
				item_button_checked_map[key] = true;
			}
			else
			{
				item_button_checked_map.insert({ key ,true });
			}

			auto select_button = new TianLiQtCommon_SelectedItemButton(str, get_selected_type(), get_selected_area(), PageTabMap_ScrollCardRect[0]);
			// 创建按钮到 物品按钮QMap 中
			//pushButtonMap_Items.insert(str, button);

			object_button_group.addButton(select_button);
			object_button_index_map[{ get_selected_area().toStdString() + "-" + get_selected_type().toStdString() + "-" + selectedStr_Item.toStdString()}] = object_button_group.id(select_button);

			// 设置按钮父对象
			select_button->setParent(PageTabMap_ScrollCardRect[0]);
			PageTabMap_ScrollCardRect[0]->addWidget(select_button);
			
		
			connect(select_button, &TianLiQtCommon_SelectedItemButton::signal_double_click, this,&GenshinImpact_TianLi::slot_delete_object);
			
			// 显示按钮
			select_button->show();
			select_button->setProgressMaxNumber(size);

			// 强制重绘MapRect
			// 切换后要触发MapRect的强制重绘
			PageTabMap_MapRect->slot_force_update();
		
	}
	else
	{
		emit slot_delete_object();
	}
}

void GenshinImpact_TianLi::slot_delete_object()
{
	enum button_class
	{
		SelectedItemButton,
		TypeGroupButton,
		Button
	};
	button_class sender_class = button_class::Button;
	TianLiQtCommon_SelectedItemButton* button_ = qobject_cast<TianLiQtCommon_SelectedItemButton*>(sender());
	if (button_ != nullptr)
	{
		sender_class = button_class::SelectedItemButton;
	}
	TianLiQtCommon_TypeGroupButton* y = qobject_cast<TianLiQtCommon_TypeGroupButton*>(sender());
	if (y != nullptr)
	{
		sender_class = button_class::TypeGroupButton;
	}
	// 每个路径都要做三个工作
	// 1. 删除SelectItem按钮
	// 2. 取消TypeGroup按钮的点击状态
	// 3. 清空该点位数据
	switch (sender_class)
	{
	case button_class::SelectedItemButton:
	{
		TianLiQtCommon_SelectedItemButton* button = qobject_cast<TianLiQtCommon_SelectedItemButton*>(sender());
		auto item = button->item_name().toStdString();
		auto type = button->type_name().toStdString();
		auto area = button->area_name().toStdString();
		std::tuple<std::string, std::string, std::string> key = { area, type, item };

		// 1. 删除自身
		button->deleteLater();
		// 2. 取消Type按钮的状态
		QAbstractButton* item_button = nullptr;
		// 2.1 先找到该按钮
		// 2.1.1 先判断该按钮是否正在显示x
		if (get_selected_area() == button->area_name() && get_selected_type() == button->type_name())
		{
			// 2.1.2 如果正在显示，则从map中找到对应文字按钮的id
			int id = -1;
			if (item_button_index_map.contains(item))
			{
				id = item_button_index_map[item];
			}
			if (id != -1)
			{
				item_button = item_button_group.button(id);
			}
		}
		// 2.1.3 否则只需要修改map中的bool即可
		if (item_button_checked_map.contains(key))
		{
			item_button_checked_map[key] = false;
		}
		// 2.2 取消按钮状态
		if (item_button != nullptr)
		{
			item_button->setChecked(false);
		}
		// 2.3 删除点位数据
		if (CoreMap.badge_info.badge_block_list.contains(key))
		{
			CoreMap.badge_info.badge_block_list.erase(key);
		}

		// 切换后要触发MapRect的强制重绘
		PageTabMap_MapRect->slot_force_update();
		break;
	}
	case button_class::TypeGroupButton:
	{
		TianLiQtCommon_TypeGroupButton* button = qobject_cast<TianLiQtCommon_TypeGroupButton*>(sender());
		// 0. 能够点击，所以type按钮一定可见
		auto item = button->text().toStdString();
		auto type = get_selected_type().toStdString();
		auto area = get_selected_area().toStdString();
		std::tuple<std::string, std::string, std::string> key = { area, type, item };
		// 1. 删除Select按钮
		QAbstractButton* select_button = nullptr;
		// 1.1 先找到按钮
		auto key_split = area + "-" + type + "-" + item;
		if (object_button_index_map.contains(key_split))
		{
			int delete_id = object_button_index_map[key_split];
			select_button = object_button_group.button(delete_id);
		}
		// 1.2 删除按钮
		select_button->deleteLater();
		// 2. 清空本身选择和map中的bool
		button->setChecked(false);
		if (item_button_checked_map.contains(key))
		{
			item_button_checked_map[key] = false;
		}
		// 3. 删除点位数据
		if (CoreMap.badge_info.badge_block_list.contains(key))
		{
			CoreMap.badge_info.badge_block_list.erase(key);
		}
		// 切换后要触发MapRect的强制重绘
		PageTabMap_MapRect->slot_force_update();
		break;
	}
	default:
	{
		LogWarn("信号来源未设定执行方法，检查来源");
	}
	}
}
