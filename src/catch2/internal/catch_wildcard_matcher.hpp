//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_WILDCARD_MATCHER_HPP_INCLUDED
#define CATCH_WILDCARD_MATCHER_HPP_INCLUDED

#ifdef CATCH_CONFIG_WILDCARD_MATCHER

#include <string_view>

namespace Catch {
    namespace Detail {

        class WildcardMatcher {
            static constexpr size_t MaxPatternLength = 1024;

        public:
            template <size_t N>
            explicit constexpr WildcardMatcher( const char (&pattern)[N] ) noexcept;

            bool matches( std::string_view input ) const noexcept;

        private:
            char m_pattern[MaxPatternLength + 1] = {};
            size_t m_length = 0;
        };

        template <size_t N>
        constexpr WildcardMatcher::WildcardMatcher( const char (&pattern)[N] ) noexcept {
            static_assert( N <= MaxPatternLength + 1,
                           "WildcardMatcher: pattern length exceeds maximum of 1024 characters" );
            for ( size_t i = 0; i < N; ++i ) {
                m_pattern[i] = pattern[i];
            }
            m_length = N - 1;
        }

    } // namespace Detail
} // namespace Catch

#endif // CATCH_CONFIG_WILDCARD_MATCHER

#endif // CATCH_WILDCARD_MATCHER_HPP_INCLUDED