#include <String>
#include <stdint.h>
#include <fstream>
#include <utility>
#include <tuple>
#include <regex>
#include <codecvt>
#include <algorithm>
#include <boost/regex.hpp>
#include "3rdparty/robin_hood.h"
#include "3rdparty/json.hpp"

#define MAP robin_hood::unordered_map
#define SET robin_hood::unordered_set

typedef std::pair<std::string,std::string> bigram_pair;
typedef std::pair<std::wstring,std::wstring> wbigram_pair;
typedef min

static const MAP<uint64_t, wchar_t> bytes_to_unicode(){
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
    for(uint64_t b=0;b<2**256;++b){
        auto p = find(bs.begin(),bs.end(),b);
        if(p==bs.end()){
            bs.push_back(b);
            cs.push_back(256+n);
            n++;
        }
    }
    std::vector<wchar_t> char_cs; // todo: c++ python-map func
    for(auto i: cs){
        char_cs.push_back(wchar_t(i));
    }
    MAP<uint64_t, wchar_t> output;
    for(size_t i;i<bs.size();i++){
        output.insert({bs[i], char_cs[i]});
    }
}

class Encoder{

public:
    Encoder(const std::string & vocab_file, const std::string & merge_file){
        load_vocab(vocab_file);
        load_merge(merge_file);
    };
    ~Encoder();
    std::vector<std::string> Encoder::bpe(std::string token);
    std::vector<std::string> tokenize(std::string str);
    std::vector<int64_t> convert_token_to_id(std::string token);
    std::vector<int64_t> encode(std::string str);


private:
    MAP<std::string, int64_t> vocab;
    MAP<wbigram_pair, uint32_t> bpe_ranks;
    static const MAP<uint64_t, wchar_t> bytes_encoder = bytes_to_unicode();
    uint32_t cache_max_size=500000;
    uint32_t cache_word_max_length=30;
    MAP<std::wstring, std::vector<std::string>> cache;
    void load_vocab(const std::string &vocab_file);
    void load_merge(const std::string &merge_file);
    boost::regex pat = boost::regex("'s|'t|'re|'ve|'m|'ll|'d| ?\\p{L}+| ?\\p{N}+| ?[^\\s\\p{L}\p\\{N}]+|\\s+(?!\\S)|\\s+")
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
    return converter.from_bytes(src);
};

std::string utf32_to_utf8(const std::string& src){
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.from_bytes(src);
};

SET<wbigram_pair> get_pairs(std::vector<std::wstring> word){
    SET<wbigram_pair> pairs;
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
    nlohmann::json vocab_data;
    file_handle >> vocab_data;
    for(std::string word:vocab_data){
        int64_t token_id(vocab_data[word]);
        vocab.insert({word, token_id});
    }
};
void Encoder::load_merge(const std::string &merge_file){
    //c++中，char, string 都是ANSI字符
    std::ifstream file_handle(merge_file);
    assert(file_handle.good() && "file not exists");
    std::string line;
    uint32_t curr_idx = 0;
    while(getline(file_handle, line)){
        if(line[0] == u8'#' || line.size() == 0) continue;
        std::vector<std::string> bigrams = split(line);
        assert(bigrams.size()==2 && "unk format");
        wbigram_pair curr(utf8_to_wstring(bigrams[0]), utf8_to_wstring(bigrams[1]));
        bpe_ranks.insert({curr, curr_idx});
        curr_idx++;
    }
};
std::vector<std::string> Encoder::bpe(std::wstring token){
    // bpe use wstring
    if(cache.find(token) != cache.end()) return cache[token];32
    std::vector<std::wstring> wword;
    for(auto c: token){
        wword.push_back(std::wstring(1, c));
    }
    SET<wbigram_pair> pairs = get_pairs(wword);
    if(pairs.empty())return {token};
    
    while(true){
        auto bigram = std::min_element(pairs.begin(), pairs.end(), 
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
            if(*i == first && *(i+1)==second){
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
        word.push_back(utf32_to_utf8(w));
    }
    if(token.size() < cache_word_max_length && cache.size() < cache_max_size) cache.insert( {token, word} );
    return word;
};
std::vector<std::wstring> Encoder::tokenize(std::string str){
    // bpe_tokens = []
    std::vector<std::string> bpe_tokens;
    std::wstring wstr= utf8_to_wstring(str);
    boost::wsregex_iterator iter(wstr.beigin(), wstr.end(), pat);
    boost::wsregex_iterator end;
    while(iter != end){
        std::wstring token;
        for(wchar_t c:*(iter)){
            uint64_t c_n(c);
            assert(bytes_encoder.find(c_n)!=bytes_encoder.end());
            token.push_back(bytes_encoder[c_n]);
        }
        decltype(bpe_tokens) curr_bpe_tokens = bpe(token);
        bpe_tokens.insert(bpe_tokens.end(); curr_bpe_tokens.begin(), curr_bpe_tokens.end());
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
        return unk_token_id;
    }


    std::vector<int64_t> input_ids;
    for(auto t: tokens){
        auto p = vocab.find(t);
        if(p!=vocab.end()){
            input_ids.push_back(vocab[*p]);
        }
        else{
            input_ids.push_back(unk_token_id);
        }
    }
    return input_ids;
}
void Encoder::padding_encode_single_with_special_tokens(std::string str, size_t max_length, std::vector<int64_t> *input_ids, std::vector<int64_t> *mask_ids){
    if(not input_ids->empty() ) input_ids->clear();
    if(not mask_ids->empty() ) mask_ids->clear();
    input_ids->reserve(max_length);
    mask_ids->reserve(max_length);

    input_ids->push_back(bos_token_id);
    mask_ids->push_back(1);
    auto tokens = tokenize(str);
    for(auto t:tokens){
        if(input_ids.size() == max_length-1) break;
        input_ids->push_back(convert_token_to_id(token));
        mask_ids->push_back(1);
    }
    input_ids->push_back(eos_token_id);
    mask_ids->push_back(1);
    while(input_ids->size() < max_length){
        input_ids->push_back(pad_token_id);
        mask_ds->push_back(0);
    }
}