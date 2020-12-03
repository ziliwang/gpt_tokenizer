### The GPT tokenizer only for deploy
The c++ version of gpt tokenizer base on https://github.com/huggingface/transformers.

Support:
 - tokenize
 - convert_token_to_id
 - padding_encode_single_with_special_tokens: encode a single string with padding mode

### exmaple
```c++
Encoder tokenizer("vocab.json", "merge.txt");
std::vector<int64_t> input_ids;
std::vector<int64_t> mask_ids;
size_t max_length=128;
tokenizer.padding_encode_single_with_special_tokens("test string", max_length, &input_ids, &mask_ids);
```
more example please see the test.cpp

### test
compiling the test.cpp and run `test <vocab.json> <merge.txt>`

### bugs
bugs may occur when windows environment as the different implement of `std::wstring`