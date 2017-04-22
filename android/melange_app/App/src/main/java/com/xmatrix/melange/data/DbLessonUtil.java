package com.xmatrix.melange.data;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.support.annotation.NonNull;
import android.util.Log;
import static org.apache.commons.lang3.StringEscapeUtils.escapeHtml4;

import com.xmatrix.melange.backend.melanageApi.model.UserLessonBean;
import com.xmatrix.melange.backend.melanageApi.model.LessonBean;

/**
 * Created by xmatrix on 6/26/2016.
 *
 * Raga Table Utilities for Melange Database
 */
public class DbLessonUtil {

    static String LOG_TAG = DbLessonUtil.class.getSimpleName();
    static int STR_BUILDER_CAPACITY = 1024;

    // A "projection" defines the columns that will be returned for each row
    static final String[] LESSON_TABLE_PROJECTION = new String[]{
            DbContract.LessonEntry._ID,
            DbContract.LessonEntry.COLUMN_UID,
            DbContract.LessonEntry.COLUMN_NUM,
            DbContract.LessonEntry.COLUMN_START_DATE,
            DbContract.LessonEntry.COLUMN_END_DATE,
            DbContract.LessonEntry.COLUMN_AUTHOR,
            DbContract.LessonEntry.COLUMN_RAGA_ID,
            DbContract.LessonEntry.COLUMN_DETAIL,
            DbContract.LessonEntry.COLUMN_NAME,
            DbContract.LessonEntry.COLUMN_PIC_URL,
            DbContract.LessonEntry.COLUMN_THALA,
            DbContract.LessonEntry.COLUMN_WATCHED,
            DbContract.LessonEntry.COLUMN_UPDATE_DATE
    };

    /**
     * Sync Raga information from Lesson Bean
     */
    public static void syncLessonFromBackend(
            @NonNull Context context,
            @NonNull UserLessonBean userLessonBean,
            @NonNull LessonBean lessonBean,
            int raga_primary_key) {

        Cursor cursor = null;

        try {
            // Check if the Raga ID exists
            String selection = String.format("%s = ?", DbContract.LessonEntry.COLUMN_UID);
            cursor = context.getContentResolver().query(
                    DbContract.LessonEntry.CONTENT_URI,                                             // uri
                    new String[] {                                                                  // projection
                            DbContract.LessonEntry._ID,
                            DbContract.LessonEntry.COLUMN_UPDATE_DATE},
                    selection,                                                                      // selection
                    new String[]{String.valueOf(lessonBean.getId())},                               // selection args
                    null);                                                                          // sort order

            if (cursor.moveToFirst()) {
                int id = cursor.getInt(cursor.getColumnIndex(DbContract.LessonEntry._ID));

                // ID present in DB. We need to update if the date changed
                ContentValues cv = getContentValues(
                                        userLessonBean,
                                        lessonBean,
                                        raga_primary_key);

                Log.d(LOG_TAG, "syncLessonFromBackend: UPDATE BEGIN");
                // update the DB
                context.getContentResolver().update(
                        DbContract.LessonEntry.CONTENT_URI,
                        cv,
                        selection,
                        new String[]{String.valueOf(id)});

                Log.d(LOG_TAG, "syncLessonFromBackend: UPDATE END");

            } else {
                // id not present in DB. We need to insert
                ContentValues cv = getContentValues(
                                        userLessonBean,
                                        lessonBean,
                                        raga_primary_key);

                Log.d(LOG_TAG, "syncLessonFromBackend: INSERT BEGIN");
                context.getContentResolver().insert(
                        DbContract.LessonEntry.CONTENT_URI,
                        cv);

                Log.d(LOG_TAG, "syncLessonFromBackend: INSERT END");
            }
        } catch(IOException expIO) {
            Log.e(LOG_TAG, expIO.toString());
        }
        finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }

