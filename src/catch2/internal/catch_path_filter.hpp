
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_PATH_FILTER_HPP_INCLUDED
#define CATCH_PATH_FILTER_HPP_INCLUDED

#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_optional.hpp>
#include <catch2/internal/catch_parse_numbers.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace Catch {

    struct PathFilter {
        enum class For {
            Section,
            Generator,
        };
        PathFilter( For type_, std::string filter_ ):
            type( type_ ), filter( CATCH_MOVE( filter_ ) ) {}

        For type;
        std::string filter;

        friend bool operator==( PathFilter const& lhs, PathFilter const& rhs );
    };

    struct ParsedFilter {
        PathFilter::For type;
        std::string filter;
        Optional<unsigned int> generatorIndex;
    };

    struct PathFilterStats {
        std::size_t sectionFilterCount = 0;
        std::size_t generatorFilterCount = 0;
        bool hasWildcardGeneratorFilter = false;
    };

    inline PathFilterStats computePathFilterStats(
        std::vector<PathFilter> const& filters ) {
        PathFilterStats stats;
        for ( auto const& f : filters ) {
            if ( f.type == PathFilter::For::Section ) {
                ++stats.sectionFilterCount;
            } else {
                ++stats.generatorFilterCount;
                if ( f.filter == "*" ) {
                    stats.hasWildcardGeneratorFilter = true;
                }
            }
        }
        return stats;
    }

    inline std::vector<ParsedFilter> parsePathFilters(
        std::vector<PathFilter> const& filters ) {
        std::vector<ParsedFilter> parsed;
        parsed.reserve( filters.size() );
        for ( auto const& f : filters ) {
            ParsedFilter pf;
            pf.type = f.type;
            pf.filter = f.filter;
            if ( f.type == PathFilter::For::Generator && f.filter != "*" ) {
                pf.generatorIndex = parseUInt( f.filter, 0 );
            }
            parsed.push_back( CATCH_MOVE( pf ) );
        }
        return parsed;
    }

} // end namespace Catch

#endif // CATCH_PATH_FILTER_HPP_INCLUDED
