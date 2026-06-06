
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include &lt;catch2/internal/catch_wildcard_matcher.hpp&gt;

#if defined( CATCH_CONFIG_WILDCARD_MATCHER )

namespace Catch {
    namespace Detail {

        bool WildcardMatcher::matches( std::string_view input ) const noexcept {
            std::size_t patternIdx = 0;
            std::size_t inputIdx = 0;
            std::size_t starIdx = static_cast&lt;std::size_t&gt;( -1 );
            std::size_t matchIdx = static_cast&lt;std::size_t&gt;( -1 );

            while ( inputIdx &lt; input.size() ) {
                if ( patternIdx &lt; m_length &amp;&amp;
                     ( m_pattern[patternIdx] == input[inputIdx] ||
                       m_pattern[patternIdx] == '?' ) ) {
                    ++patternIdx;
                    ++inputIdx;
                }
                else if ( patternIdx &lt; m_length &amp;&amp; m_pattern[patternIdx] == '*' ) {
                    starIdx = patternIdx++;
                    matchIdx = inputIdx;
                }
                else if ( starIdx != static_cast&lt;std::size_t&gt;( -1 ) ) {
                    patternIdx = starIdx + 1;
                    inputIdx = ++matchIdx;
                }
                else {
                    return false;
                }
            }

            while ( patternIdx &lt; m_length &amp;&amp; m_pattern[patternIdx] == '*' ) {
                ++patternIdx;
            }

            return patternIdx == m_length;
        }

    } // namespace Detail
} // namespace Catch

#endif // CATCH_CONFIG_WILDCARD_MATCHER
