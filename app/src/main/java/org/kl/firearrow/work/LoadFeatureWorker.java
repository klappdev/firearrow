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
package org.kl.firearrow.work;

import android.content.Context;
import android.content.res.AssetManager;

import androidx.annotation.NonNull;
import androidx.work.WorkerParameters;
import androidx.work.rxjava3.RxWorker;

import com.google.gson.reflect.TypeToken;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;

import io.reactivex.rxjava3.core.Observable;
import io.reactivex.rxjava3.core.Single;
import io.reactivex.rxjava3.schedulers.Schedulers;

import org.kl.firearrow.db.CommonDatabase;
import org.kl.firearrow.db.dao.FeatureDao;
import org.kl.firearrow.util.AssetManagerUtils;
import org.kl.firearrow.db.dao.CategoryDao;
import org.kl.firearrow.model.Feature;
import org.kl.firearrow.util.DateUtils;

public final class LoadFeatureWorker extends RxWorker {
    private static final String FILE_RESOURCE_NAME = "features.json";
    private CategoryDao categoryDao;
    private FeatureDao featureDao;

    public LoadFeatureWorker(@NonNull Context appContext, @NonNull WorkerParameters workerParams) {
        super(appContext, workerParams);
    }

    @NonNull
    @Override
    public Single<Result> createWork() {
        final var database = CommonDatabase.getInstance(getApplicationContext());
        categoryDao = database.categoryDao();
        featureDao = database.featureDao();

        return checkIfTablesEmpty()
                .flatMapObservable(this::loadFeaturesWithCorrectIdCategory)
                .flatMapCompletable(featureDao::insertAll)
                .toSingleDefault(Result.success())
                .onErrorReturnItem(Result.failure())
                .observeOn(Schedulers.io());
    }

    private Single<Boolean> checkIfTablesEmpty() {
        return Single.zip(categoryDao.getAllCount(), featureDao.getAllCount(),
                (v1, v2) -> v1 == 0 || v2 == 0);
    }

    private Observable<List<Feature>> loadFeatures() {
        return Observable.fromCallable(() -> {
            final Type type = new TypeToken<List<Feature>>(){}.getType();
            final AssetManager assetManager = getApplicationContext().getAssets();
            return AssetManagerUtils.readJsonFileLines(assetManager, FILE_RESOURCE_NAME, type);
        });
    }

    private Observable<List<Feature>> loadFeaturesWithCorrectIdCategory(boolean isEmpty) {
        if (isEmpty) {
            return Observable.zip(loadFeatures(), categoryDao.getAllIds(), (features, categoryIds) -> {
                final List<Feature> listFeatures = new ArrayList<>();

                for (final var feature : features) {
                    final long newIdCategory = categoryIds.get((int) (feature.getIdCategory() - 1));
                    listFeatures.add(feature.withIdCategory(newIdCategory).withDate(DateUtils.formatted()));
                }

                return listFeatures;
            });
        } else {
            return Observable.empty();
        }
    }
}
