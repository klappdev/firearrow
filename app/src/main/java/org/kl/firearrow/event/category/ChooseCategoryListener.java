package org.kl.firearrow.event.category;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;

import org.kl.firearrow.ui.category.CategoryAdapter;
import org.kl.firearrow.ui.category.CategoryViewHolder;

public final class ChooseCategoryListener implements View.OnClickListener {
    private final CategoryViewHolder viewHolder;

    public ChooseCategoryListener(@NonNull CategoryViewHolder viewHolder) {
        this.viewHolder = viewHolder;
    }

    @Override
    public void onClick(@Nullable View view) {
        final var categoryAdapter = (CategoryAdapter) viewHolder.getBindingAdapter();

        if (categoryAdapter != null) {
            categoryAdapter.setPosition(viewHolder.getAbsoluteAdapterPosition());
            categoryAdapter.getNavigateAction().run();
        }
    }
}
