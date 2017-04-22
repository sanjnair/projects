package com.xmatrix.melange;

import android.content.Intent;
import android.database.Cursor;
import android.media.MediaPlayer;
import android.net.Uri;
import android.support.v4.app.LoaderManager;
import android.support.v4.content.CursorLoader;
import android.support.v4.content.Loader;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.xmatrix.melange.cloud.MelangeBackend;
import com.xmatrix.melange.data.DbContract;

public class LessonResActivity
        extends AppCompatActivity
        implements LoaderManager.LoaderCallbacks<Cursor>,
        LessonResListAdapter.LessonResViewHolderListener {

    public static final String LESSON_ID            = "LessonId";
    private static final String LOG_TAG             = LessonsFragment.class.getSimpleName();
    private static final int LOADER_ID              = 10;
    private static final int INVALID_LESSON_ID      = -1;
    private static final String STATE_LESSON_ID     = "state_lesson_id";

    //-- DATABASE Related Constants ----------------------------------------------------------------
    private static final String[] LESSONS_PROJECTION = {
            DbContract.LessonResEntry._ID,
            DbContract.LessonResEntry.COLUMN_NUM,
            DbContract.LessonResEntry.COLUMN_TYPE,
            DbContract.LessonResEntry.COLUMN_RES_VAL,
            DbContract.LessonResEntry.COLUMN_DESCRIPTION
    };
    private String LESSON_RES_SELECTION =
            String.format("%s = ?", DbContract.LessonResEntry.COLUMN_LESSON_ID);

    private static final String LESSON_RES__SORT_ORDER = DbContract.LessonResEntry.COLUMN_NUM + " ASC";
    //----------------------------------------------------------------------------------------------

    private LessonResListAdapter mListAdapter;
    private TextView mTxtNotification;
    private MediaPlayer mMediaPlayer;
    private String mLastAudioPath;
    private int mLessonId = INVALID_LESSON_ID;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_lesson_res);

        RecyclerView lessonResView = (RecyclerView)findViewById(R.id.lesson_res_recycler_view);
        lessonResView.setLayoutManager(new LinearLayoutManager(lessonResView.getContext()));

        Log.d(LOG_TAG, "OnCreate called");

        int lessonId = INVALID_LESSON_ID;
        Intent intent = getIntent();
        if (intent != null) {
            lessonId = intent.getIntExtra(LESSON_ID, -1);
        }

        if (INVALID_LESSON_ID != lessonId) {
            if (savedInstanceState != null)
            {
                lessonId = savedInstanceState.getInt(STATE_LESSON_ID, INVALID_LESSON_ID);
            } else {
                Log.d(LOG_TAG, "savedInstanceState is null");
            }
        }

        if (INVALID_LESSON_ID != lessonId) {
            mLessonId = lessonId;
            mListAdapter = new LessonResListAdapter(this);
            lessonResView.setAdapter(mListAdapter);
            getSupportLoaderManager().initLoader(LOADER_ID, null, this);
        } else {
            Log.d(LOG_TAG, "Lesson ID is invalid");
        }

        mMediaPlayer = new MediaPlayer();
        mTxtNotification = (TextView)findViewById(R.id.lesson_res_notificationText);
    }

    @Override
    public void onSaveInstanceState(Bundle savedInstanceState) {
        Log.d(LOG_TAG, "onSaveInstanceState called. LessonID = " + mLessonId);

        // Save the user's current game state
        savedInstanceState.putInt(STATE_LESSON_ID, mLessonId);

        // Always call the superclass so it can save the view hierarchy state
        super.onSaveInstanceState(savedInstanceState);
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mMediaPlayer.isPlaying()) {
            mMediaPlayer.stop();
            mMediaPlayer.reset();
        }
    }

    @Override
    public Intent getParentActivityIntent () {
        Log.d(LOG_TAG, "getParentActivityIntent called");
        Intent intent = super.getParentActivityIntent();
        intent.putExtra(LessonDetailActivity.LESSON_ID, mLessonId);
        return intent;
    }

    /**
     * Creates Cursor loader
     */
    @Override
    public Loader<Cursor> onCreateLoader(int id, Bundle bundle) {
        Log.d(LOG_TAG, "onCreateLoader: id= " + id);

        CursorLoader cursorLoader = null;

        if (LOADER_ID == id) {
            cursorLoader = new CursorLoader(
                            this,
                            DbContract.LessonResEntry.CONTENT_URI,                                  // uri
                            LESSONS_PROJECTION,                                                     // projection
                            LESSON_RES_SELECTION,                                                   // selection
                            new String[]{String.valueOf(mLessonId)},                                // selectionArgs
                            LESSON_RES__SORT_ORDER);                                                // sortOrder
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

    /**
     * LessonResViewHolderListener implementation
     * @param type
     * @param resValue
     */
    public void onLessonResViewClick(
            int type,
            String resValue) {

        Log.d(LOG_TAG, "onLessonResViewClick: type: " + type + " resVal: " + resValue);

        if (mMediaPlayer.isPlaying()){
            mMediaPlayer.stop();
            mMediaPlayer.reset();
        }


        if (type == MelangeBackend.LESSON_RES_TYPE_VIDEO_YOUTUBE) {
            Utility.playYoutubeVideo(this, resValue);

        } else if (type == MelangeBackend.LESSON_RES_TYPE_AUDIO_MP3) {

            boolean playAudio = (mLastAudioPath == null) || (!mLastAudioPath.equals(resValue));

            if (playAudio) {
                try {
                    String msg = getString(R.string.audio_play_message);
                    Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();

                    Uri uri = Uri.parse(resValue);
                    mMediaPlayer.setDataSource(this, uri);
                    mMediaPlayer.prepare();
                    mMediaPlayer.start();
                    mLastAudioPath = resValue;



                } catch (Exception ex) {
                    Log.e(LOG_TAG, ex.toString());
                    String msg = getString(R.string.err_audio__play);
                    Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
                }
            } else {
                mLastAudioPath = null;
            }
        }

        if (type != MelangeBackend.LESSON_RES_TYPE_AUDIO_MP3) {
            mLastAudioPath = null;
        }
    }

    /**
     * Set the notification Text
     */
    private void setNotificationTextView(int count) {
        if (count > 0) {
            mTxtNotification.setVisibility(View.GONE);
        } else {
            mTxtNotification.setVisibility(View.VISIBLE);
        }
    }
}
