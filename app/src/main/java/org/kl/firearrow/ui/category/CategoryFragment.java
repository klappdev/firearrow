package org.kl.firearrow.ui.category;

import android.os.Bundle;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.RecyclerView;

import dagger.hilt.android.AndroidEntryPoint;
import lombok.Getter;

import org.kl.firearrow.R;
import org.kl.firearrow.databinding.FragmentCategoryBinding;
import org.kl.firearrow.event.category.NavigateCategoryListener;
import org.kl.firearrow.ui.MainActivity;
import org.kl.firearrow.viewmodel.CategoryListViewModel;

@AndroidEntryPoint
public final class CategoryFragment extends Fragment {
    private RecyclerView categoryRecyclerView;
    @Getter
    private MainActivity parentActivity;
    @Getter
    private CategoryAdapter categoryAdapter;
    private CategoryListViewModel categoriesViewModel;

    private NavigateCategoryListener navigateCategoryListener;
    private FragmentCategoryBinding binding;

    public CategoryFragment() {
        super(R.layout.fragment_category);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        setHasOptionsMenu(true);

        this.categoriesViewModel = new ViewModelProvider(this).get(CategoryListViewModel.class);
        this.binding = FragmentCategoryBinding.bind(view);

        initView();
        subscribeUi();
    }

    @Override
    public void onDestroyView() {
        this.binding = null;
        super.onDestroyView();
    }

    private void initView() {
        this.parentActivity = (MainActivity) getActivity();
        this.categoryAdapter = new CategoryAdapter();
        this.navigateCategoryListener = new NavigateCategoryListener(this);

        categoryAdapter.setNavigateAction(navigateCategoryListener::navigateShowFeature);

        this.categoryRecyclerView = binding.categoryRecycleView;
        categoryRecyclerView.setAdapter(categoryAdapter);
    }

    private void subscribeUi() {
        categoriesViewModel.getCategories().observe(getViewLifecycleOwner(), list -> {
            categoryAdapter.submitList(list);
        });
    }
}