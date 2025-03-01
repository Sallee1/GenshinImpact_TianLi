#include "pch.h"
#include "core_match_thread.h"

#include <thread>
#include <Windows.h>

SurfMatch::SurfMatch()
{
	hisP[0] = cv::Point();
	hisP[1] = cv::Point();
	hisP[2] = cv::Point();
}

SurfMatch::~SurfMatch()
{
}

void SurfMatch::setMap(cv::Mat mapMat)
{
	_mapMat = mapMat;
}

void SurfMatch::setMinMap(cv::Mat minMapMat)
{
	_minMapMat = minMapMat;
}

void SurfMatch::Init()
{
	if (isInit)return;
	detector = cv::xfeatures2d::SURF::create(minHessian);
	detector->detectAndCompute(_mapMat, cv::noArray(), Kp_Map, Dp_Map);
	isInit = true;
}

void SurfMatch::Init(std::vector<cv::KeyPoint>& gi_map_keypoints, cv::Mat& gi_map_descriptors)
{
	if (isInit)return;
	Kp_Map = std::move(gi_map_keypoints);
	Dp_Map = std::move(gi_map_descriptors);
	isInit = true;
}

void SurfMatch::match()
{
	cv::Point2d dp1 = hisP[1] - hisP[0];
	cv::Point2d dp2 = hisP[2] - hisP[1];
	
 	bool calc_is_faile = false;

	isContinuity = false;
	
	//角色移动连续性判断
	if (((dis(dp1) + dis(dp2)) < 2000) && (hisP[2].x > someSizeR && hisP[2].x < _mapMat.cols - someSizeR && hisP[2].y>someSizeR && hisP[2].y < _mapMat.rows - someSizeR))
	{
		isContinuity = true;
	}
	
	if (isContinuity)
	{
		bool calc_continuity_is_faile = false;
		
		pos = match_continuity(calc_continuity_is_faile);
		
		if (calc_continuity_is_faile)
		{
			isContinuity = false;
		}
	}
	
	if (!isContinuity)
	{
		pos = match_no_continuity(calc_is_faile);
	}
	
	if (calc_is_faile)
	{
		return;
	}

	hisP[0] = hisP[1];
	hisP[1] = hisP[2];
	hisP[2] = pos;
	
}

cv::Point2d  SurfMatch::match_continuity(bool& calc_continuity_is_faile)
{
	cv::Point2d pos_continuity;

	if (isOnCity == false)
	{
		pos_continuity = match_continuity_not_on_city(calc_continuity_is_faile);
	}
	else
	{
		pos_continuity = match_continuity_on_city(calc_continuity_is_faile);
	}

	return pos_continuity;
}

cv::Point2d SurfMatch::match_continuity_on_city(bool& calc_continuity_is_faile)
{
	cv::Point2d pos_on_city;

	cv::Mat img_scene(_mapMat);
	cv::Mat img_object(_minMapMat(cv::Rect(30, 30, _minMapMat.cols - 60, _minMapMat.rows - 60)));
	
	//在城镇中
		/***********************/
		//重新从完整中地图取出角色周围部分地图
	cv::Mat someMap(img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
	cv::Mat minMap(img_object);

	resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));

	detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
	detectorSomeMap->detectAndCompute(minMap, cv::noArray(), Kp_MinMap, Dp_MinMap);

	if (Kp_SomeMap.size() <= 2 || Kp_MinMap.size() <= 2)
	{
		calc_continuity_is_faile = true;
		return pos_on_city;
	}

	cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_mTmp;

	matcherTmp->knnMatch(Dp_MinMap, Dp_SomeMap, KNN_mTmp, 2);
	std::vector<double> lisx;
	std::vector<double> lisy;
	double sumx = 0;
	double sumy = 0;

	calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MinMap, KNN_mTmp, ratio_thresh, 0.8667, lisx, lisy, sumx, sumy);

	if (std::max(lisx.size(), lisy.size()) <= 4)
	{
		calc_continuity_is_faile = true;
		return pos_on_city;
	}

	if (std::min(lisx.size(), lisy.size()) >= 10)
	{
		isOnCity = true;
	}
	else
	{
		isOnCity = false;
	}

	cv::Point2d pos_continuity_on_city = SPC(lisx, sumx, lisy, sumy);

	pos_continuity_on_city.x = (pos_continuity_on_city.x - someMap.cols / 2.0) / 2.0;
	pos_continuity_on_city.y = (pos_continuity_on_city.y - someMap.rows / 2.0) / 2.0;

	pos_on_city = cv::Point2d(pos_continuity_on_city.x + hisP[2].x, pos_continuity_on_city.y + hisP[2].y);

	return pos_on_city;
}

