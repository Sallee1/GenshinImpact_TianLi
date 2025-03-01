﻿#pragma once
#include "..\GenshinImpact_TianLi_Resource\GenshinImpact_TianLi_Resource.h"
#pragma comment(lib,"GenshinImpact_TianLi_Resource.lib")

#include "GenshinImpact_TianLi_Track.h"

#include "Capture/Capture.h"

enum GenshinWindowClass
{
	None,
	Unity,
	Obs,
	Qt,
};

const std::vector<std::pair<std::wstring,GenshinWindowClass>> GenshinWindowNameList =
{
	{L"原神",GenshinWindowClass::Unity},
	{L"云·原神", GenshinWindowClass::Qt} ,
	{L"窗口投影（源） - 云·原神",GenshinWindowClass::Obs},
	{L"원신", GenshinWindowClass::Unity},
	{L"Genshin Impact", GenshinWindowClass::Unity},
	{L"原神-调试", GenshinWindowClass::None}
};

//class Capture;
struct GenshinHandleConfig
{
	bool is_auto_find_genshin = true;
	bool is_force_used_no_alpha = false;
	HWND genshin_handle = nullptr;
	std::vector<std::pair<std::wstring, GenshinWindowClass>> genshin_window_name_list = GenshinWindowNameList;
	TianLi::Track::CaptureType capture_type = TianLi::Track::CaptureType::Bitblt;
};
struct GenshinHandle
{
	bool is_exist = false;
	HWND handle = nullptr;
	RECT rect = RECT();
	RECT rect_client = RECT();
	double scale = 1.0;
	bool is_exist_title_bar = true;
	cv::Size size_frame= cv::Size(1920,1080);
	cv::Rect rect_paimon_maybe;
	cv::Rect rect_paimon;
	cv::Rect rect_minimap_cailb_maybe;
	cv::Rect rect_minimap_cailb;
	cv::Rect rect_minimap_maybe;
	cv::Rect rect_minimap;
	cv::Rect rect_avatar_maybe;
	cv::Rect rect_avatar;
	cv::Rect rect_uid_maybe;
	cv::Rect rect_uid;
	cv::Rect rect_star_maybe;
	cv::Rect rect_star;
	cv::Rect rect_map_flag_maybe;
	cv::Rect rect_map_flag;
	cv::Rect rect_map_flag_icon_maybe;
	cv::Rect rect_map_flag_icon;
	cv::Rect rect_left_give_items_maybe;
	cv::Rect rect_left_give_items;
	cv::Rect rect_right_pick_items_maybe;
	cv::Rect rect_right_pick_items;
	cv::Rect rect_hp_maybe;
	cv::Rect rect_hp;
	GenshinHandleConfig config;
};
struct GenshinScreenConfig
{
	Capture* capture = nullptr;

	cv::Rect rect_paimon_maybe;
	cv::Rect rect_minimap_cailb_maybe;
	cv::Rect rect_minimap_maybe;
	
	bool is_used_alpha = true;

	bool is_handle_mode = false;

	cv::Rect rect_paimon;
	cv::Rect rect_minimap_cailb;
	cv::Rect rect_minimap;
};
struct GenshinScreen
{
	cv::Rect rect_client;

	cv::Mat img_screen;
	cv::Mat img_paimon_maybe;
	cv::Mat img_paimon;
	cv::Mat img_minimap_cailb_maybe;
	cv::Mat img_minimap_cailb;
	cv::Mat img_minimap_maybe;
	cv::Mat img_minimap;
	cv::Mat img_avatar_maybe;
	cv::Mat img_avatar;
	cv::Mat img_uid_maybe;
	cv::Mat img_uid;
	cv::Mat img_star_maybe;
	cv::Mat img_star;
	cv::Mat img_map_flag_maybe;
	cv::Mat img_map_flag;
	cv::Mat img_map_flag_icon_maybe;
	cv::Mat img_map_flag_icon;
	cv::Mat img_left_give_items_maybe;
	cv::Mat img_left_give_items;
	cv::Mat img_right_pick_items_maybe;
	cv::Mat img_right_pick_items;
	cv::Mat img_hp_maybe;
	cv::Mat img_hp;
	GenshinScreenConfig config;
};
struct GenshinPaimonConfig
{
	double check_match_paimon_params = 0.80;
	double check_match_paimon_params_no_alpha = 0.50;
};

struct GenshinPaimon
{
	bool is_visial = false;
	bool is_handle_mode = false;
	cv::Rect rect_paimon;
	GenshinPaimonConfig config;
};
struct GenshinMinimapCailbConfig
{
	bool is_need_find = true;
	double check_match_minimap_cailb_params = 0.85;
	double check_match_minimap_cailb_params_no_alpha = 0.50;
};

struct GenshinMinimapCailb
{
	bool is_visial = false;
	cv::Rect rect_minimap_cailb;
	GenshinMinimapCailbConfig config;
};
struct GenshinMinimapConfig
{
	bool is_need_cailb = true;
	bool is_find_paimon = false;
	
};

struct GenshinMinimap
{
	bool is_cailb = false;
	cv::Mat img_minimap;
	cv::Rect rect_minimap;
	cv::Point point_minimap_center;
	cv::Rect rect_avatar;
	cv::Mat img_avatar;
	cv::Rect rect_viewer;
	cv::Mat img_viewer;
	cv::Rect rect_stars;
	cv::Mat img_stars;
	
