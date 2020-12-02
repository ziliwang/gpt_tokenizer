#include <string>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <utility>
#include <tuple>
#include <regex>
#include <codecvt>
#include <algorithm>
#include "3rdparty/robin_hood.h"
#include "3rdparty/json.hpp"
#include <unordered_map>
#include <unordered_set>

// #define MAP robin_hood::unordered_map
#define SET std::unordered_set
#define MAP std::unordered_map

typedef std::pair<std::string,std::string> bigram_pair;
typedef std::pair<std::wstring,std::wstring> wbigram_pair;


// codecvt abandoned in c++17
std::wstring utf8_to_wstring(const std::string& src){
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(src);
};

std::u32string utf8_to_utf32(const std::string& src){
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.from_bytes(src);
};

std::string wstring_to_utf8(const std::wstring& src){
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(src);
};

std::string utf32_to_utf8(const std::u32string& src){
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.to_bytes(src);
};



std::string unicode_letter_expr = 
        "\\u0041-\\u005A\\u0061-\\u007A\\u00AA-\\u00AA\\u00B5-\\u00B5"
        "\\u00BA-\\u00BA\\u00C0-\\u00D6\\u00D8-\\u00F6\\u00F8-\\u02C1"
        "\\u02C6-\\u02D1\\u02E0-\\u02E4\\u02EC-\\u02EC\\u02EE-\\u02EE"
        "\\u0370-\\u0374\\u0376-\\u0377\\u037A-\\u037D\\u037F-\\u037F"
        "\\u0386-\\u0386\\u0388-\\u038A\\u038C-\\u038C\\u038E-\\u03A1"
        "\\u03A3-\\u03F5\\u03F7-\\u0481\\u048A-\\u052F\\u0531-\\u0556"
        "\\u0559-\\u0559\\u0560-\\u0588\\u05D0-\\u05EA\\u05EF-\\u05F2"
        "\\u0620-\\u064A\\u066E-\\u066F\\u0671-\\u06D3\\u06D5-\\u06D5"
        "\\u06E5-\\u06E6\\u06EE-\\u06EF\\u06FA-\\u06FC\\u06FF-\\u06FF"
        "\\u0710-\\u0710\\u0712-\\u072F\\u074D-\\u07A5\\u07B1-\\u07B1"
        "\\u07CA-\\u07EA\\u07F4-\\u07F5\\u07FA-\\u07FA\\u0800-\\u0815"
        "\\u081A-\\u081A\\u0824-\\u0824\\u0828-\\u0828\\u0840-\\u0858"
        "\\u0860-\\u086A\\u08A0-\\u08B4\\u08B6-\\u08C7\\u0904-\\u0939"
        "\\u093D-\\u093D\\u0950-\\u0950\\u0958-\\u0961\\u0971-\\u0980"
        "\\u0985-\\u098C\\u098F-\\u0990\\u0993-\\u09A8\\u09AA-\\u09B0"
        "\\u09B2-\\u09B2\\u09B6-\\u09B9\\u09BD-\\u09BD\\u09CE-\\u09CE"
        "\\u09DC-\\u09DD\\u09DF-\\u09E1\\u09F0-\\u09F1\\u09FC-\\u09FC"
        "\\u0A05-\\u0A0A\\u0A0F-\\u0A10\\u0A13-\\u0A28\\u0A2A-\\u0A30"
        "\\u0A32-\\u0A33\\u0A35-\\u0A36\\u0A38-\\u0A39\\u0A59-\\u0A5C"
        "\\u0A5E-\\u0A5E\\u0A72-\\u0A74\\u0A85-\\u0A8D\\u0A8F-\\u0A91"
        "\\u0A93-\\u0AA8\\u0AAA-\\u0AB0\\u0AB2-\\u0AB3\\u0AB5-\\u0AB9"
        "\\u0ABD-\\u0ABD\\u0AD0-\\u0AD0\\u0AE0-\\u0AE1\\u0AF9-\\u0AF9"
        "\\u0B05-\\u0B0C\\u0B0F-\\u0B10\\u0B13-\\u0B28\\u0B2A-\\u0B30"
        "\\u0B32-\\u0B33\\u0B35-\\u0B39\\u0B3D-\\u0B3D\\u0B5C-\\u0B5D"
        "\\u0B5F-\\u0B61\\u0B71-\\u0B71\\u0B83-\\u0B83\\u0B85-\\u0B8A"
        "\\u0B8E-\\u0B90\\u0B92-\\u0B95\\u0B99-\\u0B9A\\u0B9C-\\u0B9C"
        "\\u0B9E-\\u0B9F\\u0BA3-\\u0BA4\\u0BA8-\\u0BAA\\u0BAE-\\u0BB9"
        "\\u0BD0-\\u0BD0\\u0C05-\\u0C0C\\u0C0E-\\u0C10\\u0C12-\\u0C28"
        "\\u0C2A-\\u0C39\\u0C3D-\\u0C3D\\u0C58-\\u0C5A\\u0C60-\\u0C61"
        "\\u0C80-\\u0C80\\u0C85-\\u0C8C\\u0C8E-\\u0C90\\u0C92-\\u0CA8"
        "\\u0CAA-\\u0CB3\\u0CB5-\\u0CB9\\u0CBD-\\u0CBD\\u0CDE-\\u0CDE"
        "\\u0CE0-\\u0CE1\\u0CF1-\\u0CF2\\u0D04-\\u0D0C\\u0D0E-\\u0D10"
        "\\u0D12-\\u0D3A\\u0D3D-\\u0D3D\\u0D4E-\\u0D4E\\u0D54-\\u0D56"
        "\\u0D5F-\\u0D61\\u0D7A-\\u0D7F\\u0D85-\\u0D96\\u0D9A-\\u0DB1"
        "\\u0DB3-\\u0DBB\\u0DBD-\\u0DBD\\u0DC0-\\u0DC6\\u0E01-\\u0E30"
        "\\u0E32-\\u0E33\\u0E40-\\u0E46\\u0E81-\\u0E82\\u0E84-\\u0E84"
        "\\u0E86-\\u0E8A\\u0E8C-\\u0EA3\\u0EA5-\\u0EA5\\u0EA7-\\u0EB0"
        "\\u0EB2-\\u0EB3\\u0EBD-\\u0EBD\\u0EC0-\\u0EC4\\u0EC6-\\u0EC6"
        "\\u0EDC-\\u0EDF\\u0F00-\\u0F00\\u0F40-\\u0F47\\u0F49-\\u0F6C"
        "\\u0F88-\\u0F8C\\u1000-\\u102A\\u103F-\\u103F\\u1050-\\u1055"
        "\\u105A-\\u105D\\u1061-\\u1061\\u1065-\\u1066\\u106E-\\u1070"
        "\\u1075-\\u1081\\u108E-\\u108E\\u10A0-\\u10C5\\u10C7-\\u10C7"
        "\\u10CD-\\u10CD\\u10D0-\\u10FA\\u10FC-\\u1248\\u124A-\\u124D"
        "\\u1250-\\u1256\\u1258-\\u1258\\u125A-\\u125D\\u1260-\\u1288"
        "\\u128A-\\u128D\\u1290-\\u12B0\\u12B2-\\u12B5\\u12B8-\\u12BE"
        "\\u12C0-\\u12C0\\u12C2-\\u12C5\\u12C8-\\u12D6\\u12D8-\\u1310"
        "\\u1312-\\u1315\\u1318-\\u135A\\u1380-\\u138F\\u13A0-\\u13F5"
        "\\u13F8-\\u13FD\\u1401-\\u166C\\u166F-\\u167F\\u1681-\\u169A"
        "\\u16A0-\\u16EA\\u16F1-\\u16F8\\u1700-\\u170C\\u170E-\\u1711"
        "\\u1720-\\u1731\\u1740-\\u1751\\u1760-\\u176C\\u176E-\\u1770"
        "\\u1780-\\u17B3\\u17D7-\\u17D7\\u17DC-\\u17DC\\u1820-\\u1878"
        "\\u1880-\\u1884\\u1887-\\u18A8\\u18AA-\\u18AA\\u18B0-\\u18F5"
        "\\u1900-\\u191E\\u1950-\\u196D\\u1970-\\u1974\\u1980-\\u19AB"
        "\\u19B0-\\u19C9\\u1A00-\\u1A16\\u1A20-\\u1A54\\u1AA7-\\u1AA7"
        "\\u1B05-\\u1B33\\u1B45-\\u1B4B\\u1B83-\\u1BA0\\u1BAE-\\u1BAF"
        "\\u1BBA-\\u1BE5\\u1C00-\\u1C23\\u1C4D-\\u1C4F\\u1C5A-\\u1C7D"
        "\\u1C80-\\u1C88\\u1C90-\\u1CBA\\u1CBD-\\u1CBF\\u1CE9-\\u1CEC"
        "\\u1CEE-\\u1CF3\\u1CF5-\\u1CF6\\u1CFA-\\u1CFA\\u1D00-\\u1DBF"
        "\\u1E00-\\u1F15\\u1F18-\\u1F1D\\u1F20-\\u1F45\\u1F48-\\u1F4D"
        "\\u1F50-\\u1F57\\u1F59-\\u1F59\\u1F5B-\\u1F5B\\u1F5D-\\u1F5D"
        "\\u1F5F-\\u1F7D\\u1F80-\\u1FB4\\u1FB6-\\u1FBC\\u1FBE-\\u1FBE"
        "\\u1FC2-\\u1FC4\\u1FC6-\\u1FCC\\u1FD0-\\u1FD3\\u1FD6-\\u1FDB"
        "\\u1FE0-\\u1FEC\\u1FF2-\\u1FF4\\u1FF6-\\u1FFC\\u2071-\\u2071"
        "\\u207F-\\u207F\\u2090-\\u209C\\u2102-\\u2102\\u2107-\\u2107"
        "\\u210A-\\u2113\\u2115-\\u2115\\u2119-\\u211D\\u2124-\\u2124"
        "\\u2126-\\u2126\\u2128-\\u2128\\u212A-\\u212D\\u212F-\\u2139"
        "\\u213C-\\u213F\\u2145-\\u2149\\u214E-\\u214E\\u2183-\\u2184"
        "\\u2C00-\\u2C2E\\u2C30-\\u2C5E\\u2C60-\\u2CE4\\u2CEB-\\u2CEE"
        "\\u2CF2-\\u2CF3\\u2D00-\\u2D25\\u2D27-\\u2D27\\u2D2D-\\u2D2D"
        "\\u2D30-\\u2D67\\u2D6F-\\u2D6F\\u2D80-\\u2D96\\u2DA0-\\u2DA6"
        "\\u2DA8-\\u2DAE\\u2DB0-\\u2DB6\\u2DB8-\\u2DBE\\u2DC0-\\u2DC6"
        "\\u2DC8-\\u2DCE\\u2DD0-\\u2DD6\\u2DD8-\\u2DDE\\u2E2F-\\u2E2F"
        "\\u3005-\\u3006\\u3031-\\u3035\\u303B-\\u303C\\u3041-\\u3096"
        "\\u309D-\\u309F\\u30A1-\\u30FA\\u30FC-\\u30FF\\u3105-\\u312F"
        "\\u3131-\\u318E\\u31A0-\\u31BF\\u31F0-\\u31FF\\u3400-\\u4DBF"
        "\\u4E00-\\u9FFC\\uA000-\\uA48C\\uA4D0-\\uA4FD\\uA500-\\uA60C"
        "\\uA610-\\uA61F\\uA62A-\\uA62B\\uA640-\\uA66E\\uA67F-\\uA69D"
        "\\uA6A0-\\uA6E5\\uA717-\\uA71F\\uA722-\\uA788\\uA78B-\\uA7BF"
        "\\uA7C2-\\uA7CA\\uA7F5-\\uA801\\uA803-\\uA805\\uA807-\\uA80A"
        "\\uA80C-\\uA822\\uA840-\\uA873\\uA882-\\uA8B3\\uA8F2-\\uA8F7"
        "\\uA8FB-\\uA8FB\\uA8FD-\\uA8FE\\uA90A-\\uA925\\uA930-\\uA946"
        "\\uA960-\\uA97C\\uA984-\\uA9B2\\uA9CF-\\uA9CF\\uA9E0-\\uA9E4"
        "\\uA9E6-\\uA9EF\\uA9FA-\\uA9FE\\uAA00-\\uAA28\\uAA40-\\uAA42"
        "\\uAA44-\\uAA4B\\uAA60-\\uAA76\\uAA7A-\\uAA7A\\uAA7E-\\uAAAF"
        "\\uAAB1-\\uAAB1\\uAAB5-\\uAAB6\\uAAB9-\\uAABD\\uAAC0-\\uAAC0"
        "\\uAAC2-\\uAAC2\\uAADB-\\uAADD\\uAAE0-\\uAAEA\\uAAF2-\\uAAF4"
        "\\uAB01-\\uAB06\\uAB09-\\uAB0E\\uAB11-\\uAB16\\uAB20-\\uAB26"
        "\\uAB28-\\uAB2E\\uAB30-\\uAB5A\\uAB5C-\\uAB69\\uAB70-\\uABE2"
        "\\uAC00-\\uD7A3\\uD7B0-\\uD7C6\\uD7CB-\\uD7FB\\uF900-\\uFA6D"
        "\\uFA70-\\uFAD9\\uFB00-\\uFB06\\uFB13-\\uFB17\\uFB1D-\\uFB1D"
        "\\uFB1F-\\uFB28\\uFB2A-\\uFB36\\uFB38-\\uFB3C\\uFB3E-\\uFB3E"
        "\\uFB40-\\uFB41\\uFB43-\\uFB44\\uFB46-\\uFBB1\\uFBD3-\\uFD3D"
        "\\uFD50-\\uFD8F\\uFD92-\\uFDC7\\uFDF0-\\uFDFB\\uFE70-\\uFE74"
        "\\uFE76-\\uFEFC\\uFF21-\\uFF3A\\uFF41-\\uFF5A\\uFF66-\\uFFBE"
        "\\uFFC2-\\uFFC7\\uFFCA-\\uFFCF\\uFFD2-\\uFFD7\\uFFDA-\\uFFDC";

