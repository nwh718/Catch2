//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_WILDCARD_MATCHER_HPP_INCLUDED
#define CATCH_WILDCARD_MATCHER_HPP_INCLUDED

#define CATCH_CONFIG_WILDCARD_MATCHER

#include <string_view>

namespace Catch {
    namespace Detail {

        class WildcardMatcher {
            std::string_view m_pattern;
        public:
            template <std::size_t N>
            WildcardMatcher(const char (&pattern)[N]) noexcept
            : m_pattern(pattern, N - 1) {
                static_assert(N <= 1024, "Wildcard pattern length must be <= 1024");
            }

            bool matches(std::string_view input) const noexcept;
        };

    } // namespace Detail
} // namespace Catch

#endif // CATCH_WILDCARD_MATCHER_HPP_INCLUDED
