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
package org.kl.firearrow.ui.feature;

import android.os.Build;
import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.paging.PagingDataAdapter;

import lombok.Setter;

import org.kl.firearrow.databinding.FeatureItemBinding;
import org.kl.firearrow.event.diff.FeatureDifferenceCallback;
import org.kl.firearrow.event.feature.ChooseFeatureListener;
import org.kl.firearrow.event.feature.FeatureMapper;
import org.kl.firearrow.model.Feature;

public final class FeatureAdapter extends PagingDataAdapter<Feature, FeatureViewHolder> {
    private final FeatureMapper featureMapper;
    @Setter
    private int position = -1;

    public FeatureAdapter(@NonNull FeatureMapper featureMapper) {
        super(new FeatureDifferenceCallback());
        this.featureMapper = featureMapper;
    }

    @NonNull
    @Override
    public FeatureViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        final var inflater = LayoutInflater.from(parent.getContext());
        final var binding = FeatureItemBinding.inflate(inflater, parent, false);

        return new FeatureViewHolder(binding);
    }

    @Override
    @RequiresApi(api = Build.VERSION_CODES.R)
    public void onBindViewHolder(@NonNull FeatureViewHolder holder, int position) {
        final var feature = getItem(position);

        if (feature != null) {
            final var mapper = featureMapper.findById((int) feature.getId());
            holder.bind(feature);
            holder.getBindingRoot().setOnClickListener(new ChooseFeatureListener(mapper));
        }
    }

    public Feature getCurrentItem() {
        return getItem(position);
    }

    public long getCurrentItemId() {
        final var item = getItem(position);
        return item != null ? item.getId() : -1L;
    }
}
