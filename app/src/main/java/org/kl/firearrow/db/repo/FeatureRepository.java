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
package org.kl.firearrow.db.repo;

import androidx.annotation.NonNull;
import androidx.paging.Pager;
import androidx.paging.PagingConfig;
import androidx.paging.PagingData;
import androidx.paging.rxjava3.PagingRx;

import javax.inject.Inject;
import javax.inject.Singleton;
import java.util.List;

import io.reactivex.rxjava3.core.Completable;
import io.reactivex.rxjava3.core.Observable;
import io.reactivex.rxjava3.core.Single;

import org.kl.firearrow.db.dao.CategoryFeaturesDao;
import org.kl.firearrow.db.dao.FeatureDao;
import org.kl.firearrow.model.Feature;

@Singleton
public final class FeatureRepository {
    private final CategoryFeaturesDao categoryFeaturesDao;
    private final FeatureDao featureDao;

    private static final PagingConfig CONFIG = new PagingConfig(
        /*pageSize=*/ 5,
        /*prefetchDistance=*/ 5,
        /*enablePlaceholders=*/ true
    );

    @Inject
    public FeatureRepository(@NonNull CategoryFeaturesDao categoryFeaturesDao,
                             @NonNull FeatureDao featureDao) {
        this.categoryFeaturesDao = categoryFeaturesDao;
        this.featureDao = featureDao;
    }

    @NonNull
    public Single<Feature> getFeature(long id) {
        return featureDao.getById(id);
    }

    @NonNull
    public Observable<PagingData<Feature>> getFeaturesByCategoryId(long categoryId) {
        return PagingRx.getObservable(
            new Pager<>(CONFIG, () -> categoryFeaturesDao.getAllById(categoryId))
        ).map(input ->
            PagingRx.flatMap(input, categoryFeatures ->
                Single.just(categoryFeatures.getFeatures())
            )
        );
    }

    @NonNull
    public Completable createFeatures(@NonNull List<Feature> features) {
        return featureDao.insertAll(features);
    }
}
