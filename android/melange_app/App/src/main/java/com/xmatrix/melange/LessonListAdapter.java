package com.xmatrix.melange;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.xmatrix.melange.data.DbContract;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by xmatrix on 7/2/2016.
 *
 * Adapter for lesson list
 */
public class LessonListAdapter extends RecyclerView.Adapter<LessonListAdapter.ViewHolder> {

    //----------------------------------------------------------------------------------------------
    // Interface for ViewHolder to inform about Click from ViewHolder
    //----------------------------------------------------------------------------------------------
    public interface LessonViewHolderListener {
        void onLessonViewClick(int lessonId);
    }

    //----------------------------------------------------------------------------------------------
    // ViewHolder  Inner Class
    //----------------------------------------------------------------------------------------------
    public class ViewHolder
            extends RecyclerView.ViewHolder
            implements View.OnClickListener {

        private final String LOG_TAG =  ViewHolder.class.getSimpleName();

        private final View mLayout;
        private final ImageView mLessonImage;
        private final TextView mLessonName;
        private final TextView mLessonDate;

        /**
         * ViewHolder constructor
         */
        public ViewHolder(View itemView) {
            super(itemView);

            Log.d(LOG_TAG, "ViewHolder constructor called");

            mLayout         = itemView.findViewById(R.id.lesson_recycler_item_layout);
            mLessonImage    = (ImageView) itemView.findViewById(R.id.lesson_image);
            mLessonName     = (TextView) itemView.findViewById(R.id.lesson_name);
            mLessonDate     = (TextView) itemView.findViewById(R.id.lesson_date);

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
                int lessonId = mCursor.getInt(mCursor.getColumnIndex(DbContract.LessonEntry._ID));
                if (mLessonClickListener != null) {
                    mLessonClickListener.onLessonViewClick(lessonId);
                }
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // LessonListAdapter implementation
    //----------------------------------------------------------------------------------------------
    // Class variables
    private static final String LOG_TAG = LessonListAdapter.class.getSimpleName();
    private static final String DATE_FORMAT = "MMM dd yyyy";
    private Cursor mCursor;
    private LessonViewHolderListener mLessonClickListener;

    /**
     * LessonListAdapter constructor
     */
    public LessonListAdapter(LessonViewHolderListener listener) {
        mLessonClickListener = listener;
    }

    /**
     * Creates ViewHolder
     */
    @Override
    public ViewHolder onCreateViewHolder(
            ViewGroup parent,
            int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.lesson_recycler_item, parent, false);
        return new ViewHolder(view);
    }

    /**
     * Adapters provide a binding from an app-specific data set to views that are
     * displayed within a RecyclerView.
     */
    @Override
    public void onBindViewHolder(
            LessonListAdapter.ViewHolder holder,
            int position) {
        Log.d(LOG_TAG, "onBindViewHolder: Cursor = " + mCursor + " position= " + position);

        if (mCursor != null) {
            mCursor.moveToPosition(position);

            String name = mCursor.getString(mCursor.getColumnIndex(DbContract.LessonEntry.COLUMN_NAME));
            String imageUrl = mCursor.getString(mCursor.getColumnIndex(DbContract.LessonEntry.COLUMN_PIC_URL));
            long lStartDate = mCursor.getLong(mCursor.getColumnIndex(DbContract.LessonEntry.COLUMN_START_DATE));
            long lEndDate = mCursor.getLong(mCursor.getColumnIndex(DbContract.LessonEntry.COLUMN_END_DATE));
            String strDates = getFormattedDate(lStartDate, lEndDate);

            Context context = holder.mLayout.getContext();

            Glide.with(context)
                    .load(Uri.parse(imageUrl))
                    .error(R.drawable.default_lesson)
                    .fitCenter()
                    .into(holder.mLessonImage);

            holder.mLessonName.setText(name);
            holder.mLessonDate.setText(strDates);
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

    /**
     * Returns the formatted date string
     */
    private String getFormattedDate(
            long lStartDate,
            long lEndDate) {

        String strStartDate = new SimpleDateFormat(DATE_FORMAT).format(new Date(lStartDate));
        String strEndDate = new SimpleDateFormat(DATE_FORMAT).format(new Date(lEndDate));
        String strDates = strStartDate + " - " + strEndDate;

        return strDates;
    }
}
