#include <algorithm>
#include <cassert>
#include <functional>
#include <ranges>
#include <string>
#include <vector>

std::vector<int> to_digits(int64_t num) {
    std::string str = std::to_string(num);
    return std::ranges::to<std::vector<int>>(
        str | std::views::transform([](char c) { return c - '0'; }));
}

auto flatten_digits() {
    return std::views::transform([](int digit) {
               if (digit < 10) {
                   return std::vector{digit};
               }
               return to_digits(digit);
           }) |
           std::views::join | std::ranges::to<std::vector<int>>();
}

bool validate(int64_t card_number) {
    if (card_number <= 0)
        return false;

    auto card_digits = to_digits(card_number);
    auto reversed_digits = card_digits | std::views::reverse;
    auto doubled_digits =
        std::views::zip(std::views::iota(0), reversed_digits) |
        std::views::transform([](const auto &pair) {
            auto [i, d] = pair;
            return i % 2 == 1 ? d * 2 : d;
        }) |
        flatten_digits();
    int sum = std::ranges::fold_left(doubled_digits, 0, std::plus{});
    return sum % 10 == 0;
}

int main(int argc, char *argv[]) {
    assert(validate(0) == false);
    assert(validate(-1) == false);

    assert(validate(4012888888881881));
    assert(!validate(4012888888881882));

    assert(validate(4532015112830366));
    assert(!validate(4532015112830367));
    return 0;
}
