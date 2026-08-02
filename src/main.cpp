#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <clocale>
#include <windows.h>
#include <codecvt>
#include <locale> 
#include <algorithm>
#include <cctype>

#include <cpr/cpr.h>
#include "parser.h"

using namespace std;

string TransformPage(int num){
    return to_string(num);
}


std::string Trim(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));

    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(),
        s.end());

    return s;
}

int main(int argc, char** argv) {

    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
        
        std::setlocale(LC_ALL, "en_US.utf8");
        string file = "jobs/page-";
        map<string, pair<string, string>> jobData;
        
    const int pagesPerFile = 2;
    const int totalPages = 10;

    for (int fileStart = 1; fileStart <= totalPages; fileStart ++)
    {
        string fileName = "jobs/page-" + to_string(fileStart) + ".md";
        ofstream MyFile(fileName);

        for (int pageNum = fileStart; pageNum < fileStart + pagesPerFile && pageNum <= totalPages; pageNum++)
        {

            try {
                cpr::Response r = cpr::Get(
                    // cpr::Url{"https://duunitori.fi/tyopaikat/alue/uusimaa"
                    cpr::Url{"https://www.jobly.fi/en/jobs"},
                    cpr::Parameters{
                    {"search", ""},
                    {"job_geo_location", "Uusimaa, Suomi"},
                    {"Search_jobs", "Search jobs"},
                    {"lat", "60.21872"},
                    {"lon", "25.2716209"},
                    {"country", "Suomi"},
                    {"administrative_area_level_1", "Uusimaa"},
                    {"page", TransformPage(pageNum)}
                });

                cout << r.status_code << endl;

                size_t listHtml_len = r.text.length();
                std::vector<char> listHtml(listHtml_len + 1, 0);
        
                for (size_t i = 0; i < listHtml_len; ++i) {
                    listHtml[i] = r.text[i];
                }
                listHtml[listHtml_len] = '\0';
                
                string emptyStr;

                auto pageJobs = Parse(listHtml.data(), listHtml_len, "div.job__content.clearfix > h2.node__title.node__title > a", &emptyStr, false);

                for (const auto& job : pageJobs)
                {
                    MyFile << "[" << Trim(job.first) << "](" << job.second.first << ")\n";
                    MyFile << job.second.second << "\n";
                    MyFile << "***\n\n";
                }
                
            } catch (const std::exception& e) {
                cout << "Exception: " << e.what() << endl;
                cout << "Press Enter to exit..." << endl;
                cin.get();
            }
        }
    }

    cout << "Press Enter to exit..." << endl;
    cin.get();
    return 0;
}