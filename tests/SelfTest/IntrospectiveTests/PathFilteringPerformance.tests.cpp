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

TEST_CASE( "Resolved path filters unify section and generator lookup",
           "[config][path-filter]" ) {
    std::vector<Catch::PathFilter> filters = {
        { Catch::PathFilter::For::Section, "outer" },
        { Catch::PathFilter::For::Generator, "4" },
        { Catch::PathFilter::For::Section, "leaf" },
    };

    auto const legacySection = Catch::resolveActivePathFilter(
        filters,
        false,
        99,
        0,
        Catch::PathFilter::For::Section );
    REQUIRE( legacySection.kind == Catch::ResolvedPathFilter::Kind::Section );
    REQUIRE( legacySection.filter == "outer" );

    auto const legacyGenerator = Catch::resolveActivePathFilter(
        filters,
        false,
        1,
        1,
        Catch::PathFilter::For::Generator );
    REQUIRE( legacyGenerator.kind == Catch::ResolvedPathFilter::Kind::None );

    auto const newGenerator = Catch::resolveActivePathFilter(
        filters,
        true,
        1,
        0,
        Catch::PathFilter::For::Generator );
    REQUIRE( newGenerator.kind ==
             Catch::ResolvedPathFilter::Kind::GeneratorIndex );
    REQUIRE( newGenerator.generatorIndex == 4 );

    auto const mismatchedSection = Catch::resolveActivePathFilter(
        filters,
        true,
        1,
        0,
        Catch::PathFilter::For::Section );
    REQUIRE( mismatchedSection.kind ==
             Catch::ResolvedPathFilter::Kind::TrackerMismatch );

    auto const* childFilter = Catch::pathFilterAtDepth( filters, 2 );
    REQUIRE( childFilter != nullptr );
    REQUIRE( childFilter->type == Catch::PathFilter::For::Section );
    REQUIRE( childFilter->filter == "leaf" );

    std::vector<Catch::PathFilter> wildcardFilter = {
        { Catch::PathFilter::For::Generator, "*" },
    };
    auto const wildcard = Catch::resolveActivePathFilter(
        wildcardFilter,
        true,
        0,
        0,
        Catch::PathFilter::For::Generator );
    REQUIRE( wildcard.kind ==
             Catch::ResolvedPathFilter::Kind::GeneratorWildcard );
}

TEST_CASE( "Generator path filtering keeps Take on the direct skip path",
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
        GeneratorWrapper<int>( new CountingSkipGenerator( skipCalls, nextCalls ) ) );
    REQUIRE( generator.get() == 0 );

    generator.skipToNthElement( activeFilter.generatorIndex );

    REQUIRE( generator.get() == 9 );
    REQUIRE( skipCalls == 1 );
    REQUIRE( nextCalls == 0 );
}
