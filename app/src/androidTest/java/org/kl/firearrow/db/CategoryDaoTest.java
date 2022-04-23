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
package org.kl.firearrow.db;

import android.content.Context;

import androidx.arch.core.executor.testing.InstantTaskExecutorRule;
import androidx.room.Room;
import androidx.test.platform.app.InstrumentationRegistry;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.List;

import org.kl.firearrow.db.dao.CategoryDao;
import org.kl.firearrow.db.entity.Category;

@RunWith(AndroidJUnit4.class)
public class CategoryDaoTest {

    @Rule
    public final InstantTaskExecutorRule taskExecutorRule = new InstantTaskExecutorRule();

    private CommonDatabase db;
    private CategoryDao dao;

    @Before
    public void setUp() {
        final Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();

        db = Room.inMemoryDatabaseBuilder(context, CommonDatabase.class)
                .allowMainThreadQueries()
                .build();
        dao = db.categoryDao();
    }

    @After
    public void tearDown() {
        db.close();
    }

    @Test
    public void insertCategoryTest() {
        final var name = "first category";
        final var category = new Category(1, name, "category description", "test.ico");

        dao.insert(category).blockingAwait();

        dao.searchByName(name)
            .test()
            .assertValue(data -> category.equals(data.get(0)));
    }

    @Test
    public void updateCategoryTest() {
        final var name = "first category";
        final var description = "category description";
        final var category = new Category(1, name, description, "test.ico");

        dao.insert(category).blockingAwait();

        dao.searchByName(name)
            .doOnError(Throwable::printStackTrace)
            .doOnNext((List<Category> insertData) -> {
                final long id = insertData.get(0).getId();
                final var updateCategory = new Category(id, name, description, "test_update.ico");

                dao.update(updateCategory).blockingAwait();

                dao.searchByName(name)
                    .test()
                    .assertValue(updateData -> updateCategory.equals(updateData.get(0)));
            });
    }

    @Test
    public void deleteCategoryTest() {
        final var name = "first category";
        final var description = "category description";
        final var category = new Category(1, name, description, "test.ico");

        dao.insert(category).blockingAwait();

        dao.searchByName(name)
            .subscribe(data -> dao.delete(data.get(0)).blockingAwait(),
                       Throwable::printStackTrace);

        dao.searchByName(name)
            .test()
            .assertValue(List::isEmpty);
    }
}