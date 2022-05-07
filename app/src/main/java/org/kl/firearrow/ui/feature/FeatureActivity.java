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

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Bundle;
import android.widget.EditText;

import javax.inject.Inject;
import dagger.hilt.android.AndroidEntryPoint;
import io.reactivex.rxjava3.disposables.CompositeDisposable;
import lombok.Getter;

import org.kl.firearrow.databinding.ActivityFeatureBinding;
import org.kl.firearrow.viewmodel.FeatureListViewModel;

@AndroidEntryPoint
public class FeatureActivity extends AppCompatActivity {

    @Inject
    @Getter
    public CompositeDisposable disposables;

    private FeatureAdapter featureAdapter;
    private FeatureListViewModel featuresViewModel;
    private ActivityFeatureBinding binding;

    private EditText consoleTextEdit;
    private RecyclerView featureRecyclerView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        featuresViewModel = new ViewModelProvider(this).get(FeatureListViewModel.class);
        binding = ActivityFeatureBinding.inflate(getLayoutInflater());

        initView();
        subscribeUi();

        setContentView(binding.getRoot());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        disposables.dispose();
    }

    public void showExecutionFeature(@NonNull String text) {
        consoleTextEdit.append(text);
    }

    private void initView() {
        this.consoleTextEdit = binding.consoleEditText;
        this.featureRecyclerView = binding.featureRecyclerView;

        this.featureAdapter = new FeatureAdapter(this);
        featureRecyclerView.setAdapter(featureAdapter);
    }

    private void subscribeUi() {
        disposables.add(featuresViewModel.getFeatures()
            .subscribe(data -> {
                featureAdapter.submitData(getLifecycle(), data);
            }));
    }
}