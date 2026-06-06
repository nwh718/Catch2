#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_config.hpp>
#include <catch2/internal/catch_path_filter.hpp>

TEST_CASE( "Path filtering resolution performance and logic", "[config][filter][performance]" ) {
    using namespace Catch;

    SECTION( "Resolve Active Path Filter" ) {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Section, "section1");
        filters.emplace_back(PathFilter::For::Generator, "5");
        filters.emplace_back(PathFilter::For::Section, "subsection");

        // When not using new filter behaviour, generator filters are ignored
        auto filter1 = resolveActivePathFilter(filters, false, 1, 1, PathFilter::For::Generator);
        REQUIRE( filter1.kind == ResolvedPathFilter::Kind::None );

        // When using new filter behaviour
        auto filter2 = resolveActivePathFilter(filters, true, 1, 1, PathFilter::For::Generator);
        REQUIRE( filter2.kind == ResolvedPathFilter::Kind::GeneratorIndex );
        REQUIRE( filter2.generatorIndex == 5 );
        REQUIRE( filter2.filter == "5" );

        auto filter3 = resolveActivePathFilter(filters, true, 0, 0, PathFilter::For::Section);
        REQUIRE( filter3.kind == ResolvedPathFilter::Kind::Section );
        REQUIRE( filter3.filter == "section1" );

        // Tracker mismatch
        auto filter4 = resolveActivePathFilter(filters, true, 0, 0, PathFilter::For::Generator);
        REQUIRE( filter4.kind == ResolvedPathFilter::Kind::TrackerMismatch );
        REQUIRE( filter4.filter == "section1" );
    }

    SECTION( "Wildcard generator filter" ) {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Generator, "*");

        auto filter = resolveActivePathFilter(filters, true, 0, 0, PathFilter::For::Generator);
        REQUIRE( filter.kind == ResolvedPathFilter::Kind::GeneratorWildcard );
        REQUIRE( filter.filter == "*" );
    }
}
