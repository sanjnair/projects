package com.xmatrix.melange;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.xmatrix.melange.cloud.MelangeBackend;
import com.xmatrix.melange.data.DbContract;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by xmatrix on 7/2/2016.
 *
 * Adapter for lesson list
 */
public class LessonResListAdapter extends RecyclerView.Adapter<LessonResListAdapter.ViewHolder> {

    //----------------------------------------------------------------------------------------------
    // Interface for ViewHolder to inform about Click from ViewHolder
    //----------------------------------------------------------------------------------------------
    public interface LessonResViewHolderListener {
        void onLessonResViewClick(int type, String resValue);
    }

    //----------------------------------------------------------------------------------------------
    // ViewHolder  Inner Class
    //----------------------------------------------------------------------------------------------
    public class ViewHolder
            extends RecyclerView.ViewHolder
            implements View.OnClickListener {

        private final String LOG_TAG =  ViewHolder.class.getSimpleName();

        private final View mLayout;
        private final ImageView mLessonResImage;
        private final TextView mLessonDesc;

        /**
         * ViewHolder constructor
         */
        public ViewHolder(View itemView) {
            super(itemView);

            Log.d(LOG_TAG, "ViewHolder constructor called");

            mLayout         = itemView.findViewById(R.id.lesson_res_recycler_item_layout);
            mLessonResImage = (ImageView) itemView.findViewById(R.id.lesson_res_image);
            mLessonDesc     = (TextView) itemView.findViewById(R.id.lesson_res_desc);

            itemView.setOnClickListener(this);
        }

        /**
         * Click listener
         */
        @Override
        public void onClick(View view) {
            int position = getAdapterPosition();
            if (position >= 0) {
                mCursor.moveToPosition(position);

                int type = mCursor.getInt(
                        mCursor.getColumnIndex(DbContract.LessonResEntry.COLUMN_TYPE));

                String resVal = mCursor.getString(
                                mCursor.getColumnIndex(DbContract.LessonResEntry.COLUMN_RES_VAL));

                if (mLessonResClickListener != null) {
                    mLessonResClickListener.onLessonResViewClick(type, resVal);
                }
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // LessonResListAdapter implementation
    //----------------------------------------------------------------------------------------------
    // Class variables
    private static final String LOG_TAG = LessonResListAdapter.class.getSimpleName();
    private Cursor mCursor;
    private LessonResViewHolderListener mLessonResClickListener;

    /**
     * LessonResListAdapter constructor
     */
    public LessonResListAdapter(LessonResViewHolderListener listener) {
        mLessonResClickListener = listener;
    }

    /**
     * Creates ViewHolder
     */
    @Override
    public ViewHolder onCreateViewHolder(
            ViewGroup parent,
            int viewType) {

        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.lesson_res_recycler_item, parent, false);
        return new ViewHolder(view);
    }

    /**
     * Adapters provide a binding from an app-specific data set to views that are
     * displayed within a RecyclerView.
     */
    @Override
    public void onBindViewHolder(
            LessonResListAdapter.ViewHolder holder,
            int position) {
        Log.d(LOG_TAG, "onBindViewHolder: Cursor = " + mCursor + " position= " + position);

        if (mCursor != null) {
            mCursor.moveToPosition(position);

            int type            = mCursor.getInt(mCursor.getColumnIndex(DbContract.LessonResEntry.COLUMN_TYPE));
            String description  = mCursor.getString(mCursor.getColumnIndex(DbContract.LessonResEntry.COLUMN_DESCRIPTION));

            BitmapDrawable bitmapDrawable = (BitmapDrawable)holder.mLessonResImage.getDrawable();
            if (bitmapDrawable != null) {
                bitmapDrawable.getBitmap().recycle();
            }

            Drawable drawable;
            if (type == MelangeBackend.LESSON_RES_TYPE_AUDIO_MP3)
            {
                drawable = holder.mLayout.getContext().getDrawable(R.drawable.music);
            } else if (type == MelangeBackend.LESSON_RES_TYPE_VIDEO_YOUTUBE) {
                drawable = holder.mLayout.getContext().getDrawable(R.drawable.youtube);
            } else {
                drawable = holder.mLayout.getContext().getDrawable(R.drawable.default_lesson);
            }

            holder.mLessonResImage.setImageDrawable(drawable);
            holder.mLessonDesc.setText(description);
        }
    }

    /**
     * Returns the item count
     */
    @Override
    public int getItemCount() {
        return (mCursor == null) ? 0 : mCursor.getCount();
    }

    /**
     * Sets the cursor
     */
    public void setCursor(Cursor cursor) {
        mCursor = cursor;
        notifyDataSetChanged();
    }
}
