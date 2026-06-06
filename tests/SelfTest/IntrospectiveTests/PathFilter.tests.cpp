//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_path_filter.hpp>
#include <catch2/catch_config.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

#include <vector>

using namespace Catch;

TEST_CASE("computePathFilterStats counts filter types", "[path-filter][stats]") {
    SECTION("empty filters") {
        std::vector<PathFilter> empty;
        auto stats = computePathFilterStats(empty);
        REQUIRE(stats.sectionFilterCount == 0);
        REQUIRE(stats.generatorFilterCount == 0);
        REQUIRE_FALSE(stats.hasWildcardGeneratorFilter);
    }

    SECTION("only section filters") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Section, "A");
        filters.emplace_back(PathFilter::For::Section, "B");
        auto stats = computePathFilterStats(filters);
        REQUIRE(stats.sectionFilterCount == 2);
        REQUIRE(stats.generatorFilterCount == 0);
        REQUIRE_FALSE(stats.hasWildcardGeneratorFilter);
    }

    SECTION("only generator filters") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Generator, "3");
        filters.emplace_back(PathFilter::For::Generator, "7");
        auto stats = computePathFilterStats(filters);
        REQUIRE(stats.sectionFilterCount == 0);
        REQUIRE(stats.generatorFilterCount == 2);
        REQUIRE_FALSE(stats.hasWildcardGeneratorFilter);
    }

    SECTION("wildcard generator filter") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Generator, "*");
        auto stats = computePathFilterStats(filters);
        REQUIRE(stats.generatorFilterCount == 1);
        REQUIRE(stats.hasWildcardGeneratorFilter);
    }

    SECTION("mixed filters") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Section, "A");
        filters.emplace_back(PathFilter::For::Generator, "5");
        filters.emplace_back(PathFilter::For::Section, "B");
        filters.emplace_back(PathFilter::For::Generator, "*");
        auto stats = computePathFilterStats(filters);
        REQUIRE(stats.sectionFilterCount == 2);
        REQUIRE(stats.generatorFilterCount == 2);
        REQUIRE(stats.hasWildcardGeneratorFilter);
    }
}

TEST_CASE("parsePathFilters parses generator indices", "[path-filter][parse]") {
    SECTION("empty filters") {
        std::vector<PathFilter> empty;
        auto parsed = parsePathFilters(empty);
        REQUIRE(parsed.empty());
    }

    SECTION("section filters are not parsed as indices") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Section, "A");
        auto parsed = parsePathFilters(filters);
        REQUIRE(parsed.size() == 1);
        REQUIRE(parsed[0].type == PathFilter::For::Section);
        REQUIRE(parsed[0].filter == "A");
        REQUIRE_FALSE(parsed[0].generatorIndex);
    }

    SECTION("generator filter with numeric index") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Generator, "42");
        auto parsed = parsePathFilters(filters);
        REQUIRE(parsed.size() == 1);
        REQUIRE(parsed[0].type == PathFilter::For::Generator);
        REQUIRE(parsed[0].filter == "42");
        REQUIRE(parsed[0].generatorIndex);
        REQUIRE(*parsed[0].generatorIndex == 42);
    }

    SECTION("wildcard generator filter is not parsed as index") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Generator, "*");
        auto parsed = parsePathFilters(filters);
        REQUIRE(parsed.size() == 1);
        REQUIRE(parsed[0].type == PathFilter::For::Generator);
        REQUIRE(parsed[0].filter == "*");
        REQUIRE_FALSE(parsed[0].generatorIndex);
    }

    SECTION("non-numeric generator filter") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Generator, "abc");
        auto parsed = parsePathFilters(filters);
        REQUIRE(parsed.size() == 1);
        REQUIRE(parsed[0].type == PathFilter::For::Generator);
        REQUIRE_FALSE(parsed[0].generatorIndex);
    }

    SECTION("mixed filters preserve order") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Section, "A");
        filters.emplace_back(PathFilter::For::Generator, "3");
        filters.emplace_back(PathFilter::For::Section, "B");
        filters.emplace_back(PathFilter::For::Generator, "*");
        auto parsed = parsePathFilters(filters);
        REQUIRE(parsed.size() == 4);
        REQUIRE(parsed[0].type == PathFilter::For::Section);
        REQUIRE(parsed[1].type == PathFilter::For::Generator);
        REQUIRE(*parsed[1].generatorIndex == 3);
        REQUIRE(parsed[2].type == PathFilter::For::Section);
        REQUIRE(parsed[3].type == PathFilter::For::Generator);
        REQUIRE_FALSE(parsed[3].generatorIndex);
    }
}