    /**
     * Get the PrimaryKey value for Lesson given lesson UID.
     * Returns -1 if uid is not valid
     */
    public static int getPrimaryKey(
            @NonNull Context context,
            String uid) {

        Cursor cursor = null;
        int primaryKey = -1;

        try {
            // Check if the Lesson ID exists
            String selection = String.format("%s = ?", DbContract.LessonEntry.COLUMN_UID);
            cursor = context.getContentResolver().query(
                    DbContract.LessonEntry.CONTENT_URI,                                             // uri
                    new String[]{DbContract.LessonEntry._ID},                                       // projection
                    selection,                                                                      // selection
                    new String[]{String.valueOf(uid)},                                              // selection args
                    null);                                                                          // sort order

            if (cursor.moveToFirst()) {
                primaryKey = cursor.getInt(cursor.getColumnIndex(DbContract.LessonEntry._ID));
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        return primaryKey;
    }

    /**
     * Update Lesson Watched value for the given lesson ID
     */
    public static void setLessonWatched(
            @NonNull Context context,
            int lessonId) {

        String selection = String.format("%s = ?", DbContract.LessonEntry._ID);
        ContentValues cv = new ContentValues();
        cv.put(DbContract.LessonEntry.COLUMN_WATCHED, 1);

        try {
            context.getContentResolver().update(
                    DbContract.LessonEntry.CONTENT_URI,
                    cv,
                    selection,
                    new String[]{String.valueOf(lessonId)});
            Log.d(LOG_TAG, "setLessonWatched: UPDATED");
        } catch (Exception ex) {
            Log.e(LOG_TAG, "setLessonWatched: UPDATE FAILED");
        }
    }

    /**
     * Returns Content Values from the Lesson Bean
     */
    private static ContentValues getContentValues(
        @NonNull UserLessonBean userLessonBean,
        @NonNull LessonBean lessonBean,
        int raga_primary_key) throws IOException {

         String lessonText = getEscapedLessonText(lessonBean.getLessonUrl());
         ContentValues cv = new ContentValues();

        cv.put(DbContract.LessonEntry.COLUMN_UID , lessonBean.getId());
        cv.put(DbContract.LessonEntry.COLUMN_NUM ,  userLessonBean.getLessonNumber());
        cv.put(DbContract.LessonEntry.COLUMN_START_DATE , userLessonBean.getStartDate().getValue());
        cv.put(DbContract.LessonEntry.COLUMN_END_DATE , userLessonBean.getEndDate().getValue());
        cv.put(DbContract.LessonEntry.COLUMN_AUTHOR , lessonBean.getAuthor());
        cv.put(DbContract.LessonEntry.COLUMN_RAGA_ID , raga_primary_key);
        cv.put(DbContract.LessonEntry.COLUMN_DETAIL, lessonText);
        cv.put(DbContract.LessonEntry.COLUMN_NAME , lessonBean.getName());
        cv.put(DbContract.LessonEntry.COLUMN_PIC_URL , lessonBean.getLessonPicUrl());
        cv.put(DbContract.LessonEntry.COLUMN_THALA , lessonBean.getThala());
        cv.put(DbContract.LessonEntry.COLUMN_WATCHED , 0);
        cv.put(DbContract.LessonEntry.COLUMN_UPDATE_DATE , lessonBean.getUpdateDate().getValue());

         return cv;
    }

    /**
     * Download the Lesson detail from Lesson URL
     */
    private static String getEscapedLessonText(@NonNull String lessonUrl) throws IOException{
        StringBuilder builder   = new StringBuilder(STR_BUILDER_CAPACITY);
        String line             = null;
        BufferedReader in       = null;
        URL url                 = new URL(lessonUrl);

        Log.d(LOG_TAG, "Getting Lesson from " + lessonUrl);
        try{
            in = new BufferedReader(new InputStreamReader(url.openStream()));
            while ((line = in.readLine()) != null) {
                builder.append(line);
            }
        }finally {
             if (in != null) {
                 in.close();
             }
        }

        return escapeHtml4(builder.toString());
    }
}
