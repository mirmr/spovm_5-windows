#pragma once
#include <string>
#include <vector>

extern "C" __declspec(dllexport) void reader(std::vector<std::string> filenames);
extern "C" __declspec(dllexport) void writer(std::string filename);