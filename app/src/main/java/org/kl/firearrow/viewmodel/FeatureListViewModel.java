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
package org.kl.firearrow.viewmodel;

import androidx.annotation.NonNull;
import androidx.lifecycle.SavedStateHandle;
import androidx.lifecycle.ViewModel;
import androidx.paging.PagingData;

import dagger.hilt.android.lifecycle.HiltViewModel;
import io.reactivex.rxjava3.core.Observable;

import javax.inject.Inject;

import org.kl.firearrow.db.repo.FeatureRepository;
import org.kl.firearrow.db.entity.Feature;

import java.util.Optional;

@HiltViewModel
public final class FeatureListViewModel extends ViewModel {
    private final SavedStateHandle savedStateHandle;
    private final FeatureRepository featureRepository;
    private final long categoryId;
    private Observable<PagingData<Feature>> features;

    @Inject
    public FeatureListViewModel(@NonNull SavedStateHandle savedStateHandle,
                                @NonNull FeatureRepository featureRepository) {
        this.savedStateHandle = savedStateHandle;
        this.featureRepository = featureRepository;
        this.categoryId = (long) Optional.ofNullable(savedStateHandle.get("categoryId")).orElse(-1L);
    }

    @NonNull
    public Observable<PagingData<Feature>> getFeatures() {
        if (features == null) {
            features = featureRepository.getFeaturesByCategoryId(categoryId);
        }

        return features;
    }
}
