package com.xmatrix.melange.data;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.support.annotation.NonNull;
import android.util.Log;

import com.xmatrix.melange.backend.melanageApi.model.LessonResourceBean;

/**
 * Created by xmatrix on 6/26/2016.
 *
 * Raga Table Utilities for Melange Database
 */
public class DbLessonResUtil {

    static String LOG_TAG = DbLessonResUtil.class.getSimpleName();
    static int STR_BUILDER_CAPACITY = 1024;

    // A "projection" defines the columns that will be returned for each row
    static final String[] SYNC_TABLE_PROJECTION = new String[]{
            DbContract.LessonResEntry._ID,
            DbContract.LessonResEntry.COLUMN_LESSON_ID,
            DbContract.LessonResEntry.COLUMN_UID,
            DbContract.LessonResEntry.COLUMN_TYPE,
            DbContract.LessonResEntry.COLUMN_RES_VAL,
            DbContract.LessonResEntry.COLUMN_DESCRIPTION,
            DbContract.LessonResEntry.COLUMN_NUM,
            DbContract.LessonResEntry.COLUMN_UPDATE_DATE
    };

    //-- DATABASE Queries---------------------------------------------------------------------------
    private static final String COL_COUNT = "COUNT";
    private static final String[] LESSON_RES_COUNT_PROJECTION = { "count(*) AS " + COL_COUNT };

    private static final String LESSON_NUM_RES_SELECTION =
            String.format("%s = ?", DbContract.LessonResEntry.COLUMN_LESSON_ID);

    //----------------------------------------------------------------------------------------------

    /**
     * Sync Raga information from LessonRes Bean
     */
    public static void syncLessonResourceFromBackend(
            @NonNull Context context,
            @NonNull LessonResourceBean lessonResBean,
            int lesson_primary_key) {

        Cursor cursor = null;

        try {
            // Check if the Raga ID exists
            String selection = String.format("%s = ?", DbContract.LessonResEntry.COLUMN_UID);
            cursor = context.getContentResolver().query(
                    DbContract.LessonResEntry.CONTENT_URI,                                          // uri
                    new String[]{DbContract.LessonResEntry.COLUMN_UPDATE_DATE},                     // projection
                    selection,                                                                      // selection
                    new String[]{String.valueOf(lessonResBean.getId())},                            // selection args
                    null);                                                                          // sort order

            if (cursor.moveToFirst()) {
                // ID present in DB. We need to update if the date changed

                ContentValues cv = getContentValues(
                        lessonResBean,
                        lesson_primary_key);

                Log.d(LOG_TAG, "syncLessonResourceFromBackend: UPDATE BEGIN");
                // update the DB
                context.getContentResolver().update(
                        DbContract.LessonResEntry.CONTENT_URI,
                        cv,
                        selection,
                        new String[]{String.valueOf(lessonResBean.getId())});

                Log.d(LOG_TAG, "syncLessonResourceFromBackend: UPDATE END");
            } else {
                // id not present in DB. We need to insert
                ContentValues cv = getContentValues(
                        lessonResBean,
                        lesson_primary_key);

                Log.d(LOG_TAG, "syncLessonResourceFromBackend: INSERT BEGIN");
                Log.d(LOG_TAG, "lessonResBean UID = " + lessonResBean.getId());

                context.getContentResolver().insert(
                        DbContract.LessonResEntry.CONTENT_URI,
                        cv);

                Log.d(LOG_TAG, "syncLessonResourceFromBackend: INSERT END");
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }

    /**
     * Get the PrimaryKey value for LessonResource given lessonRes UID.
     * Returns -1 if uid is not valid
     */
    public static int getPrimaryKey(
            @NonNull Context context,
            String uid) {

        Cursor cursor = null;
        int primaryKey = -1;

        try {
            Log.d(LOG_TAG, "getPrimaryKey: lessonResBean UID = " + uid);
            // Check if the Lesson ID exists
            String selection = String.format("%s = ?", DbContract.LessonResEntry.COLUMN_UID);
            cursor = context.getContentResolver().query(
                    DbContract.LessonResEntry.CONTENT_URI,                                          // uri
                    new String[]{DbContract.LessonResEntry._ID},                                    // projection
                    selection,                                                                      // selection
                    new String[]{String.valueOf(uid)},                                              // selection args
                    null);                                                                          // sort order

            if (cursor.moveToFirst()) {
                primaryKey = cursor.getInt(cursor.getColumnIndex(DbContract.LessonResEntry._ID));
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        return primaryKey;
    }


    /**
     * Returns the number of resources for the given lesson
     */
    public static int getNumLessonResources(
            @NonNull Context context,
            int lesson_id) {
        Cursor cursor = null;
        int numRes = 0;

        try {
            cursor = context.getContentResolver().query(
                        DbContract.LessonResEntry.CONTENT_URI,                                      // uri
                        LESSON_RES_COUNT_PROJECTION,                                                // projection
                        LESSON_NUM_RES_SELECTION,                                                   // selection
                        new String[] {String.valueOf(lesson_id)},                                   // values
                        null);                                                                      // sort

            if (cursor.moveToFirst()) {
                numRes = cursor.getInt(cursor.getColumnIndex(COL_COUNT));
            }

        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        return numRes;
    }

    /**
     * Returns Content Values from the LessonRes Bean
     */
    private static ContentValues getContentValues(
        @NonNull LessonResourceBean lessonResBean,
        int lesson_primary_key) {

        ContentValues cv = new ContentValues();

        cv.put(DbContract.LessonResEntry.COLUMN_LESSON_ID, lesson_primary_key);
        cv.put(DbContract.LessonResEntry.COLUMN_UID, lessonResBean.getId());
        cv.put(DbContract.LessonResEntry.COLUMN_TYPE, lessonResBean.getType());
        cv.put(DbContract.LessonResEntry.COLUMN_RES_VAL, lessonResBean.getResval());
        cv.put(DbContract.LessonResEntry.COLUMN_DESCRIPTION, lessonResBean.getDescription());
        cv.put(DbContract.LessonResEntry.COLUMN_NUM, lessonResBean.getNum());
        cv.put(DbContract.LessonResEntry.COLUMN_UPDATE_DATE, lessonResBean.getUpdateDate().getValue());

        return cv;
    }
}
