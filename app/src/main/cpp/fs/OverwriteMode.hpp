/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2022 https://github.com/klappdev
 *
 * Permission is hereby  granted, free of charge, to any  person obtaining a copy
 * of this software and associated  documentation files (the "Software"), to deal
 * in the Software  without restriction, including without  limitation the rights
 * to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
 * copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
 * IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
 * FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
 * AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
 * LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <util/enumeration/Enumeration.hpp>

namespace kl::fs {

    enum class OverwriteMode {
        SIMPLE_MODE  = 1,
        DOE_MODE     = 3,
        OPENBSD_MODE = 4,
        RCMP_MODE    = 5,
        DOD_MODE     = 7
    };

    inline constexpr util::enumeration::Enumeration<OverwriteMode, 5> overwriteMode = {
        {OverwriteMode::SIMPLE_MODE, "SIMPLE_MODE"},
        {OverwriteMode::DOE_MODE, "DOE_MODE"},
        {OverwriteMode::OPENBSD_MODE, "OPENBSD_MODE"},
        {OverwriteMode::RCMP_MODE, "RCMP_MODE"},
        {OverwriteMode::DOD_MODE, "DOD_MODE"}
    };
}
