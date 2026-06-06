//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_WILDCARD_MATCHER_HPP_INCLUDED
#define CATCH_WILDCARD_MATCHER_HPP_INCLUDED

#include <catch2/catch_user_config.hpp>

#if !defined( CATCH_CONFIG_NO_WILDCARD_MATCHER ) && \
    !defined( CATCH_CONFIG_WILDCARD_MATCHER )
#    define CATCH_CONFIG_WILDCARD_MATCHER
#endif

#include <array>
#include <cstddef>
#include <string_view>

namespace Catch {

#if defined( CATCH_CONFIG_WILDCARD_MATCHER )
    class WildcardMatcher {
        static constexpr std::size_t max_pattern_size = 1024;

    public:
        template <std::size_t N>
        explicit WildcardMatcher( char const (&pattern)[N] ) noexcept:
            m_pattern_size( N - 1 ) {
            static_assert( N > 0, "Wildcard pattern must be null-terminated" );
            static_assert( N - 1 <= max_pattern_size,
                           "Wildcard pattern length must be at most 1024" );

            for( std::size_t i = 0; i < m_pattern_size; ++i ) {
                m_pattern[i] = pattern[i];
            }
        }

        bool matches( std::string_view input ) const noexcept;

    private:
        std::array<char, max_pattern_size> m_pattern{};
        std::size_t m_pattern_size;
    };
#endif

} // namespace Catch

#endif // CATCH_WILDCARD_MATCHER_HPP_INCLUDED
