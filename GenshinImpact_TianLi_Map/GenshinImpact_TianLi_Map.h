#pragma once
#ifndef _LIB
#ifdef GENSHINIMPACTTIANLIMAP_EXPORTS
#define GENSHINIMPACTTIANLIMAP_API __declspec(dllexport)
#else
//#define GENSHINIMPACTTIANLIMAP_API __declspec(dllimport)
#define GENSHINIMPACTTIANLIMAP_API /* No thing */
#endif

#define APICALL __stdcall
#ifndef DLLAPI
#define DLLAPI GENSHINIMPACTTIANLIMAP_API
#endif // DLLAPI

#else
#ifndef DLLAPI
#define DLLAPI
#endif // DLLAPI
#endif

#include <map>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

struct MapBlock 
{
	
};
struct AvatarInfo
{
	double x= 2000;
	double y= 2937;
	double z=0; // 根据地图所在位置，大致判定高度区间
	double a=0;
	double r=0;
};
struct MapInfo
{
	cv::Point map_pos;
	int center_x=0;
	int center_y=0;
	int viewer_width=0;
	int viewer_height=0;
	cv::Rect map_rect;
	cv::Rect map_rect_tianli;
	double scale_form_gimap=1.0;
	int scale_width=0;
	int scale_height=0;
	bool is_overlay = false;
	bool is_show_map = true;
};
struct BadgeInfo
{
	struct BadgeBlock
	{
		std::string name;
		cv::Mat image;
		cv::Point center_pos;
		struct Badge
		{
			std::string message;
			std::string picture_url;
			
			double x=0;
			double y=0;
			double z=0;
					
			int map_id=0;
			bool operator& (const cv::Rect& rect)
			{
				return rect.contains(cv::Point2d(x,y));			
			}
		};
		std::vector<Badge> badge_list;
	};
	std::map<std::tuple<std::string,std::string,std::string>,BadgeBlock> badge_block_list;
};

struct MapShowObjects
{
	std::vector<BadgeInfo::BadgeBlock> types;

};
//
//class object
//{
//public:
//	double x;
//	double y;
//	int id;
//	
//};
//class objects
//{
//public:
//	objects filte(double x, double y, double r = 200);
//	
//	object& at(int index);
//	int size();
//	
//	class iterator
//	{
//	public:
//		iterator(objects* objects, int index)
//		{
//			this->objects = objects;
//			this->index = index;
//		}
//		iterator& operator++()
//		{
//			index++;
//			return *this;
//		}
//		iterator operator++(int)
//		{
//			iterator tmp = *this;
//			index++;
//			return tmp;
//		}
//		bool operator==(const iterator& rhs)
//		{
//			return index == rhs.index;
//		}
//		bool operator!=(const iterator& rhs)
//		{
//			return index != rhs.index;
//		}
//		object& operator*()
//		{
//			return objects->at(index);
//		}
//		object* operator->()
//		{
//			return &objects->at(index);
//		}
//	private:
//		objects* objects;
//		int index;
//	};
//	// ʵ��for����
//	objects::iterator begin()
//	{
//		return objects::iterator(this, 0);
//	}
//	objects::iterator end()
//	{
//		return objects::iterator(this, this->size());
//	}
//	
//};
namespace TianLi
{
	class object
	{
	public:
		double x;
		double y;
		int id;
	};
	class objects
	{
		object* _objects;
		int _size;
	public:
		objects& filte(double x, double y, double r)
		{
			return *this;
		}
		object& at(int index)
		{
			return _objects[index];
		}
		int size()
		{
			return _size;
		}
		class iterator
		{
		public:
			iterator(objects* objects, int index)
			{
				this->objects = objects;
				this->index = index;
			}
			iterator& operator++()
			{
				index++;
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp = *this;
				index++;
				return tmp;
			}
			bool operator==(const iterator& rhs)
			{
				return index == rhs.index;
			}
			bool operator!=(const iterator& rhs)
			{
				return index != rhs.index;
			}
			object& operator*()
			{
				return objects->at(index);
			}
			object* operator->()
			{
				return &objects->at(index);
			}
		private:
			objects* objects;
			int index;
		};
		// ʵ��for����
		objects::iterator begin()
		{
			return objects::iterator(this, 0);
		}
		objects::iterator end()
		{
			return objects::iterator(this, this->size());
		}
	};
	
}

#include "..\GenshinImpact_TianLi_Core\GenshinImpact_TianLi_Core.h"
#pragma comment(lib,"GenshinImpact_TianLi_Core.lib")
#include "..\GenshinImpact_TianLi_Data\GenshinImpact_TianLi_Data.h"
#pragma comment(lib,"GenshinImpact_TianLi_Data.lib")

class GenshinImpact_TianLi_Core;
class GenshinImpact_TianLi_Data;
class DLLAPI GenshinImpact_TianLi_Map
{
	GenshinImpact_TianLi_Core* core;
	GenshinImpact_TianLi_Data* data;
	
	GenshinImpact_TianLi_Map();
public:
	~GenshinImpact_TianLi_Map();
	static GenshinImpact_TianLi_Map& GetInstance();
	
//public:
//	GenshinImpact_TianLi_Map();
//	~GenshinImpact_TianLi_Map();
	
public:
	AvatarInfo avatar_info;
	MapInfo map_info;
	BadgeInfo badge_info;
	MapShowObjects map_show_objects;
	//cv::Mat viewer_mat;
	//cv::Mat viewer_draw_badge_mat;
	GenshinImpact_TianLi_Core& Core() { return *core; }
	GenshinImpact_TianLi_Data& Data() { return *data; }
	
	void render_overlay(cv::Mat& map);
	void render_legend(cv::Mat& map);
	
	BadgeInfo search(const char* country, const char* type, const char* item);
	
	TianLi::objects& search(const char* name, double x = 0, double y = 0, double r = 0);
public:
	// std::function<cv::Mat(std::string area, std::string type, std::string item, std::string object)> get_image;
	cv::Mat get_image_tag(const std::string& area, const std::string& type, const std::string& item, const std::string& object);
};

#define CoreMap GenshinImpact_TianLi_Map::GetInstance()
#define Core GenshinImpact_TianLi_Map::GetInstance().Core()
#define Data GenshinImpact_TianLi_Map::GetInstance().Data()