std::string unicode_number_expr = 
        "\\u0030-\\u0039\\u00B2-\\u00B3\\u00B9-\\u00B9\\u00BC-\\u00BE"
        "\\u0660-\\u0669\\u06F0-\\u06F9\\u07C0-\\u07C9\\u0966-\\u096F"
        "\\u09E6-\\u09EF\\u09F4-\\u09F9\\u0A66-\\u0A6F\\u0AE6-\\u0AEF"
        "\\u0B66-\\u0B6F\\u0B72-\\u0B77\\u0BE6-\\u0BF2\\u0C66-\\u0C6F"
        "\\u0C78-\\u0C7E\\u0CE6-\\u0CEF\\u0D58-\\u0D5E\\u0D66-\\u0D78"
        "\\u0DE6-\\u0DEF\\u0E50-\\u0E59\\u0ED0-\\u0ED9\\u0F20-\\u0F33"
        "\\u1040-\\u1049\\u1090-\\u1099\\u1369-\\u137C\\u16EE-\\u16F0"
        "\\u17E0-\\u17E9\\u17F0-\\u17F9\\u1810-\\u1819\\u1946-\\u194F"
        "\\u19D0-\\u19DA\\u1A80-\\u1A89\\u1A90-\\u1A99\\u1B50-\\u1B59"
        "\\u1BB0-\\u1BB9\\u1C40-\\u1C49\\u1C50-\\u1C59\\u2070-\\u2070"
        "\\u2074-\\u2079\\u2080-\\u2089\\u2150-\\u2182\\u2185-\\u2189"
        "\\u2460-\\u249B\\u24EA-\\u24FF\\u2776-\\u2793\\u2CFD-\\u2CFD"
        "\\u3007-\\u3007\\u3021-\\u3029\\u3038-\\u303A\\u3192-\\u3195"
        "\\u3220-\\u3229\\u3248-\\u324F\\u3251-\\u325F\\u3280-\\u3289"
        "\\u32B1-\\u32BF\\uA620-\\uA629\\uA6E6-\\uA6EF\\uA830-\\uA835"
        "\\uA8D0-\\uA8D9\\uA900-\\uA909\\uA9D0-\\uA9D9\\uA9F0-\\uA9F9"
        "\\uAA50-\\uAA59\\uABF0-\\uABF9\\uFF10-\\uFF19";

