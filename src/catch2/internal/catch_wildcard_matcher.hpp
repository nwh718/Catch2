
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_WILDCARD_MATCHER_HPP_INCLUDED
#define CATCH_WILDCARD_MATCHER_HPP_INCLUDED

#include &lt;catch2/catch_user_config.hpp&gt;

#if !defined( CATCH_CONFIG_NO_WILDCARD_MATCHER ) &amp;&amp; !defined( CATCH_CONFIG_WILDCARD_MATCHER )
#  define CATCH_CONFIG_WILDCARD_MATCHER
#endif

#if defined( CATCH_CONFIG_WILDCARD_MATCHER )

#include &lt;string_view&gt;
#include &lt;cstddef&gt;

namespace Catch {
    namespace Detail {

        class WildcardMatcher {
        public:
            template &lt;std::size_t N&gt;
            constexpr WildcardMatcher( const char (&amp;pattern)[N] ) noexcept
                : m_pattern( pattern ),
                  m_length( N - 1 ) {
                static_assert( N &lt;= 1025, "Wildcard pattern length must not exceed 1024 characters" );
            }

            bool matches( std::string_view input ) const noexcept;

        private:
            const char* m_pattern;
            std::size_t m_length;
        };

    } // namespace Detail
} // namespace Catch

#endif // CATCH_CONFIG_WILDCARD_MATCHER

#endif // CATCH_WILDCARD_MATCHER_HPP_INCLUDED
