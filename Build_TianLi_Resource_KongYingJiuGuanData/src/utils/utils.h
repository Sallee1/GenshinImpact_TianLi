#pragma once
#include <iostream>
#include <filesystem>
#include <regex>
// �������� Opencv
#include <opencv2/opencv.hpp>
// �������� MeoJson
#include "meojson/json.hpp"
#include "meojson/json5.hpp"
// �������� Sqlite3
#include "sqlite3/sqlite3.h"
// src
#include "include/config.h"

namespace utils {
	json::array load_json_api(std::string file_name)
	{
		//const std::string file_name = "/area.json";
		const std::string file_path = config::root + config::dir_name + file_name;
		std::ifstream ifs(file_path, std::ios::in | std::ios::binary);
		if (!ifs.is_open())
		{
			std::cout << "open file failed" << std::endl;
			return json::array();
		}
		std::string json_string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		auto json = json::parse(json_string);
		if (!json)
		{
			std::cout << "parse json failed" << std::endl;
			return json::array();
		}
		return json.value().as_array();
	}
	auto get_all_file_names(const std::string& path)
	{
		std::vector<std::string> files;
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			files.push_back(entry.path().filename().string());
		}
		return files;
	}
	auto split(const std::string& s, const std::string& separator = ",") {
		std::vector<std::string> tokens;
		size_t begin_pos = 0;
		while (begin_pos < s.size()) {
			const size_t end_pos = s.find_first_of(separator, begin_pos);
			if (end_pos != std::string::npos) {
				if (end_pos > begin_pos) {
					tokens.emplace_back(&s[begin_pos], end_pos - begin_pos);
				}
				begin_pos = end_pos + 1;
			}
			else {
				tokens.emplace_back(&s[begin_pos]);
				break;
			}
		}
		return tokens;
	}
	bool is_file_exist(const std::string& path)
	{
		return std::filesystem::exists(path);
	}
	void delete_file(const std::string& path)
	{
		std::filesystem::remove(path);
	}
	std::string get_file_name(const std::string& url)
	{
		std::regex reg(R"(([^/]+)\.png)");
		std::smatch sm;
		std::regex_search(url, sm, reg);
		auto name = sm[1].str() + ".png";
		return name;

	}
	bool download_file(const std::string& url, const std::string& path)
	{
		std::string cmd = "curl -o " + path + " " + url + " -s";
		return system(cmd.c_str()) == 0;
	}
	struct Data {
		const char* data;
		int size;
	};
	Data read_file_data(std::string path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open()) {
			cv::Mat img = cv::imread("C:/Users/GengG/source/repos/GenshinImpact_TianLi/Build_TianLi_Resource_KongYingJiuGuanData/resource/�ʺ�.png", cv::IMREAD_UNCHANGED);
			cv::imwrite(path, img);
			file.close();
			file = std::ifstream(path, std::ios::binary);
		}
		cv::Mat img = cv::imread(path, cv::IMREAD_UNCHANGED);
		if (img.empty())
		{
			cv::Mat img = cv::imread("C:/Users/GengG/source/repos/GenshinImpact_TianLi/Build_TianLi_Resource_KongYingJiuGuanData/resource/�ʺ�.png", cv::IMREAD_UNCHANGED);
			cv::imwrite(path, img);
			file.close();
			file = std::ifstream(path, std::ios::binary);
		}
		file.seekg(0, std::ios::end);
		auto size = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);
		char* buffer = new char[size];
		file.read(buffer, size);
		file.close();
		return { buffer, size };
	}
	cv::Mat read_png(std::string path)
	{
		cv::Mat img = cv::imread(path, cv::IMREAD_UNCHANGED);
		return img;

	}
}