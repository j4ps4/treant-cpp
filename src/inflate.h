#pragma once

#include <map>

const std::map<std::string, std::string> nullmap = {};

void inflate(const char* fn, std::map<std::string, std::string> converts = nullmap);