std::wstring wpat_expr = utf8_to_wstring("'s|'t|'re|'ve|'m|'ll|'d| ?[" + unicode_letter_expr + 
        "]+| ?[" + unicode_number_expr + "]+| ?[^\\s" + unicode_letter_expr + unicode_number_expr + 
        "]+|\\s+(?!\\S)|\\s+");

wchar_t* bytes_to_unicode(){
    std::vector<uint64_t> bs;
    for(auto i =uint64_t(L'!');i < uint64_t(L'~')+1;++i){
        bs.push_back(i);
    }
    for(auto i=uint64_t(L'¡');i<uint64_t(L'¬')+1;++i){
        bs.push_back(i);
    }
    for(auto i=uint64_t(L'®');i<uint64_t(L'ÿ')+1;++i){
        bs.push_back(i);
    }
    std::vector<uint64_t> cs = bs;
    uint64_t n=0;
    for(uint64_t b=0;b<256;++b){
        auto p = find(bs.begin(),bs.end(),b);
        if(p==bs.end()){
            bs.push_back(b);
            cs.push_back(256+n);
            n++;
        }
    }
    static wchar_t bytes_mapping[256]={};
    for(size_t i=0;i<256;i++){
        bytes_mapping[i] = i;
    }
    for(size_t i=0;i<bs.size();i++){
        bytes_mapping[bs[i]] = cs[i];
    }
    return bytes_mapping;
}

struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const std::pair<T1, T2>& p) const
    { 
        auto hash1 = std::hash<T1>{}(p.first); 
        auto hash2 = std::hash<T2>{}(p.second); 
        return hash1 ^ hash2; 
    } 
}; 

class Encoder{

public:
    Encoder(const std::string & vocab_file, const std::string & merge_file, const std::string & bos_token_str="<s>", 
    const std::string eos_token_str="</s>", const std::string pad_token_str="<pad>", const std::string unk_token_str="<unk>",
    const std::string mask_token_str="<mask>"){
        load_vocab(vocab_file);
        load_merge(merge_file);
        bos_token = bos_token_str;
        eos_token = eos_token_str;
        pad_token = pad_token_str;
        unk_token = unk_token_str;
        mask_token = mask_token_str;
        bytes_encoder = bytes_to_unicode();
    };
    // ~Encoder();
    std::vector<std::string> bpe(std::wstring token);
    std::vector<std::string> tokenize(std::string str);
    int64_t convert_token_to_id(std::string token);
    void padding_encode_single_with_special_tokens(std::string str, size_t max_length, std::vector<int64_t> *input_ids, std::vector<int64_t> *mask_ids);
    std::string bos_token;
    std::string eos_token;
    std::string pad_token;
    std::string unk_token;
    std::string mask_token;


private:
    MAP<std::string, int64_t> vocab;
    MAP<wbigram_pair, uint32_t, hash_pair> bpe_ranks;
    wchar_t* bytes_encoder = bytes_to_unicode();
    uint32_t cache_max_size=500000;
    uint32_t cache_word_max_length=30;
    const std::wregex pat = std::wregex(wpat_expr);
    MAP<std::wstring, std::vector<std::string>> cache;
    void load_vocab(const std::string &vocab_file);
    void load_merge(const std::string &merge_file);
};