cv::Point2d SurfMatch::match_continuity_not_on_city(bool& calc_continuity_is_faile)
{
	cv::Point2d pos_not_on_city;
	
	cv::Mat img_scene(_mapMat);
	cv::Mat img_object(_minMapMat(cv::Rect(30, 30, _minMapMat.cols - 60, _minMapMat.rows - 60)));
	
	//不在城镇中时
	cv::Mat someMap(img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)));
	cv::Mat minMap(img_object);

	detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
	detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
	detectorSomeMap->detectAndCompute(minMap, cv::noArray(), Kp_MinMap, Dp_MinMap);

	// 如果搜索范围内可识别特征点数量为0，则认为计算失败
	if (Kp_SomeMap.size() == 0 || Kp_MinMap.size() == 0)
	{
		calc_continuity_is_faile = true;
		return pos_not_on_city;
	}
	cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_mTmp;

	matcherTmp->knnMatch(Dp_MinMap, Dp_SomeMap, KNN_mTmp, 2);
	std::vector<double> lisx;
	std::vector<double> lisy;
	double sumx = 0;
	double sumy = 0;

	calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MinMap, KNN_mTmp, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);

	// 如果范围内最佳匹配特征点对数量大于4，则认为不可能处于城镇之中，位于城镇之外
	if (std::min(lisx.size(), lisy.size()) > 4)
	{
		isOnCity = false;

		cv::Point2d p = SPC(lisx, sumx, lisy, sumy);
		pos_not_on_city = cv::Point2d(p.x + hisP[2].x - someSizeR, p.y + hisP[2].y - someSizeR);
	}
	else
	{

		//有可能处于城镇中

		/***********************/
		//重新从完整中地图取出角色周围部分地图
		img_scene(cv::Rect(static_cast<int>(hisP[2].x - someSizeR), static_cast<int>(hisP[2].y - someSizeR), someSizeR * 2, someSizeR * 2)).copyTo(someMap);
		//Mat minMap(img_object);

		resize(someMap, someMap, cv::Size(someSizeR * 4, someSizeR * 4));
		//resize(minMap, minMap, Size(), MatchMatScale, MatchMatScale, 1);

		detectorSomeMap = cv::xfeatures2d::SURF::create(minHessian);
		detectorSomeMap->detectAndCompute(someMap, cv::noArray(), Kp_SomeMap, Dp_SomeMap);
		//detectorSomeMap->detectAndCompute(minMap, noArray(), Kp_MinMap, Dp_MinMap);
		if (Kp_SomeMap.size() == 0 || Kp_MinMap.size() == 0)
		{
			calc_continuity_is_faile = true;
			return pos_not_on_city;
		}

		cv::Ptr<cv::DescriptorMatcher> matcherTmp = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
		std::vector< std::vector<cv::DMatch> > KNN_mTmp;

		matcherTmp->knnMatch(Dp_MinMap, Dp_SomeMap, KNN_mTmp, 2);

		std::vector<double> list_x_on_city;
		std::vector<double> list_y_on_city;
		double sum_x_on_city = 0;
		double sum_y_on_city = 0;

		calc_good_matches(someMap, Kp_SomeMap, img_object, Kp_MinMap, KNN_mTmp, ratio_thresh, 0.8667, list_x_on_city, list_y_on_city, sum_x_on_city, sum_y_on_city);

		if (std::min(list_x_on_city.size(), list_y_on_city.size()) <= 4)
		{
			calc_continuity_is_faile = true;
			return pos_not_on_city;
		}

		if (std::min(list_x_on_city.size(), list_y_on_city.size()) >= 10)
		{
			isOnCity = true;
		}
		else
		{
			isOnCity = false;
		}

		cv::Point2d p = SPC(list_x_on_city, sum_x_on_city, list_y_on_city, sum_y_on_city);

		double x = (p.x - someMap.cols / 2.0) / 2.0;
		double y = (p.y - someMap.rows / 2.0) / 2.0;

		pos_not_on_city = cv::Point2d(x + hisP[2].x, y + hisP[2].y);

	}
	
	return pos_not_on_city;
}

