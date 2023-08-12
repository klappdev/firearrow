/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2022-2023 https://github.com/klappdev
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
package org.kl.firearrow.db.dao;

import androidx.paging.PagingSource;
import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.OnConflictStrategy;
import androidx.room.Query;
import androidx.room.Update;

import io.reactivex.rxjava3.core.Completable;
import io.reactivex.rxjava3.core.Single;

import java.util.List;
import org.kl.firearrow.db.entity.Feature;

@Dao
public interface FeatureDao {
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    Completable insert(Feature feature);

    @Insert(onConflict = OnConflictStrategy.IGNORE)
    Completable insertAll(List<Feature> features);

    @Update
    Completable update(Feature feature);

    @Delete
    Completable delete(Feature feature);

    @Query("SELECT COUNT(*) FROM feature")
    Single<Integer> getAllCount();

    @Query("SELECT * FROM feature WHERE id_feature = :id")
    Single<Feature> getById(long id);

    @Query("SELECT * FROM feature ORDER BY name ASC")
    PagingSource<Integer, Feature> getAll();

    @Query("""
              SELECT * FROM feature ORDER BY
              CASE WHEN :isAsc = 1 THEN date END ASC,
              CASE WHEN :isAsc = 0 THEN date END DESC
           """)
    PagingSource<Integer, Feature> sortByDate(boolean isAsc);

    @Query("SELECT * FROM feature WHERE name LIKE :name")
    PagingSource<Integer, Feature> searchByName(String name);
}