TEST_CASE("Config unified filter interface", "[path-filter][config]") {
    ConfigData data;
    data.useNewPathFilteringBehaviour = true;

    SECTION("validatePathFilters with empty filters") {
        Config config(data);
        REQUIRE(config.validatePathFilters());
    }

    SECTION("validatePathFilters with valid generator indices") {
        data.pathFilters.emplace_back(PathFilter::For::Section, "A");
        data.pathFilters.emplace_back(PathFilter::For::Generator, "3");
        Config config(data);
        REQUIRE(config.validatePathFilters());
    }

    SECTION("validatePathFilters with invalid generator index") {
        data.pathFilters.emplace_back(PathFilter::For::Generator, "abc");
        Config config(data);
        REQUIRE_FALSE(config.validatePathFilters());
    }

    SECTION("validatePathFilters with wildcard generator") {
        data.pathFilters.emplace_back(PathFilter::For::Generator, "*");
        Config config(data);
        REQUIRE(config.validatePathFilters());
    }

    SECTION("getPathFilterStats returns correct counts") {
        data.pathFilters.emplace_back(PathFilter::For::Section, "A");
        data.pathFilters.emplace_back(PathFilter::For::Generator, "3");
        data.pathFilters.emplace_back(PathFilter::For::Section, "B");
        Config config(data);
        auto stats = config.getPathFilterStats();
        REQUIRE(stats.sectionFilterCount == 2);
        REQUIRE(stats.generatorFilterCount == 1);
    }

    SECTION("getParsedPathFilters returns parsed data") {
        data.pathFilters.emplace_back(PathFilter::For::Section, "A");
        data.pathFilters.emplace_back(PathFilter::For::Generator, "7");
        Config config(data);
        auto parsed = config.getParsedPathFilters();
        REQUIRE(parsed.size() == 2);
        REQUIRE(parsed[0].type == PathFilter::For::Section);
        REQUIRE(parsed[1].type == PathFilter::For::Generator);
        REQUIRE(parsed[1].generatorIndex);
        REQUIRE(*parsed[1].generatorIndex == 7);
    }

    SECTION("getPathFilterStats with empty config") {
        Config config(data);
        auto stats = config.getPathFilterStats();
        REQUIRE(stats.sectionFilterCount == 0);
        REQUIRE(stats.generatorFilterCount == 0);
    }

    SECTION("getParsedPathFilters with empty config") {
        Config config(data);
        auto parsed = config.getParsedPathFilters();
        REQUIRE(parsed.empty());
    }

    SECTION("useNewFilterBehaviour is reflected") {
        Config config(data);
        REQUIRE(config.useNewFilterBehaviour());
    }

    SECTION("old-style filters still work") {
        ConfigData oldData;
        Config config(oldData);
        REQUIRE_FALSE(config.useNewFilterBehaviour());
        REQUIRE(config.validatePathFilters());
    }
}

TEST_CASE("TakeGenerator skip interaction with filter validation",
          "[path-filter][generators][take][performance]") {
    using namespace Catch::Generators;

    SECTION("skip within bounds uses TakeGenerator::skipToNthElementImpl") {
        TakeGenerator<int> take(
            10, values({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));
        REQUIRE(take.currentElementIndex() == 0);

        take.skipToNthElement(5);
        REQUIRE(take.currentElementIndex() == 5);
        REQUIRE(take.get() == 5);
    }

    SECTION("skip beyond TakeGenerator bounds throws") {
        TakeGenerator<int> take(
            5, values({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
        REQUIRE_THROWS(take.skipToNthElement(7));
    }

    SECTION("skip to same element is no-op") {
        TakeGenerator<int> take(
            10, values({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
        take.skipToNthElement(3);
        REQUIRE(take.currentElementIndex() == 3);

        take.skipToNthElement(3);
        REQUIRE(take.currentElementIndex() == 3);
        REQUIRE(take.get() == 3);
    }

    SECTION("skip backwards in TakeGenerator throws") {
        TakeGenerator<int> take(
            10, values({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
        take.skipToNthElement(5);
        REQUIRE_THROWS(take.skipToNthElement(3));
    }
}

TEST_CASE("Filter stats reflect filter depth for generators",
          "[path-filter][generators][depth]") {
    SECTION("filter depth matches generator nesting") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Section, "A");
        filters.emplace_back(PathFilter::For::Generator, "3");
        filters.emplace_back(PathFilter::For::Section, "B");
        filters.emplace_back(PathFilter::For::Generator, "*");

        auto stats = computePathFilterStats(filters);
        REQUIRE(stats.sectionFilterCount == 2);
        REQUIRE(stats.generatorFilterCount == 2);
        REQUIRE(stats.hasWildcardGeneratorFilter);
    }
}

TEST_CASE("Parsed filter preserves generator index for runtime use",
          "[path-filter][generators][runtime]") {
    using namespace Catch::Generators;

    SECTION("parsed filter index can be used with skipToNthElement") {
        std::vector<PathFilter> filters;
        filters.emplace_back(PathFilter::For::Generator, "4");

        auto parsed = parsePathFilters(filters);
        REQUIRE(parsed.size() == 1);
        REQUIRE(parsed[0].generatorIndex);
        auto targetIndex = static_cast<std::size_t>(*parsed[0].generatorIndex);

        TakeGenerator<int> take(
            10, values({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
        take.skipToNthElement(targetIndex);
        REQUIRE(take.currentElementIndex() == 4);
        REQUIRE(take.get() == 4);
    }
}