cv::Point2d SurfMatch::match_no_continuity(bool& calc_is_faile)
{
	cv::Point2d pos_continuity_no;
	
	// TODO: 可优化为static
	cv::Mat img_scene(_mapMat);
	cv::Mat img_object(_minMapMat(cv::Rect(30, 30, _minMapMat.cols - 60, _minMapMat.rows - 60)));
	
	detector->detectAndCompute(img_object, cv::noArray(), Kp_MinMap, Dp_MinMap);

	if (Kp_MinMap.size() == 0)
	{
		calc_is_faile = true;
		return pos_continuity_no;
	}
	
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<cv::DMatch> > KNN_m;

	matcher->knnMatch(Dp_MinMap, Dp_Map, KNN_m, 2);

	std::vector<double> lisx;
	std::vector<double> lisy;
	double sumx = 0;
	double sumy = 0;

	calc_good_matches(img_scene, Kp_Map, img_object, Kp_MinMap, KNN_m, ratio_thresh, mapScale, lisx, lisy, sumx, sumy);

	if (std::min(lisx.size(), lisy.size()) == 0)
	{
		calc_is_faile = true;
		return pos_continuity_no;
	}
	
	pos_continuity_no = SPC(lisx, sumx, lisy, sumy);
	
	return pos_continuity_no;
}


void calc_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<std::vector<cv::DMatch>>& KNN_m, double ratio_thresh, double mapScale, std::vector<double>& lisx, std::vector<double>& lisy, double& sumx, double& sumy)
{
#ifdef _DEBUG
	std::vector<cv::DMatch> good_matches;
#endif
	for (size_t i = 0; i < KNN_m.size(); i++)
	{
		if (KNN_m[i][0].distance < ratio_thresh * KNN_m[i][1].distance)
		{
#ifdef _DEBUG
			good_matches.push_back(KNN_m[i][0]);
#endif
			if (KNN_m[i][0].queryIdx >= keypoint_object.size())
			{
				continue;
			}
			lisx.push_back(((img_object.cols / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.x) * mapScale + keypoint_scene[KNN_m[i][0].trainIdx].pt.x));
			lisy.push_back(((img_object.rows / 2.0 - keypoint_object[KNN_m[i][0].queryIdx].pt.y) * mapScale + keypoint_scene[KNN_m[i][0].trainIdx].pt.y));
			sumx += lisx.back();
			sumy += lisy.back();
		}
	}
#ifdef _DEBUG
	draw_good_matches(img_scene, keypoint_scene, img_object, keypoint_object, good_matches);
#endif
}


