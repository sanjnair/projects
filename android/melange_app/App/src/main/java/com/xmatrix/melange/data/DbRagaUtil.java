package com.xmatrix.melange.data;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.support.annotation.NonNull;
import android.util.Log;

import com.xmatrix.melange.backend.melanageApi.model.RagaBean;

/**
 * Created by xmatrix on 6/26/2016.
 *
 * Raga Table Utilities for Melange Database
 */
public class DbRagaUtil {

    static String LOG_TAG = DbRagaUtil.class.getSimpleName();

    // A "projection" defines the columns that will be returned for each row
    static final String[] SYNC_TABLE_PROJECTION = new String[]{
            DbContract.RagaEntry._ID,
            DbContract.RagaEntry.COLUMN_UID,
            DbContract.RagaEntry.COLUMN_NAME,
            DbContract.RagaEntry.COLUMN_AROHANA,
            DbContract.RagaEntry.COLUMN_AVAROHANA,
            DbContract.RagaEntry.COLUMN_MELAKARTHA,
            DbContract.RagaEntry.COLUMN_PIC_URL,
            DbContract.RagaEntry.COLUMN_UPDATE_DATE
    };

    /**
     * Returns the Cursor for Sync Table
     * @param context
     * @return
     */
    static Cursor getCursor(
            @NonNull Context context) {
        return context.getContentResolver().query(
                DbContract.SyncEntry.CONTENT_URI,                                                   //Uri
                SYNC_TABLE_PROJECTION,                                                              //projection
                null,                                                                               //selection
                null,                                                                               //selectionArgs
                null);                                                                              //sortOrder
    }

    /**
     * Sync Raga information from Raga Bean
     */
    public static void syncRagaFromBackend(
            @NonNull Context context,
            @NonNull RagaBean ragaBean) {

        Cursor cursor = null;

        try {
            // Check if the Raga ID exists
            String selection = String.format("%s = ?", DbContract.RagaEntry.COLUMN_UID);
            cursor = context.getContentResolver().query(
                        DbContract.RagaEntry.CONTENT_URI,                                           // uri
                        new String[]{DbContract.RagaEntry.COLUMN_UPDATE_DATE},                      // projection
                        selection,                                                                  // selection
                        new String[]{String.valueOf(ragaBean.getId())},                             // selection args
                        null);                                                                      // sort order

            if (cursor.moveToFirst()) {
                // ID present in DB. We need to update if the date changed
                ContentValues cv = getContentValues(ragaBean);

                Log.d(LOG_TAG, "syncRagaFromBackend: BEGIN");
                // update the DB
                context.getContentResolver().update(
                        DbContract.RagaEntry.CONTENT_URI,
                        cv,
                        selection,
                        new String[]{String.valueOf(ragaBean.getId())});

                Log.d(LOG_TAG, "syncRagaFromBackend: UPDATE END");
            } else {
                // id not present in DB. We need to insert
                Log.d(LOG_TAG, "syncRagaFromBackend: INSERT UPDATE");
                ContentValues cv = getContentValues(ragaBean);
                context.getContentResolver().insert(
                        DbContract.RagaEntry.CONTENT_URI,
                        cv);

                Log.d(LOG_TAG, "syncRagaFromBackend: INSERT END");
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }

    /**
     * Get the PrimaryKey value for Raga given raga UID.
     * Returns -1 if uid is not valid
     */
    public static int getPrimaryKey(
            @NonNull Context context,
            String uid) {

        Cursor cursor = null;
        int primaryKey = -1;

        try {
            // Check if the Raga ID exists
            String selection = String.format("%s = ?", DbContract.RagaEntry.COLUMN_UID);
            cursor = context.getContentResolver().query(
                    DbContract.RagaEntry.CONTENT_URI,                                               // uri
                    new String[]{DbContract.RagaEntry._ID},                                         // projection
                    selection,                                                                          // selection
                    new String[]{String.valueOf(uid)},                                                  // selection args
                    null);                                                                              // sort order

            if (cursor.moveToFirst()) {
                primaryKey = cursor.getInt(cursor.getColumnIndex(DbContract.RagaEntry._ID));
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        return primaryKey;
    }


    /**
     * Returns Content Values from the Raga Bean
     */
    private static ContentValues getContentValues(@NonNull RagaBean ragaBean) {
        ContentValues cv = new ContentValues();

        cv.put(DbContract.RagaEntry.COLUMN_UID, ragaBean.getId());
        cv.put(DbContract.RagaEntry.COLUMN_NAME, ragaBean.getName());
        cv.put(DbContract.RagaEntry.COLUMN_AROHANA, ragaBean.getArohana());
        cv.put(DbContract.RagaEntry.COLUMN_AVAROHANA, ragaBean.getAvarohana());
        cv.put(DbContract.RagaEntry.COLUMN_MELAKARTHA, ragaBean.getMelakartha());
        cv.put(DbContract.RagaEntry.COLUMN_PIC_URL, ragaBean.getPicture());
        cv.put(DbContract.RagaEntry.COLUMN_UPDATE_DATE, ragaBean.getUpdateDate().getValue());

        return cv;
    }
}
