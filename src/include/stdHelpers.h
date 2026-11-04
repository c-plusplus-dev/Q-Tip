//
// Created by Ayaan on 2026-11-04.
//

#ifndef QTIP_STDHELPERS_H
#define QTIP_STDHELPERS_H

#include <iterator>
#include <ranges>
#include <utility>

namespace QTip::Detail {
    template <std::ranges::input_range range_T, typename iter_T>
        requires requires(range_T& r, iter_T i) {
            *i;
            r.erase(i);
        }
    [[nodiscard]] constexpr std::ranges::range_value_t<range_T>
    extract(range_T&& r, iter_T i) {
        using value_T = std::ranges::range_value_t<range_T>;

        value_T data = std::move(*i);
        r.erase(i);

        return data;
    }

    template <std::ranges::input_range range_T>
        requires requires(range_T& r) {
            r.front();
            r.erase(r.begin());
        }
    [[nodiscard]] constexpr std::ranges::range_value_t<range_T>
    extract_front(range_T&& r) {
        using value_T = std::ranges::range_value_t<range_T>;

        value_T data = std::move(r.front());
        r.erase(r.begin());

        return data;
    }

    template <std::ranges::bidirectional_range range_T>
        requires requires(range_T& r) {
            r.back();
            r.erase(std::prev(r.end()));
        }
    [[nodiscard]] constexpr std::ranges::range_value_t<range_T>
    extract_back(range_T&& r) {
        using value_T = std::ranges::range_value_t<range_T>;

        value_T data = std::move(r.back());
        r.erase(std::prev(r.end()));

        return data;
    }
}

#endif //QTIP_STDHELPERS_H