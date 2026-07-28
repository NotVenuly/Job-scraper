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

#include <cpr/cpr.h>
#include "parser.h"

using namespace std;

string utf8_to_system(const std::string& utf8_str) {
    #ifdef _WIN32
        int wide_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
        if (wide_len == 0) return utf8_str;
        
        std::vector<wchar_t> wide_buf(wide_len);
        MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, wide_buf.data(), wide_len);
        
        int ansi_len = WideCharToMultiByte(CP_ACP, 0, wide_buf.data(), -1, NULL, 0, NULL, NULL);
        if (ansi_len == 0) return utf8_str;
        
        std::vector<char> ansi_buf(ansi_len);
        WideCharToMultiByte(CP_ACP, 0, wide_buf.data(), -1, ansi_buf.data(), ansi_len, NULL, NULL);
        
        return std::string(ansi_buf.data());
    #else
        return utf8_str;
    #endif
}

int main(int argc, char** argv) {

    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    
    std::setlocale(LC_ALL, "en_US.utf8");
    string str;
    map<string, string> jobData;

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
            {"administrative_area_level_1", "Uusimaa"}
        });
        ofstream file("page1.html");
        file << r.text;
        file.close();
        cout << "HTML saved to page.html" << endl;

        cout << r.status_code << endl;
        cout << r.header["content-type"] << endl;
        cout << r.header["content-length"] << endl;
        cout << r.header["date"] << endl;
        size_t listHtml_len = r.text.length();
        char listHtml[listHtml_len + 1];

        // Copy the HTML content to the char array
        for (size_t i = 0; i < listHtml_len; ++i) {
            listHtml[i] = r.text[i];
        }
        listHtml[listHtml_len] = '\0';

        //cout << r.text << endl;
        cout << r.url.str() << endl;

        jobData = Parse(listHtml, listHtml_len);
        map<string, string>::iterator it;
        for (const auto& pair : jobData) {
            string title_display = utf8_to_system(pair.first);
            string url_display = utf8_to_system(pair.second);
            
            cout << "Job Title: " << title_display << endl;
            cout << "Job URL: " << url_display << endl;
            cout << "------------------------" << endl;
        };

        cout << "Press Enter to exit..." << endl;
        cin.get();


    } catch (const std::exception& e) {
        cout << "Exception: " << e.what() << endl;
        cout << "Press Enter to exit..." << endl;
        cin.get();
    }

    cin >> str;
    return 0;
}