std::vector<std::string> split(const std::string &s, std::regex rgx=std::regex("\\s+")){
    std::vector<std::string> elems;
    std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
    std::sregex_token_iterator end;
    while (iter != end)
    {
        elems.push_back(*iter);
        ++iter;
    }
    return elems;
};

SET<wbigram_pair, hash_pair> get_pairs(std::vector<std::wstring> word){
    SET<wbigram_pair, hash_pair> pairs;
    std::wstring prev_char = word[0];
    for(size_t i=1;i<word.size();++i){
        pairs.insert(wbigram_pair({prev_char, word[i]}));
        prev_char = word[i];
    }
    return pairs;
};

void Encoder::load_vocab(const std::string &vocab_file){
    std::ifstream file_handle(vocab_file);
    assert(file_handle.good() && "file not exists");
    nlohmann::json vocab_data_;
    file_handle >> vocab_data_;
    auto vocab_ = vocab_data_.get<std::unordered_map<std::string, int64_t>>();
    for(auto item: vocab_){
        vocab.insert({item.first, item.second});
    }
};
void Encoder::load_merge(const std::string &merge_file){
    //c++中，char, string 都是ANSI字符
    bpe_ranks.reserve(60000);
    std::ifstream file_handle(merge_file);
    assert(file_handle.good() && "file not exists");
    std::string line;
    uint32_t curr_idx = 0;
    while(getline(file_handle, line)){
        if(line[0] == '#' || line.size() == 0) continue;
        std::vector<std::string> bigrams = split(line);
        assert(bigrams.size()==2 && "unk format");
        wbigram_pair curr(utf8_to_wstring(bigrams[0]), utf8_to_wstring(bigrams[1]));
        bpe_ranks.insert({curr, curr_idx});
        curr_idx++;
    }
};
std::vector<std::string> Encoder::bpe(std::wstring token){
    // bpe use wstring
    if(cache.find(token) != cache.end()) return cache[token];
    std::vector<std::wstring> wword;
    for(auto c: token){
        wword.push_back(std::wstring(1, c));
    }
    SET<wbigram_pair, hash_pair> pairs = get_pairs(wword);
    if(pairs.empty()) return {wstring_to_utf8(token)};
    
    while(true){
        auto bigram = pairs.begin();
        if(pairs.size() > 1) bigram = std::min_element(pairs.begin(), pairs.end(), 
            [this](const wbigram_pair & a, const wbigram_pair & b) -> bool{
                if(bpe_ranks.find(a)==bpe_ranks.end()) return false;
                if(bpe_ranks.find(b)==bpe_ranks.end()) return true;
                return bpe_ranks[a] < bpe_ranks[b]; 
            });
        if(bpe_ranks.find(*bigram) == bpe_ranks.end()) break;
        std::wstring first = bigram->first;
        std::wstring second = bigram->second;
        decltype(wword) new_wword;

        auto i = wword.begin();
        while(i < wword.end()){
            auto j = std::find(i, wword.end(), first);
            if(j == wword.end()){
                new_wword.insert(new_wword.end(), i, wword.end());
                break;
            }
            new_wword.insert(new_wword.end(), i, j);
            i = j;
            // i <= wword.end
            if(*i == first && i < wword.end() -1 && *(i+1)==second){
                new_wword.push_back(first+second);
                i += 2;
            }
            else{
                new_wword.push_back(*i);
                i += 1;
            }
        }
        wword = new_wword;
        if(wword.size()==1) break;
        else pairs = get_pairs(wword);
    }
    std::vector<std::string> word;
    for(auto w: wword){
        word.push_back(wstring_to_utf8(w));
    }
    if(token.size() < cache_word_max_length && cache.size() < cache_max_size) cache.insert( {token, word} );
    return word;
};
std::vector<std::string> Encoder::tokenize(std::string str){
    // bpe_tokens = []
    std::vector<std::string> bpe_tokens;
    std::wstring wstr= utf8_to_wstring(str);
    std::wsregex_iterator iter(wstr.begin(), wstr.end(), pat);
    std::wsregex_iterator end;
    while(iter != end){
        std::wstring token;
        for(char c: wstring_to_utf8(iter->str())){
            if(0 > c) token.push_back(*(bytes_encoder + c + 256));
            else token.push_back(*(bytes_encoder + c));
        }
        decltype(bpe_tokens) curr_bpe_tokens = bpe(token);
        bpe_tokens.insert(bpe_tokens.end(), curr_bpe_tokens.begin(), curr_bpe_tokens.end());
        ++iter;
    }
    return bpe_tokens;
}
int64_t Encoder::convert_token_to_id(std::string token){
    // return self.encoder.get(token, self.encoder.get(self.unk_token))
    auto p = vocab.find(token);
    if(p!=vocab.end()){
        return vocab[token];
    }
    else{
        return vocab[unk_token];
    }
}
void Encoder::padding_encode_single_with_special_tokens(std::string str, size_t max_length, std::vector<int64_t> *input_ids, std::vector<int64_t> *mask_ids){
    if(str[0] != ' ') str=" " + str;
    if(not input_ids->empty() ) input_ids->clear();
    if(not mask_ids->empty() ) mask_ids->clear();
    input_ids->reserve(max_length);
    mask_ids->reserve(max_length);
    input_ids->push_back(vocab[bos_token]);
    mask_ids->push_back(1);
    auto tokens = tokenize(str);
    for(auto t:tokens){
        if(input_ids->size() == max_length-1) break;
        input_ids->push_back(convert_token_to_id(t));
        mask_ids->push_back(1);
    }
    input_ids->push_back(vocab[eos_token]);
    mask_ids->push_back(1);
    while(input_ids->size() < max_length){
        input_ids->push_back(vocab[pad_token]);
        mask_ids->push_back(0);
    }
}