void draw_good_matches(cv::Mat& img_scene, std::vector<cv::KeyPoint> keypoint_scene, cv::Mat& img_object, std::vector<cv::KeyPoint> keypoint_object, std::vector<cv::DMatch>& good_matches)
{
	cv::Mat img_matches, imgmap, imgminmap;
	drawKeypoints(img_scene, keypoint_scene, imgmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(img_object, keypoint_object, imgminmap, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	drawMatches(img_object, keypoint_object, img_scene, keypoint_scene, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

cv::Point2d SurfMatch::SURFMatch(cv::Mat minMapMat)
{
	return cv::Point2d();
}

cv::Point2d SurfMatch::getLocalPos()
{
	return pos;
}

bool SurfMatch::getIsContinuity()
{
	return isContinuity;
}

double SurfMatch::dis(cv::Point2d& p)
{
	return sqrt(p.x * p.x + p.y * p.y);
}

cv::Point2d SurfMatch::SPC(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy)
{
	//这个剔除异常点算法
	//回头要改
	cv::Point2d mpos;
	double meanx = sumx / lisx.size(); //均值
	double meany = sumy / lisy.size(); //均值
	double x = meanx;
	double y = meany;
	if (std::min(lisx.size(), lisy.size()) > 3)
	{
		double accumx = 0.0;
		double accumy = 0.0;
		for (int i = 0; i < std::min(lisx.size(), lisy.size()); i++)
		{
			accumx += (lisx[i] - meanx) * (lisx[i] - meanx);
			accumy += (lisy[i] - meany) * (lisy[i] - meany);
		}

		double stdevx = sqrt(accumx / (lisx.size() - 1)); //标准差
		double stdevy = sqrt(accumy / (lisy.size() - 1)); //标准差

		sumx = 0;
		sumy = 0;
		double numx = 0;
		double numy = 0;
		for (int i = 0; i < std::min(lisx.size(), lisy.size()); i++)
		{
			if (abs(lisx[i] - meanx) < 1 * stdevx)
			{
				sumx += lisx[i];
				numx++;
			}

			if (abs(lisy[i] - meany) < 1 * stdevy)
			{
				sumy += lisy[i];
				numy++;
			}
		}
		double xx = sumx / numx;
		double yy = sumy / numy;
		mpos = cv::Point2d(xx, yy);
	}
	else
	{
		mpos = cv::Point2d(x, y);
	}
	return mpos;
}

double SurfMatch::var(std::vector<double> lisx, double sumx, std::vector<double> lisy, double sumy)
{
	double accumx = 0.0;
	double accumy = 0.0;
	for (int i = 0; i < std::min(lisx.size(), lisy.size()); i++)
	{
		accumx = (lisx[i] - sumx) * (lisx[i] - sumx);
		accumy = (lisy[i] - sumy) * (lisy[i] - sumy);
	}
	double stdevx = sqrt(accumx / (lisx.size() - 1));
	double stdevy = sqrt(accumy / (lisy.size() - 1));

	return sqrt(stdevx * stdevx + stdevy * stdevy);
}

void ATM_TM_TemplatePaimon::setPaimonTemplate(cv::Mat paimonTemplateMat)
{
	_paimonTemplate = paimonTemplateMat;
}

void ATM_TM_TemplatePaimon::setPaimonMat(cv::Mat paimonMat)
{
	_paimonMat = paimonMat;
}

void ATM_TM_TemplatePaimon::match()
{
	cv::Mat tmp;
	matchTemplate(_paimonTemplate, _paimonMat, tmp, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	//寻找最佳匹配位置
	minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	std::cout << "Match Template MinVal & MaxVal" << minVal << " , " << maxVal << std::endl;
#endif
	if (minVal < 0.51 || maxVal == 1)
	{
		isPaimonVisible = false;
	}
	else
	{
		isPaimonVisible = true;
	}
}

bool ATM_TM_TemplatePaimon::getPaimonVisible()
{
	return isPaimonVisible;
}

void ATM_TM_ORBAvatar::setAvatarTemplate(cv::Mat avatarTemplateMat)
{
	_avatarTemplate = avatarTemplateMat;
}

void ATM_TM_ORBAvatar::setAvatarMat(cv::Mat avatarMat)
{
	_avatarMat = avatarMat;
}

void ATM_TM_ORBAvatar::Init()
{
	if (isInit)return;

	isInit = true;
}

bool GreaterSort(cv::DMatch a, cv::DMatch b)
{
	return (a.distance > b.distance);
}

void ATM_TM_ORBAvatar::match()
{
	cv::Mat giAvatarRef = _avatarMat;

	cv::resize(giAvatarRef, giAvatarRef, cv::Size(), 2, 2);
	std::vector<cv::Mat> lis;
	cv::split(giAvatarRef, lis);

	threshold(lis[0], gray0, 240, 255, cv::THRESH_BINARY);
	threshold(lis[1], gray1, 212, 255, cv::THRESH_BINARY);
	threshold(lis[2], gray2, 25, 255, cv::THRESH_BINARY_INV);


	bitwise_and(gray1, gray2, and12, gray0);
	resize(and12, and12, cv::Size(), 2, 2, 3);
	Canny(and12, and12, 20, 3 * 20, 3);
	circle(and12, cv::Point(cvCeil(and12.cols / 2), cvCeil(and12.rows / 2)), cvCeil(and12.cols / 4.5), cv::Scalar(0, 0, 0), -1);

	dilate(and12, and12, dilate_element);
	erode(and12, and12, erode_element);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarcy;

	findContours(and12, contours, hierarcy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<cv::Rect> boundRect(contours.size());  //定义外接矩形集合
	//std::vector<RotatedRect> box(contours.size()); //定义最小外接矩形集合

	cv::Point2f rect[4];

	std::vector<cv::Point2d> AvatarKeyPoint;
	double AvatarKeyPointLine[3] = { 0 };
	std::vector<cv::Point2f> AvatarKeyLine;
	cv::Point2f KeyLine;

	if (contours.size() != 3)
	{
		return;
	}

	for (int i = 0; i < 3; i++)
	{
		//box[i] = minAreaRect(Mat(contours[i]));  //计算每个轮廓最小外接矩形
		boundRect[i] = cv::boundingRect(cv::Mat(contours[i]));
		AvatarKeyPoint.push_back(cv::Point(cvRound(boundRect[i].x + boundRect[i].width / 2), cvRound(boundRect[i].y + boundRect[i].height / 2)));
	}

	AvatarKeyPointLine[0] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
	AvatarKeyPointLine[1] = dis(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
	AvatarKeyPointLine[2] = dis(AvatarKeyPoint[1] - AvatarKeyPoint[0]);



	if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[2] && AvatarKeyPointLine[1] >= AvatarKeyPointLine[2])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[1]);
		AvatarKeyLine.push_back(AvatarKeyPoint[2] - AvatarKeyPoint[0]);
	}
	if (AvatarKeyPointLine[0] >= AvatarKeyPointLine[1] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[1])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[0]);
		AvatarKeyLine.push_back(AvatarKeyPoint[1] - AvatarKeyPoint[2]);
	}
	if (AvatarKeyPointLine[1] >= AvatarKeyPointLine[0] && AvatarKeyPointLine[2] >= AvatarKeyPointLine[0])
	{
		AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[1]);
		AvatarKeyLine.push_back(AvatarKeyPoint[0] - AvatarKeyPoint[2]);
	}

	AvatarKeyLine = Vector2UnitVector(AvatarKeyLine);
	KeyLine = AvatarKeyLine[0] + AvatarKeyLine[1];
	rotationAngle = Line2Angle(KeyLine);
}

double ATM_TM_ORBAvatar::getRotationAngle()
{
	return rotationAngle;
}

double ATM_TM_ORBAvatar::dis(cv::Point p)
{
	return sqrt(p.x * p.x + p.y * p.y);
}
std::vector<cv::Point2f> ATM_TM_ORBAvatar::Vector2UnitVector(std::vector<cv::Point2f> pLis)
{
	double length = 1;
	std::vector<cv::Point2f> res;
	for (int i = 0; i < pLis.size(); i++)
	{
		length = sqrt(pLis[i].x * pLis[i].x + pLis[i].y * pLis[i].y);
		res.push_back(cv::Point2f((float)(pLis[i].x / length), (float)(pLis[i].y / length)));
	}
	return res;
}

double ATM_TM_ORBAvatar::Line2Angle(cv::Point2f p)
{
	const double rad2degScale = 180 / CV_PI;
	double res = atan2(-p.y, p.x) * rad2degScale;
	res = res - 90; //从屏幕空间左侧水平线为0度转到竖直向上为0度
	return res;
}

//void ATM_TM_Thread::run()
//{
//	while (isExitThread == false)
//	{
//		if (isRunWork && (*ptr) != nullptr)
//		{
//			ptr(workInput);
//			isRunWork = false;
//			isEndWork = true;
//		}
//		else
//		{
//			std::this_thread::sleep_for(std::chrono::milliseconds(1));
//		}
//	}
//}
//
//ATM_TM_Thread::ATM_TM_Thread()
//{
//	tLoopWork = new thread(&ATM_TM_Thread::run, this);
//}
//
//ATM_TM_Thread::~ATM_TM_Thread()
//{
//	if (tLoopWork != nullptr)
//	{
//		isExitThread = true;
//		tLoopWork->join();
//		delete tLoopWork;
//	}
//}
//
//ATM_TM_Thread::ATM_TM_Thread(void(*funPtr)(Mat &inMat))
//{
//	setFunction(funPtr);
//	tLoopWork = new thread(&ATM_TM_Thread::run, this);
//}
//
//void ATM_TM_Thread::setFunction(void(*funPtr)(Mat &inMat))
//{
//	ptr = funPtr;
//	isExistFunction = true;
//}
//
//void ATM_TM_Thread::start(Mat & inMat)
//{
//	if (isExistFunction == false)
//	{
//		throw"Not Found Work Function";
//	}
//	workInput = inMat;
//	isRunWork = true;
//	isEndWork = false;
//}
//
//bool ATM_TM_Thread::isEnd()
//{
//	return isEndWork;
//}

int ATM_TM_TemplateUID::getMaxID(double lis[], int len)
{
	int maxId = 0;
	for (int i = 1; i < len; i++)
	{
		if (lis[i] > lis[maxId])
		{
			maxId = i;
		}
	}
	return maxId;
}

void ATM_TM_TemplateUID::Init()
{
	if (isInit)return;

	isInit = true;
}

void ATM_TM_TemplateUID::setUIDTemplate(cv::Mat* uidTemplateMat)
{
	for (int i = 0; i < 10; i++)
	{
		uidTemplateMat[i].copyTo(giNumUID.n[i]);
	}
	uidTemplateMat[10].copyTo(giNumUID.UID);

}

void ATM_TM_TemplateUID::setUIDMat(cv::Mat uidMat)
{
	if (uidMat.channels() == 4)
	{
		uidMat.copyTo(_uidMat);
	}
	else
	{
		cvtColor(uidMat, _uidMat, cv::COLOR_RGB2RGBA);
	}
}

/// <summary>
/// 识别UID
/// 只针对键盘模式下的1920x1080分辨率画面适用
/// 对于其他分辨率以及手柄模式，需要在传参前进行一定的缩放
/// 
/// 前置变量
/// giNumUID : struct{ cv::Mat n[10]; cv::Mat UID; }
/// _uidMat  : cv::Mat
/// 输出变量
/// uid      : int
/// </summary>
void ATM_TM_TemplateUID::match()
{
	static cv::Mat checkUID = giNumUID.UID;
	
	int bitCount = 1;
	cv::Mat Roi(_uidMat);

	cv::Mat match_result;
	matchTemplate(Roi, checkUID, match_result, cv::TM_CCOEFF_NORMED);

	double minVal_uid_, maxVal_uid_;
	cv::Point minLoc_uid_, maxLoc_uid_;
	
	//寻找最佳匹配位置
	cv::minMaxLoc(match_result, &minVal_uid_, &maxVal_uid_, &minLoc_uid_, &maxLoc_uid_);
	if (maxVal_uid_ > 0.75)
	{
		int x_uid_i = maxLoc_uid_.x + checkUID.cols + 7;
		int y_uid_i = 0;
		double maxVal_i_list[10] = { 0 };
		int x_i_list[10] = { 0 };
		for (int uid_p = 8; uid_p >= 0; uid_p--)
		{
			_NumBit[uid_p] = 0;
			for (int i = 0; i < giNumUID.max; i++)
			{
				// 180-46/9->140/9->16->16*9=90+54=144
				// 此处的计算细节如上
				cv::Rect r(x_uid_i, y_uid_i, giNumUID.n[i].cols + 2, giNumUID.n[i].rows);
				if (x_uid_i + r.width > _uidMat.cols)
				{
					r = cv::Rect(_uidMat.cols - giNumUID.n[i].cols - 2, 0, giNumUID.n[i].cols + 2, giNumUID.n[i].rows);
				}

				cv::Mat numCheckUID = giNumUID.n[i];
				Roi = _uidMat(r);

				matchTemplate(Roi, numCheckUID, match_result, cv::TM_CCOEFF_NORMED);

				double minVal_i, maxVal_i;
				cv::Point minLoc_i, maxLoc_i;
				//寻找最佳匹配位置
				cv::minMaxLoc(match_result, &minVal_i, &maxVal_i, &minLoc_i, &maxLoc_i);

				maxVal_i_list[i] = maxVal_i;
				x_i_list[i] = maxLoc_i.x + numCheckUID.cols - 1;
				if (maxVal_i > 0.85)
				{
					_NumBit[uid_p] = i;
					x_uid_i = x_uid_i + maxLoc_i.x + numCheckUID.cols - 1;
					break;
				}
				if (i == giNumUID.max - 1)
				{
					_NumBit[uid_p] = getMaxID(maxVal_i_list, 10);
					x_uid_i = x_uid_i + x_i_list[_NumBit[uid_p]];
				}
			}
		}
	}
	_uid = 0;
	for (int i = 0; i < 9; i++)
	{
		_uid += _NumBit[i] * bitCount;
		bitCount = bitCount * 10;
	}

}

int ATM_TM_TemplateUID::getUID()
{
	return _uid;
}

void ATM_TM_TemplateStar::Init()
{
	if (isInit)return;

	isInit = true;
}

void ATM_TM_TemplateStar::setStarTemplate(cv::Mat starTemplateMat)
{
	_starTemplate = starTemplateMat;
}

void ATM_TM_TemplateStar::setStarMat(cv::Mat starMat)
{
	_starMat = starMat;
}

void ATM_TM_TemplateStar::match()
{
	int MAXLOOP = 0;
	bool isLoopMatch = false;
	cv::Mat tmp;
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;

	pos.clear();

	matchTemplate(_starTemplate, _starMat, tmp, cv::TM_CCOEFF_NORMED);
	minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
	std::cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << std::endl;
#endif
	if (maxVal < 0.66)
	{
		isStarVisible = false;
	}
	else
	{
		isLoopMatch = true;
		isStarVisible = true;
		pos.push_back(maxLoc - cv::Point(_starMat.cols / 2, _starMat.rows / 2) + cv::Point(_starTemplate.cols / 2, _starTemplate.rows / 2));
	}

	while (isLoopMatch)
	{
		_starMat(cv::Rect(maxLoc.x, maxLoc.y, _starTemplate.cols, _starTemplate.rows)) = cv::Scalar(0, 0, 0);
		matchTemplate(_starTemplate, _starMat, tmp, cv::TM_CCOEFF_NORMED);
		minMaxLoc(tmp, &minVal, &maxVal, &minLoc, &maxLoc);
#ifdef _DEBUG
		std::cout << "Match Star MinVal & MaxVal : " << minVal << " , " << maxVal << std::endl;
#endif
		if (maxVal < 0.66)
		{
			isLoopMatch = false;
		}
		else
		{
			pos.push_back(maxLoc - cv::Point(_starMat.cols / 2, _starMat.rows / 2) + cv::Point(_starTemplate.cols / 2, _starTemplate.rows / 2));
		}

		MAXLOOP > 10 ? isLoopMatch = false : MAXLOOP++;
	}
}

bool ATM_TM_TemplateStar::getStar()
{
	return isStarVisible;
}

std::vector<cv::Point2d> ATM_TM_TemplateStar::getStarPos()
{
	return pos;
}
