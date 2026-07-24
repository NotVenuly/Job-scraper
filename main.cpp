#include <iostream>

#include <cpr/cpr.h>

using namespace std;

int main(int argc, char** argv) {
    string str;
    cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                      cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
                      cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string

    cout << r.status_code << endl;
    cout << r.header["content-type"] << endl;
    cout << r.text << endl;

    cin >> str;
    return 0;
}