int main(){
    Encoder test("/home/wangzl/WD/SimilarTextSug/pretrained/roberta-base-es/vocab.json", "/home/wangzl/WD/SimilarTextSug/pretrained/roberta-base-es/merges.txt");
    std::vector<int64_t> input_ids;
    std::vector<int64_t> mask_ids;
    std::cout << "init ok\n";
    test.padding_encode_single_with_special_tokens("this is   a test example!!!", 10, &input_ids, &mask_ids);
    for(auto i: input_ids){
        std::cout << i << " ";
    }
    std::cout << "\n-----\n";
    for(auto i: mask_ids){
        std::cout << i << " ";
    }
    std::cout << "\n";
    test.padding_encode_single_with_special_tokens("Tribunal Supremo tendrá a los prisión", 10, &input_ids, &mask_ids);
    for(auto i: input_ids){
        std::cout << i << " ";
    }
    std::cout << "\n-----\n";
    for(auto i: mask_ids){
        std::cout << i << " ";
    }
    std::cout << "\n";
    test.padding_encode_single_with_special_tokens("assé .. sss 哈練ウ페 يَّةُ ру́сский", 10, &input_ids, &mask_ids);
    for(auto i: input_ids){
        std::cout << i << " ";
    }
    std::cout << "\n-----\n";
    for(auto i: mask_ids){
        std::cout << i << " ";
    }
    std::cout << "\n";
}