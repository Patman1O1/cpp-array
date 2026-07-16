// ISO C Includes
#include <cstdlib>

// ISO C++ Includes
#include <iostream>

int main(const int argc, char* argv[]) {
    if (argc < 3) [[unlikely]] {
        std::println(std::cerr, "Usage {} <percent1> <percent2>", argv[0]);
        return 1;
    }

    const double percent1 = std::stod(argv[1]);
    const double percent2 = std::stod(argv[2]); // 3.01 2.33

    const double diff = std::abs(percent1 - percent2) / ((percent1 + percent2) / 2) * 100;
    std::println(std::cout, "{} %", diff);
    return 0;
}