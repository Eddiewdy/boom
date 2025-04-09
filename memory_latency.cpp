#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <cmath>
#include <cstddef>  // 添加 size_t 定义

// 缓存大小定义
const size_t L1_CACHE_SIZE = 128 * 1024;         // 128KB
const size_t L2_CACHE_SIZE = 12 * 1024 * 1024;   // 12MB
const size_t ARRAY_SIZE = L2_CACHE_SIZE * 4;     // 大数组用于主存测试

// 循环次数（可调）
const int NUM_RUNS = 1;

// 读取cycle计数器的内联函数（适用于 M1/ARM 架构）
static inline uint64_t read_cycle() {
    uint64_t counter_val;
    // 启用性能计数器
    asm volatile("msr PMCR_EL0, %w0" : : "r"(1));
    // 读取性能计数器
    asm volatile("mrs %0, PMCCNTR_EL0" : "=r"(counter_val));
    return counter_val;
}

// 污染整个缓存层级，确保目标数据被驱逐
void pollute_all_caches() {
    const size_t POLLUTE_SIZE = 1 * 1024 * 1024 * 1024; // 1GB
    std::vector<uint64_t> big_array(POLLUTE_SIZE / sizeof(uint64_t), 42);
    volatile uint64_t sink = 0;
    for (size_t i = 0; i < big_array.size(); ++i) {
        sink += big_array[i];
    }
    std::cout << sink << std::endl;
}

// 计算平均值和标准差
void compute_stats(const std::vector<double> &samples, double &mean, double &stddev) {
    double sum = 0.0;
    for (auto v : samples) sum += v;
    mean = sum / samples.size();

    double var = 0.0;
    for (auto v : samples) var += (v - mean) * (v - mean);
    stddev = std::sqrt(var / samples.size());
}

// 测试主存访问延迟（随机访问）
void test_memory_latency(std::vector<uint64_t> &array) {
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<size_t> dis(0, array.size() - 1);

    std::vector<double> results;
    volatile uint64_t dummy = 0;

    for (int i = 0; i < NUM_RUNS; ++i) {
        pollute_all_caches();  // 确保命中主存
        size_t index = 0;
        uint64_t start = read_cycle();
        dummy += array[index];
        uint64_t end = read_cycle();
        results.push_back(end - start);
    }
    std::cout << dummy << std::endl;
    double mean, stddev;
    compute_stats(results, mean, stddev);
    std::cout << "[主存延迟] 平均: " << mean << " cycles, 标准差: " << stddev << std::endl;
}

// 测试 L1 延迟（热缓存命中）
void test_l1_latency(std::vector<uint64_t> &array) {
    std::vector<double> results;
    volatile uint64_t dummy = 0;

    // 预热
    dummy += array[0];

    for (int i = 0; i < NUM_RUNS; ++i) {
        uint64_t start = read_cycle();
        dummy += array[0];
        uint64_t end = read_cycle();
        results.push_back(end - start);
    }
    std::cout << dummy << std::endl;
    double mean, stddev;
    compute_stats(results, mean, stddev);
    std::cout << "[L1 缓存] 平均: " << mean << " cycles, 标准差: " << stddev << std::endl;
}

// 测试 L2 延迟（避开 L1）
void test_l2_latency(std::vector<uint64_t> &array) {
    size_t l1_lines = L1_CACHE_SIZE / sizeof(uint64_t);
    std::vector<double> results;
    volatile uint64_t dummy = 0;

    for (int i = 0; i < NUM_RUNS; ++i) {
        // Step 1: 读完整个数组，进入 L2
        for (size_t j = 0; j < array.size(); ++j) dummy += array[j];

        // Step 2: 访问前 L1 大小数据，刷新 L1 cache
        for (size_t j = 0; j < l1_lines; ++j) dummy += array[j];

        // Step 3: 测试一个刚好超出 L1 的元素
        size_t test_index = l1_lines + 256;
        uint64_t start = read_cycle();
        dummy += array[test_index];
        uint64_t end = read_cycle();
        results.push_back(end - start);
    }
    std::cout << dummy << std::endl;
    double mean, stddev;
    compute_stats(results, mean, stddev);
    std::cout << "[L2 缓存] 平均: " << mean << " cycles, 标准差: " << stddev << std::endl;
}

int main() {
    std::vector<uint64_t> array(ARRAY_SIZE);
    std::vector<uint64_t> l2_array(L2_CACHE_SIZE / sizeof(uint64_t));

    // 初始化数组内容，避免优化
    // for (size_t i = 0; i < array.size(); ++i) array[i] = i * 17 + 123;
    // for (size_t i = 0; i < l2_array.size(); ++i) l2_array[i] = i * 13 + 456;

    std::cout << "\n=== 缓存延迟测试 ===\n" << std::endl;

    test_memory_latency(array);
    test_l1_latency(array);
    test_l2_latency(l2_array);

    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}
