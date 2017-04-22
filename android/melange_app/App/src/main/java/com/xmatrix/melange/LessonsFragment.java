package com.xmatrix.melange;


import android.content.Context;
import android.database.Cursor;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.support.v4.app.LoaderManager;
import android.widget.TextView;
import android.support.v4.content.Loader;
import android.support.v4.content.CursorLoader;
import com.xmatrix.melange.data.DbContract;

/**
 * A simple {@link Fragment} subclass.
 */
public class LessonsFragment extends Fragment
        implements LoaderManager.LoaderCallbacks<Cursor>,
        LessonListAdapter.LessonViewHolderListener
{

    /**
     * Interface for Notifying about Lesson Click
     */
    public interface LessonClickListener {
        void onLessonClick(int lessonId);
    }

    private static final String LOG_TAG = LessonsFragment.class.getSimpleName();
    private static final int LESSON_LOADER_ID = 0;

    private static final String[] LESSONS_PROJECTION = {
            DbContract.LessonEntry._ID,
            DbContract.LessonEntry.COLUMN_RAGA_ID,
            DbContract.LessonEntry.COLUMN_PIC_URL,
            DbContract.LessonEntry.COLUMN_NAME,
            DbContract.LessonEntry.COLUMN_START_DATE,
            DbContract.LessonEntry.COLUMN_END_DATE
    };
    private static final String LESSONS_SORT_ORDER = DbContract.LessonEntry.COLUMN_NUM + " ASC";

    private LessonListAdapter mListAdapter;
    private TextView mTxtNotification;


    /**
     * Constructs Lesson Fragment
     */
    public LessonsFragment() {
        // required
    }

    @Override
    public View onCreateView(
            LayoutInflater inflater,
            ViewGroup container,
            Bundle savedInstanceState) {

        Log.d(LOG_TAG, "LessonsFragment: onCreateView");

        View fragmentView = inflater.inflate(R.layout.fragment_lessons, container, false);
        RecyclerView lessonView = (RecyclerView) fragmentView.findViewById(R.id.lesson_recycler_view);
        lessonView.setLayoutManager(new LinearLayoutManager(lessonView.getContext()));

        mTxtNotification = (TextView)fragmentView.findViewById(R.id.lesson_notificationText);

        mListAdapter = new LessonListAdapter(this);
        lessonView.setAdapter(mListAdapter);

        // Inflate the layout for this fragment
        return fragmentView;
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        getLoaderManager().initLoader(LESSON_LOADER_ID, null, this);
        super.onActivityCreated(savedInstanceState);
    }

    /**
     * Creates Cursor loader
     */
    @Override
    public Loader<Cursor> onCreateLoader(int id, Bundle bundle) {
        Log.d(LOG_TAG, "onCreateLoader: id= " + id);

        CursorLoader cursorLoader = null;

        if (LESSON_LOADER_ID == id) {
            cursorLoader = new CursorLoader(
                                getActivity(),                                                      // context
                                DbContract.LessonEntry.CONTENT_URI,                                 // uri
                                LESSONS_PROJECTION,                                                 // projection
                                null,                                                               // selection
                                null,                                                               // selectionArgs
                                LESSONS_SORT_ORDER);                                                // sortOrder
        } else {
            Log.e(LOG_TAG, "Invalid Loader ID " + id);
        }

        return cursorLoader;
    }

    /**
     * Load finished callback
     */
    @Override
    public void onLoadFinished(
            Loader<Cursor> loader,
            Cursor data) {

        Log.d(LOG_TAG, "onLoadFinished:" );
        int count = 0;
        mListAdapter.setCursor(data);

        if (data != null) {
            count = data.getCount();
        }
        Log.d(LOG_TAG, "count = " + count );
        setNotificationTextView(count);
    }

    /**
     * Loader reset
     */
    @Override
    public void onLoaderReset(Loader<Cursor> loader) {
        mListAdapter.setCursor(null);
    }

    @Override
    public void onLessonViewClick(int lessonId) {
        Log.d(LOG_TAG, "onLessonViewClick called");

        LessonClickListener listener = (LessonClickListener) getActivity();
        listener.onLessonClick(lessonId);
    }

    /**
     * Set the notification Text
     */
    private void setNotificationTextView(int count) {
        if (count > 0) {
            mTxtNotification.setVisibility(View.GONE);
        } else {
            mTxtNotification.setVisibility(View.VISIBLE);

            // Set the message if network is not available
            Context context = getContext();

            String msg;

            if (!Utility.isNetworkAvailable(context)) {
                msg = context.getString(R.string.no_lessons_no_network);
            } else {
                msg = context.getString(R.string.no_lessons_syncing);
            }
            mTxtNotification.setText(msg);
        }
    }
}
