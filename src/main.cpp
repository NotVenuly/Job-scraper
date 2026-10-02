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


std::string Trim(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
    [](unsigned char ch) { return !std::isspace(ch); }));
    
    s.erase(std::find_if(s.rbegin(), s.rend(),
    [](unsigned char ch) { return !std::isspace(ch); }).base(),
    s.end());
    
    return s;
}

void ScrapeSite(std::ofstream& file, const cpr::Url& url, const cpr::Parameters& params, const char* listSelector, const SiteConfig& config){
    
    cpr::Response r = cpr::Get(url, params);

    cout << r.status_code << endl;
    if (r.status_code != 200)
    {
        std::cout << "Failed: " << r.status_code << '\n';
        return;
    }
    size_t listHtml_len = r.text.length();

    std::vector<char> listHtml(r.text.begin(), r.text.end());
    listHtml.push_back('\0');   
    
    string emptyStr;

    auto pageJobs = Parse(listHtml.data(), listHtml_len, listSelector, &emptyStr, false, config);

       for (const auto& job : pageJobs)
        {
            file << "#### [" << Trim(job.first) << "](" << job.second.first << ")\n";
            file << job.second.second << "\n";
            file << "***\n\n";
        }

}

int main(int argc, char** argv) {
    
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    
    std::setlocale(LC_ALL, "en_US.utf8");
    string file = "jobs/page-";
    map<string, pair<string, string>> jobData;

    const int totalPages = 10;
    const int initialPage = 58;
    
    SiteConfig joblyConfig{
        "Jobly",
        "",
        "div.field__item.even"
    };
    
    SiteConfig duunitoriConfig{
        "Duunitori",
        "https://duunitori.fi",
        "div.description-box"
    };

    int numPage = 1;
    

        
    for (int step = 0; step < totalPages; step++)
    {
        int pageNum = initialPage + step;
        string fileName = "jobs/page-" + to_string(pageNum) + ".md";
        ofstream MyFile(fileName);
        numPage ++;
        
        try {

            ScrapeSite(
                MyFile,
                cpr::Url{"https://www.jobly.fi/en/jobs"},
                {
                    {"search", ""},
                    {"job_geo_location", "Uusimaa, Suomi"},
                    {"Search_jobs", "Search jobs"},
                    {"lat", "60.21872"},
                    {"lon", "25.2716209"},
                    {"country", "Suomi"},
                    {"administrative_area_level_1", "Uusimaa"},
                    {"page", to_string(pageNum)}
                },
                "div.job__content.clearfix > h2.node__title.node__title > a",
                joblyConfig
            );



            
        } catch (const std::exception& e) {
            cout << "Exception: " << e.what() << endl;
            cout << "Press Enter to exit..." << endl;
            std::cin.get();
        }

        try{

            ScrapeSite(
                MyFile,
                cpr::Url{"https://duunitori.fi/tyopaikat"},
                {
                    {"alue", "uusimaa"},
                    {"sivu", to_string(pageNum)}
                },
                "a.job-box__hover.gtm-search-result",
                duunitoriConfig
            );

        }catch (const std::exception& e) {
            cout << "Exception: " << e.what() << endl;
            cout << "Press Enter to exit..." << endl;
            std::cin.get();
        }
    }


    cout << "Press Enter to exit..." << endl;
    std::cin.get();
    return 0;
}


