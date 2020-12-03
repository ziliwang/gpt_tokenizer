#include "gpt_encoder.cpp"

int main(int argc, char * argv[]){
    using namespace std;
    assert(argc ==3 && "usage: test <vocab> <merge>");
    Encoder test(argv[1], argv[2]);
    size_t exmaple_length =10;
    unordered_map<string, pair<vector<int64_t>, vector<int64_t>>> exmaples;
    exmaples[u8"this is   a test example!!!"] = {{0, 16541, 2619, 4391, 271, 10505, 5499, 2548, 3747, 2}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    exmaples[u8"Tribunal Supremo tendrá a los prisión"] = {{0, 5725, 11056, 3160, 271, 316, 7582, 2, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 0, 0}};
    exmaples[u8"assé .. sss 哈練ウ페 يَّةُ ру́сский"] = {{0, 496, 36307, 6050, 273, 7762, 43264, 246, 235, 2}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    
    std::vector<int64_t> input_ids;
    std::vector<int64_t> mask_ids;
    for(auto key=exmaples.begin();key != exmaples.end();key++){
        cout << "test: '" << key->first << "'\n";
        test.padding_encode_single_with_special_tokens(key->first, exmaple_length, &input_ids, &mask_ids);
        for(size_t i =0;i<input_ids.size();++i){
            assert(input_ids[i] == key->second.first[i] && "input id mismatch");
            assert(mask_ids[i] == key->second.second[i] && "mask id mismatch");
        }
    }
}