	GenshinMinimapConfig config;
};
struct GenshinAvatarDirectionConfig
{

};
struct GenshinAvatarDirection
{
	double angle = 0;
	GenshinAvatarDirectionConfig config;
};

struct GenshinAvatarPositionConfig
{
	bool is_init_finish = false;
	bool is_lock_minimap_rotation = true;
	double minimap_rotation = 0;
	bool is_con = false;
	bool is_cov = false;
	bool is_on_city = false;
};
struct GenshinAvatarPosition
{
	cv::Point2d position;
	GenshinAvatarPositionConfig config;
};

struct GenshinViewerDirectionConfig
{
	bool is_lock_minimap_rotation = true;
	double minimap_rotation = 0;
};

struct GenshinViewerDirection
{
	double angle = 0;
	GenshinViewerDirectionConfig config;
};

struct GenshinMinimapDirectionConfig
{
	bool  is_defalut = true;
	bool is_skip = true;
};

struct GenshinMinimapDirection
{
	double angle = 0;
	GenshinMinimapDirectionConfig config;
};
struct GenshinStarsConfig
{
	double check_match_star_params = 0.85;
};
struct GenshinStars
{
	bool is_find = false;
	std::vector<cv::Point> points_star;
	GenshinStarsConfig config;
};

struct GenshinTagflagsConfig
{
	double check_match_star_params = 0.85;
};
struct GenshinTagflags
{
	bool is_find = false;
	std::vector<cv::Point> points_star;
	GenshinTagflagsConfig config;
};

struct GenshinUIDConfig
{
	double check_match_uid_params = 0.85;
};
struct GenshinUID
{
	int uid = 0;
	GenshinUIDConfig config;
};

struct GenshinPickableItemsConfig
{
	
};
struct GenshinPickableItems
{
	std::vector<cv::Mat> item_imgs;
	std::vector<std::string> item_tags;
	GenshinPickableItemsConfig config;
};

class Filter
{
public:
	Filter() {}
	~Filter() {}
public:
	virtual cv::Point2d filterting(const cv::Point2d& pos) = 0;
	virtual cv::Point2d re_init_filterting(const cv::Point2d& pos) = 0;
public:
	enum FilterType
	{
		Kalman,
		Smooth,
		Untouched,
		Unknown
	};
public:
	FilterType type = FilterType::Unknown;
};

class Kalman : public Filter
{
public:
	Kalman();
	~Kalman();
public:
	virtual cv::Point2d filterting(const cv::Point2d& pos) override;
	virtual cv::Point2d re_init_filterting(const cv::Point2d& pos) override;
private:
	int stateNum = 4;
	int measureNum = 2;

	cv::KalmanFilter KF;
	cv::Mat state; /* (phi, delta_phi) */
	cv::Mat processNoise;
	cv::Mat measurement;
};


class SurfMatch
{
	cv::Mat _mapMat;
	cv::Mat _miniMapMat;
	cv::Mat _minimap_mat_mask;
	cv::Mat _miniMapLastMat;

	double ratio_thresh = 0.66;
	double render_map_scale = 1.3;//1.3;
	//int someSizeR = 106;
	double MatchMatScale = 2.0;





	cv::Point2d hisP[3];

	cv::Point2d pos;
public:
	SurfMatch();
	~SurfMatch();

public:
	Kalman fiting;
public:
	//int hessian_threshold = 1;
	//int octaves = 1;
	//int octave_layers = 1;
	int hessian_threshold = 10;
	int octaves = 4;
	int octave_layers = 6;

	cv::Ptr<cv::xfeatures2d::SURF> detector, detectorSomeMap;
	std::vector<cv::KeyPoint> Kp_MinMap, Kp_Map, Kp_SomeMap;
	cv::Mat Dp_MinMap, Dp_Map, Dp_SomeMap;

	bool isInit = false;
	bool isContinuity = false;
	bool isOnCity = false;

	void setMap(cv::Mat gi_map);
	void setMiniMap(cv::Mat miniMapMat);

	void Init();
	void Init(std::vector<cv::KeyPoint>& gi_map_keypoints, cv::Mat& gi_map_descriptors);
	void match();

	cv::Point2d match_continuity(bool& calc_continuity_is_faile);
	cv::Point2d match_continuity_on_city(bool& calc_continuity_is_faile);
	cv::Point2d match_continuity_not_on_city(bool& calc_continuity_is_faile);

	cv::Point2d match_no_continuity(bool& calc_is_faile);

	cv::Point2d SURFMatch(cv::Mat minMapMat);
	cv::Point2d getLocalPos();
	bool getIsContinuity();
private:
	double dis(cv::Point2d& p);
	cv::Point2d SPC(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy);
	//Point SPC2(vector<double> lisx, double sumx, vector<double> lisy, double sumy);
	double var(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy);
};

void draw_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<cv::DMatch>& good_matches);

void calc_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, double render_map_scale, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy);


#ifdef _DEBUG
#define log(str) std::cout << __FUNCTION__ <<":"<< __LINE__<<" ||=>> " << str <<'\n';
#else
#define log(str)
#endif