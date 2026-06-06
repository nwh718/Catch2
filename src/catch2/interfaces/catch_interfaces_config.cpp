
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_stringref.hpp>

namespace Catch {
    IConfig::~IConfig() = default;

    bool IConfig::matchesPathFilter(StringRef) const {
        return true;
    }
}
