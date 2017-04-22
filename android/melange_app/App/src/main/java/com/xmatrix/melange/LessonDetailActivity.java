package com.xmatrix.melange;

import android.content.Intent;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

public class LessonDetailActivity
        extends AppCompatActivity
        implements LessonDetailFragment.OnFragmentInteractionListener{

    public static final String LESSON_ID        = "LessonId";
    private static final String LOG_TAG         = LessonDetailActivity.class.getSimpleName();
    private static final int INVALID_LESSON_ID  = -1;
    private static final String STATE_LESSON_ID = "state_lesson_id";

    private int mLessonId = INVALID_LESSON_ID;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_lesson_detail);

        int lessonId = mLessonId;

        if (lessonId == INVALID_LESSON_ID) {
            Intent intent = getIntent();
            if (intent != null) {
                lessonId = intent.getIntExtra(LESSON_ID, INVALID_LESSON_ID);
            }

            if (lessonId == INVALID_LESSON_ID) {
                if (savedInstanceState != null) {
                    lessonId = savedInstanceState.getInt(STATE_LESSON_ID, INVALID_LESSON_ID);
                } else {
                    Log.d(LOG_TAG, "savedInstanceState is null");
                }
            }
        }

        if (lessonId != INVALID_LESSON_ID) {
            mLessonId = lessonId;
            LessonDetailFragment fragment = LessonDetailFragment.newInstance(lessonId);
            getSupportFragmentManager()
                .beginTransaction().replace(R.id.lesson_detail_fragment_container, fragment)
                .commit();

        } else {
            Log.e(LOG_TAG, "Invalid lessonID " + lessonId);
        }
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
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        Log.v(LOG_TAG, "onRestoreInstanceState called");
        mLessonId = savedInstanceState.getInt(STATE_LESSON_ID, INVALID_LESSON_ID);
    }

    /**
     * LessonDetailFragment.OnFragmentInteractionListener{
     * Empty for now
     */
    public void onFragmentInteraction(Uri uri) {
        Log.d(LOG_TAG, "onFragmentInteraction called");
    }
}
