//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_wildcard_matcher.hpp>

#ifdef CATCH_CONFIG_WILDCARD_MATCHER

namespace Catch {
    namespace Detail {

        bool WildcardMatcher::matches( std::string_view input ) const noexcept {
            const char* s = input.data();
            const char* s_end = s + input.size();
            const char* p = m_pattern;
            const char* star = nullptr;
            const char* ss = s;

            while ( s < s_end ) {
                if ( ( *p == '?' ) || ( *p == *s ) ) {
                    ++s;
                    ++p;
                    continue;
                }

                if ( *p == '*' ) {
                    star = p++;
                    ss = s;
                    continue;
                }

                if ( star ) {
                    p = star + 1;
                    s = ++ss;
                    continue;
                }

                return false;
            }

            while ( *p == '*' ) {
                ++p;
            }

            return *p == '\0';
        }

    } // namespace Detail
} // namespace Catch

#endif // CATCH_CONFIG_WILDCARD_MATCHER