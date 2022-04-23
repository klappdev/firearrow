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

import io.reactivex.rxjava3.core.Observable;
import io.reactivex.rxjava3.core.Single;
import io.reactivex.rxjava3.schedulers.Schedulers;

import java.lang.reflect.Type;
import java.util.List;

import org.kl.firearrow.db.CommonDatabase;
import org.kl.firearrow.db.dao.CategoryDao;
import org.kl.firearrow.db.entity.Category;
import org.kl.firearrow.util.AssetManagerUtils;

public final class LoadCategoryWorker extends RxWorker {
    private static final String FILE_RESOURCE_NAME = "categories.json";
    private CategoryDao categoryDao;

    public LoadCategoryWorker(@NonNull Context appContext, @NonNull WorkerParameters workerParams) {
        super(appContext, workerParams);
    }

    @NonNull
    @Override
    public Single<Result> createWork() {
        final var database = CommonDatabase.getInstance(getApplicationContext());
        categoryDao = database.categoryDao();

        return checkIfTableEmpty()
                .flatMapObservable(this::loadCategories)
                .flatMapCompletable(categoryDao::insertAll)
                .toSingleDefault(Result.success())
                .onErrorReturnItem(Result.failure())
                .observeOn(Schedulers.io());
    }

    private Single<Boolean> checkIfTableEmpty() {
        return categoryDao.getAllCount().map(v -> v == 0);
    }

    private Observable<List<Category>> loadCategories(boolean isTableEmpty) {
        if (isTableEmpty) {
            return Observable.fromCallable(() -> {
                final Type type = new TypeToken<List<Category>>(){}.getType();
                final AssetManager assetManager = getApplicationContext().getAssets();
                return AssetManagerUtils.readJsonFileLines(assetManager, FILE_RESOURCE_NAME, type);
            });
        } else {
            return Observable.empty();
        }
    }
}
