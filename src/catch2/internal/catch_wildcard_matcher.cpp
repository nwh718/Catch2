//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_wildcard_matcher.hpp>

namespace Catch {

#if defined( CATCH_CONFIG_WILDCARD_MATCHER )
    bool WildcardMatcher::matches( std::string_view input ) const noexcept {
        std::size_t pattern_index = 0;
        std::size_t input_index = 0;
        std::size_t starred_pattern_index = std::string_view::npos;
        std::size_t restart_input_index = 0;

        while( input_index < input.size() ) {
            if( pattern_index < m_pattern_size ) {
                char const pattern_char = m_pattern[pattern_index];

                if( pattern_char == '*' ) {
                    starred_pattern_index = pattern_index++;
                    restart_input_index = input_index;
                    continue;
                }

                if( pattern_char == '?' || pattern_char == input[input_index] ) {
                    ++pattern_index;
                    ++input_index;
                    continue;
                }
            }

            if( starred_pattern_index != std::string_view::npos ) {
                pattern_index = starred_pattern_index + 1;
                input_index = ++restart_input_index;
                continue;
            }

            return false;
        }

        while( pattern_index < m_pattern_size && m_pattern[pattern_index] == '*' ) {
            ++pattern_index;
        }

        return pattern_index == m_pattern_size;
    }
#endif

} // namespace Catch
