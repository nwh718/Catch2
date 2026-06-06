
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_config.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/internal/catch_path_filter.hpp>

#include <vector>

namespace {
    struct CountingSkipGenerator final : Catch::Generators::IGenerator<int> {
        int m_current = 0;
        int* m_skipCalls;
        int* m_nextCalls;

        CountingSkipGenerator( int& skipCalls, int& nextCalls ):
            m_skipCalls( &skipCalls ),
            m_nextCalls( &nextCalls ) {}

        int const& get() const override { return m_current; }

        bool next() override {
            ++*m_nextCalls;
            ++m_current;
            return true;
        }

        void skipToNthElementImpl( std::size_t n ) override {
            ++*m_skipCalls;
            m_current = static_cast<int>( n );
        }

        bool isFinite() const override { return true; }
    };
}

TEST_CASE( "ResolvedPathFilter unifies section and generator lookup",
           "[config][path-filter]" ) {
    using namespace Catch;

    std::vector<PathFilter> filters = {
        { PathFilter::For::Section, "outer" },
        { PathFilter::For::Generator, "4" },
        { PathFilter::For::Section, "leaf" },
    };

    SECTION( "Legacy mode ignores generator filters" ) {
        auto const result = resolveActivePathFilter(
            filters,
            false,
            1,
            1,
            PathFilter::For::Generator );
        REQUIRE( result.kind == ResolvedPathFilter::Kind::None );
    }

    SECTION( "New mode resolves generator index" ) {
        auto const result = resolveActivePathFilter(
            filters,
            true,
            1,
            0,
            PathFilter::For::Generator );
        REQUIRE( result.kind ==
                 ResolvedPathFilter::Kind::GeneratorIndex );
        REQUIRE( result.generatorIndex == 4 );
        REQUIRE( result.filter == "4" );
    }

    SECTION( "New mode resolves section filter" ) {
        auto const result = resolveActivePathFilter(
            filters,
            true,
            0,
            0,
            PathFilter::For::Section );
        REQUIRE( result.kind == ResolvedPathFilter::Kind::Section );
        REQUIRE( result.filter == "outer" );
    }

    SECTION( "Tracker mismatch when filter type differs" ) {
        auto const result = resolveActivePathFilter(
            filters,
            true,
            0,
            0,
            PathFilter::For::Generator );
        REQUIRE( result.kind ==
                 ResolvedPathFilter::Kind::TrackerMismatch );
        REQUIRE( result.filter == "outer" );
    }

    SECTION( "Wildcard generator filter" ) {
        std::vector<PathFilter> wildcardFilter = {
            { PathFilter::For::Generator, "*" },
        };
        auto const result = resolveActivePathFilter(
            wildcardFilter,
            true,
            0,
            0,
            PathFilter::For::Generator );
        REQUIRE( result.kind ==
                 ResolvedPathFilter::Kind::GeneratorWildcard );
        REQUIRE( result.filter == "*" );
    }

    SECTION( "No filter at depth returns None" ) {
        auto const result = resolveActivePathFilter(
            filters,
            true,
            99,
            99,
            PathFilter::For::Section );
        REQUIRE( result.kind == ResolvedPathFilter::Kind::None );
    }
}

TEST_CASE( "pathFilterAtDepth returns correct pointer",
           "[config][path-filter]" ) {
    using namespace Catch;

    std::vector<PathFilter> filters = {
        { PathFilter::For::Section, "A" },
        { PathFilter::For::Generator, "2" },
    };

    SECTION( "Valid depth returns pointer" ) {
        auto const* f = pathFilterAtDepth( filters, 0 );
        REQUIRE( f != nullptr );
        REQUIRE( f->type == PathFilter::For::Section );
        REQUIRE( f->filter == "A" );
    }

    SECTION( "Second depth returns pointer" ) {
        auto const* f = pathFilterAtDepth( filters, 1 );
        REQUIRE( f != nullptr );
        REQUIRE( f->type == PathFilter::For::Generator );
        REQUIRE( f->filter == "2" );
    }

    SECTION( "Out-of-bounds depth returns nullptr" ) {
        auto const* f = pathFilterAtDepth( filters, 2 );
        REQUIRE( f == nullptr );
    }

    SECTION( "Empty filters always return nullptr" ) {
        std::vector<PathFilter> empty;
        auto const* f = pathFilterAtDepth( empty, 0 );
        REQUIRE( f == nullptr );
    }
}

