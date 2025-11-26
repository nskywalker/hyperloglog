#include <print>
#include <fstream>
#include <ranges>
#include <array>
#include <set>

import hyperloglog;

std::pair<void*, int> makeKeyLen(const std::string& line) {
    void* p = const_cast<char*>(line.c_str());
    return std::pair<void*, int>{p, line.length()};
}
float calc_precision(uint64_t ans, uint64_t elems)
{
    float precision = 0;
    if (ans < elems){
        precision = 1.f - (elems - ans * 1.f) / elems;
    } else {
        precision = 1.f - (ans - elems * 1.f) / elems;
    }
    return precision * 100.f;
}



void test_dataset() {
    auto test = [](uint8_t p) {
        std::ifstream in("../dataset.txt");
        HyperLogLog hll(p, makeKeyLen);
        std::string line;
        uint64_t elems = 0;
        while (in >> line){
            hll.insert(line);
            elems += 1;
        }
        auto ans = hll.calculate();
        auto precision = calc_precision(ans, elems);
        std::println("dataset random strings from file: registers = {}, unique elems = {}, HyperLogLog estimate = {}, precision = {} %", p, elems, ans, precision);
    };
    test(4);
    test(16);
    test(32);
    test(64);
    test(128);
}

void test_simple_strings() {
    std::array<std::string, 10> lines = {"sean", "rebekka", "john", "sweaty",
    "jimmy", "vasya", "nikita", "alex", "smell", "papa"};
    auto test = [&](uint8_t p)  {
        HyperLogLog hll(p, makeKeyLen);
        for (const auto& line : lines){
            hll.insert(line);
        }
        auto ans = hll.calculate();
        auto precision = calc_precision(ans, 10);
        std::println("dataset from strings: registers = {}, true unique elems = {}, HyperLogLog estimate = {}, precision {} %", p, lines.size(), ans,precision);
    };
    test(4);
    test(16);
    test(32);
    test(64);
    test(128);
}

void test_simple_numbers() {
    auto test = [&](uint8_t p) {
        HyperLogLog<uint64_t> hll(p);
        auto elems = 100ull;
        for (auto i : std::views::iota(0ull, elems)) {
            hll.insert(i);
        }
        auto ans = hll.calculate();
        auto precision = calc_precision(ans, elems);
        std::println("dataset from numbers: registers = {}, true unique elems = {}, HyperLogLog estimate = {}, precision = {} %",p, elems, ans, precision);
    };
    test(4);
    test(16);
    test(32);
    test(64);
    test(128);
}

int main()
{
    test_simple_numbers();
    test_simple_strings();
    test_dataset();
    return 0;
}
