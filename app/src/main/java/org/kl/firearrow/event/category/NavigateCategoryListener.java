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

package org.kl.firearrow.event.category;

import androidx.annotation.NonNull;
import androidx.navigation.Navigation;

import org.kl.firearrow.R;
import org.kl.firearrow.ui.MainActivity;
import org.kl.firearrow.ui.category.CategoryAdapter;
import org.kl.firearrow.ui.category.CategoryFragment;
import org.kl.firearrow.ui.category.CategoryFragmentDirections;

public final class NavigateCategoryListener {
    private final MainActivity activity;
    private final CategoryAdapter adapter;

    public NavigateCategoryListener(@NonNull CategoryFragment fragment) {
        this.activity = fragment.getParentActivity();
        this.adapter = fragment.getCategoryAdapter();
    }

    public void navigateShowFeature() {
        final long categoryId = adapter.getCurrentItemId();

        if (categoryId != -1L) {
            final var direction = CategoryFragmentDirections
                .actionCategoryFragmentToFeatureActivity();
            direction.setCategoryId(categoryId);

            final var navigationController = Navigation.findNavController(activity, R.id.navigation_host_fragment);
            navigationController.navigate(direction);

        }
    }
}