TEST_CASE( "Generator path filtering uses direct skip path for TakeGenerator",
           "[generators][take][path-filter][performance][regression]" ) {
    using namespace Catch::Generators;

    int skipCalls = 0;
    int nextCalls = 0;

    std::vector<Catch::PathFilter> filters = {
        { Catch::PathFilter::For::Section, "outer" },
        { Catch::PathFilter::For::Generator, "9" },
        { Catch::PathFilter::For::Section, "leaf" },
    };

    auto const activeFilter = Catch::resolveActivePathFilter(
        filters,
        true,
        1,
        0,
        Catch::PathFilter::For::Generator );
    REQUIRE( activeFilter.kind ==
             Catch::ResolvedPathFilter::Kind::GeneratorIndex );

    auto generator = take(
        16,
        GeneratorWrapper<int>(
            new CountingSkipGenerator( skipCalls, nextCalls ) ) );
    REQUIRE( generator.get() == 0 );

    generator.skipToNthElement( activeFilter.generatorIndex );

    REQUIRE( generator.get() == 9 );
    REQUIRE( skipCalls == 1 );
    REQUIRE( nextCalls == 0 );
}

TEST_CASE( "FilterGenerator does not interfere with skipToNthElement",
           "[generators][filter][path-filter][performance][regression]" ) {
    using namespace Catch::Generators;

    int skipCalls = 0;
    int nextCalls = 0;

    auto generator = filter(
        []( int const& ) { return true; },
        GeneratorWrapper<int>(
            new CountingSkipGenerator( skipCalls, nextCalls ) ) );

    REQUIRE( generator.get() == 0 );
    REQUIRE( nextCalls == 0 );
    REQUIRE( skipCalls == 0 );

    generator.skipToNthElement( 7 );

    REQUIRE( generator.get() == 7 );
    REQUIRE( skipCalls == 1 );
    REQUIRE( nextCalls == 0 );
}

TEST_CASE( "ResolvedPathFilter with multiple generator indices",
           "[config][path-filter][performance]" ) {
    using namespace Catch;

    std::vector<PathFilter> filters = {
        { PathFilter::For::Generator, "0" },
        { PathFilter::For::Section, "inner" },
        { PathFilter::For::Generator, "42" },
    };

    SECTION( "First generator index is 0" ) {
        auto const result = resolveActivePathFilter(
            filters,
            true,
            0,
            0,
            PathFilter::For::Generator );
        REQUIRE( result.kind ==
                 ResolvedPathFilter::Kind::GeneratorIndex );
        REQUIRE( result.generatorIndex == 0 );
    }

    SECTION( "Deep generator index is 42" ) {
        auto const result = resolveActivePathFilter(
            filters,
            true,
            2,
            0,
            PathFilter::For::Generator );
        REQUIRE( result.kind ==
                 ResolvedPathFilter::Kind::GeneratorIndex );
        REQUIRE( result.generatorIndex == 42 );
    }
}

TEST_CASE( "Legacy section filter depth uses sectionOnlyDepth",
           "[config][path-filter]" ) {
    using namespace Catch;

    std::vector<PathFilter> filters = {
        { PathFilter::For::Section, "A" },
        { PathFilter::For::Section, "B" },
    };

    SECTION( "Legacy mode uses sectionOnlyDepth" ) {
        auto const result = resolveActivePathFilter(
            filters,
            false,
            5,
            1,
            PathFilter::For::Section );
        REQUIRE( result.kind == ResolvedPathFilter::Kind::Section );
        REQUIRE( result.filter == "B" );
    }

    SECTION( "New mode uses allTrackerDepth" ) {
        auto const result = resolveActivePathFilter(
            filters,
            true,
            0,
            1,
            PathFilter::For::Section );
        REQUIRE( result.kind == ResolvedPathFilter::Kind::Section );
        REQUIRE( result.filter == "A" );
    }
}
