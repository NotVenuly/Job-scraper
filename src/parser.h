#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <string>

struct SiteConfig
{
    std::string siteName;
    std::string baseUrl;
    const char* detailSelector;
};

std::map<std::string, std::pair<std::string, std::string>> Parse(char htmlIN[], size_t htmlIN_len, const char selectorStr[], std::string* returnedStr, bool isDetail, const SiteConfig& config);

#endif 