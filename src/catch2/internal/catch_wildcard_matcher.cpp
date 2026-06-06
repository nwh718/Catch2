//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_wildcard_matcher.hpp>

namespace Catch {
    namespace Detail {

        bool WildcardMatcher::matches(std::string_view input) const noexcept {
            const char* pattern = m_pattern.data();
            const char* pattern_end = pattern + m_pattern.size();
            const char* text = input.data();
            const char* text_end = text + input.size();

            const char* star = nullptr;
            const char* match = nullptr;

            const char* p = pattern;
            const char* t = text;

            while (t < text_end) {
                if (p < pattern_end && (*p == '?' || *p == *t)) {
                    p++;
                    t++;
                } else if (p < pattern_end && *p == '*') {
                    star = p++;
                    match = t;
                } else if (star) {
                    p = star + 1;
                    match++;
                    t = match;
                } else {
                    return false;
                }
            }

            while (p < pattern_end && *p == '*') {
                p++;
            }

            return p == pattern_end;
        }

    } // namespace Detail
} // namespace Catch
