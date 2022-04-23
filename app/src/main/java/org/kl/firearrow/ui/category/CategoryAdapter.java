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
package org.kl.firearrow.ui.category;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ListAdapter;

import lombok.Getter;
import lombok.Setter;

import org.kl.firearrow.db.entity.Category;
import org.kl.firearrow.databinding.CategoryItemBinding;
import org.kl.firearrow.event.diff.CategoryDifferenceCallback;
import org.kl.firearrow.event.category.ChooseCategoryListener;

public final class CategoryAdapter extends ListAdapter<Category, CategoryViewHolder> {
    @Getter @Setter
    private Runnable navigateAction;

    @Setter
    private int position = -1;

    public CategoryAdapter() {
        super(new CategoryDifferenceCallback());
    }

    @NonNull
    @Override
    public CategoryViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        final var inflater = LayoutInflater.from(parent.getContext());
        final var binding = CategoryItemBinding.inflate(inflater, parent, false);

        final var viewHolder = new CategoryViewHolder(binding);
        binding.categoryCardView.setOnClickListener(new ChooseCategoryListener(viewHolder));

        return viewHolder;
    }

    @Override
    public void onBindViewHolder(@NonNull CategoryViewHolder holder, int position) {
        final var category = getItem(position);
        holder.bind(category);
    }

    public Category getCurrentItem() {
        return getItem(position);
    }

    public long getCurrentItemId() {
        return getItem(position).getId();
    }
}
