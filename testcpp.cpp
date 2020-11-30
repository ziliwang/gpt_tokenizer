#include <iostream>
#include <boost/regex.hpp>
#include <regex>
#include <string>
#include <codecvt>
#include <locale.h>

std::string w_to_utf8(const std::wstring &s){
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
    return conv.to_bytes(s);
};

int main(){
    using namespace std;
    wstring a = L"assé..sss哈練ウ페يَّةُру́сский"; //26letter
    boost::wregex reg(L"[[:alpha:]]");
    boost::wsregex_iterator iter(a.begin(), a.end(), reg);
    boost::wsregex_iterator end;
    while(iter != end){
        wstring tmp = iter->str();
        cout << w_to_utf8(tmp) << endl;
        ++iter;
    }
}