package com.xmatrix.melange.data;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.provider.BaseColumns;
import android.support.annotation.NonNull;
import android.util.Log;

import java.util.Date;

/**
 * Created by xmatrix on 6/26/2016.
 *
 * Sync Table Utilities for Melange Database
 */
public class DbSyncUtil {

    static String LOG_TAG = DbSyncUtil.class.getSimpleName();

    // A "projection" defines the columns that will be returned for each row
    static final String[] SYNC_TABLE_PROJECTION = new String[]{
            DbContract.SyncEntry._ID,
            DbContract.SyncEntry.COLUMN_SYNC_FROM_SERVER_DATE,
            DbContract.SyncEntry.COLUMN_SYNC_TO_SERVER_DATE
    };

    /**
     * Returns the Cursor for Sync Table
     * @param context
     * @return
     */
    static Cursor getCursor(
            Context context) {
        return context.getContentResolver().query(
                DbContract.SyncEntry.CONTENT_URI,               //Uri
                SYNC_TABLE_PROJECTION,                          //projection
                null,                                           //selection
                null,                                           //selectionArgs
                null);                                          //sortOrder
    }

    /**
     * Gets the last Sync date/time between client and server.
     * null will be returned if no record has been found
     * @return
     */
    public static Date getSyncDateFromServer(
            @NonNull  Context context) {
        Cursor cursor = null;
        Date syncDate = null;

        try {
            cursor = getCursor(context);
            if (cursor.moveToFirst()) {
                long syncTime = cursor.getLong(cursor.getColumnIndex(
                                    DbContract.SyncEntry.COLUMN_SYNC_FROM_SERVER_DATE));
                syncDate  = new Date(syncTime);
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return syncDate;
    }

    /**
     * Set Sync date to server in to table
     */
    public static void setSyncDateFromServer(
            @NonNull Context context,
            @NonNull Date syncDate) {

        Cursor cursor = null;

        try {
            cursor = getCursor(context);
            ContentValues contentValues = new ContentValues();
            contentValues.put(
                    DbContract.SyncEntry.COLUMN_SYNC_FROM_SERVER_DATE,
                    syncDate.getTime());

            if (cursor.moveToFirst()) {
                // entry exists, update the row with the new data
                String selection = String.format("%s = ?", DbContract.SyncEntry._ID);
                long rowId = cursor.getLong(cursor.getColumnIndex(BaseColumns._ID));

                Log.d(LOG_TAG, "setSyncDateFromServer: UPDATE BEGIN");
                context.getContentResolver().update(
                        DbContract.SyncEntry.CONTENT_URI,
                        contentValues,
                        selection,
                        new String[]{Long.toString(rowId)});

                Log.d(LOG_TAG, "setSyncDateFromServer: UPDATE END");

            } else {
                Log.d(LOG_TAG, "setSyncDateFromServer: INSERT BEGIN");
                context.getContentResolver().insert(
                        DbContract.SyncEntry.CONTENT_URI,
                        contentValues);
                Log.d(LOG_TAG, "setSyncDateFromServer: INSERT END");

            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }
}
