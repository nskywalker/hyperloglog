module;

#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <bit>

export module hyperloglog;
import murmurhash;

float calc_func_val(const uint8_t p, const float xf) {
    auto pf = static_cast<float>(p);
    auto drob = (2.f + xf) / (1.f + xf);
    return std::powf(std::log2(drob), pf);
}

inline float calc_square(float y1, float y2, float h) {
    return 0.5f * (y1 + y2) * h;
}

float calc_alpha(const uint8_t p) {
    const float step = 0.001f;
    auto prev_val = calc_func_val(p, 0);
    auto next_val = calc_func_val(p, step);
    float i = 2.f * step;
    float S = calc_square(prev_val, next_val, step);
    while (std::fabs(prev_val - next_val) > 0.000000001f) {
        prev_val = next_val;
        next_val = calc_func_val(p, i);
        i += step;
        S += calc_square(prev_val, next_val, step);
    }
    return 1.f / (S * static_cast<float>(p));
}

export template <typename T>
class HyperLogLog {
    uint8_t precision;
    uint8_t bits;
    std::vector<uint64_t> dense;
    float alpha;
    using TypeRepresent = std::pair<void*, int>(*)(const T&);
    TypeRepresent tf;
public:
    explicit HyperLogLog(uint8_t p, TypeRepresent typeRepresentFunc = nullptr);
    void insert(const T& item);
    uint64_t calculate() const;
    ~HyperLogLog() = default;
};

template<typename T>
std::pair<void*, int> getTypeReprBase(const T& t) {
    auto p = reinterpret_cast<void*>(&const_cast<T&>(t));
    return std::pair<void*, int>{p, sizeof(T)};
}

template<typename T>
HyperLogLog<T>::HyperLogLog(uint8_t p, TypeRepresent typeRepresentFunc) : precision(p), bits(0), alpha(calc_alpha(p)), tf(typeRepresentFunc ? typeRepresentFunc : getTypeReprBase<T>) {
    auto p_f = static_cast<float>(precision);
    p_f = std::log2(p_f);
    bits = static_cast<uint8_t>(std::ceilf(p_f));
    dense.resize(precision, 0);
}

template<typename T>
void HyperLogLog<T>::insert(const T &item) {
    auto [key, len] = tf(item);
    auto hash = murmurhash3_x64_128(key, len, 0);
    auto shift = sizeof(hash) * 8 - static_cast<uint64_t>(bits);
    auto bucket_num = (hash >> shift);
    uint64_t num_zeros = std::countl_zero(hash << bits) + 1;
    dense[bucket_num] = std::max(num_zeros, dense[bucket_num]);
}

template<typename T>
uint64_t HyperLogLog<T>::calculate() const {
    auto V = std::ranges::count(dense, 0);
    auto p_f = static_cast<float>(precision);
    if (V != 0) {
        return p_f * std::log(p_f / V);
    }
    float Z = 0;
    for (auto v : dense) {
        auto p = -static_cast<float>(v);
        Z += std::powf(2, p);
    }
    Z = 1.f / Z;
    return alpha * std::powf(p_f, 2.f) * Z;
}
