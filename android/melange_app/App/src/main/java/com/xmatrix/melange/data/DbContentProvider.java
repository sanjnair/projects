package com.xmatrix.melange.data;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.util.Log;

import com.xmatrix.melange.R;

/**
 * Created by xmatrix on 7/1/2016.
 *
 * The abstract class ContentProvider defines six abstract methods that you must implement
 * as part of your own concrete subclass. All of these methods except onCreate() are called by a
 * client application that is attempting to access your content provider:
 */
public class DbContentProvider extends ContentProvider {
    // Creates a UriMatcher object.
    // maps content URI "patterns" to integer values
    private static final String LOG_TAG         = DbContentProvider.class.getSimpleName();
    private static final UriMatcher mUriMatcher = buildUriMatcher();

    private static final int TYPE_RAGA                      = 100;
    private static final int TYPE_LESSON                    = 200;
    private static final int TYPE_LESSON_RES                = 300;
    private static final int TYPE_SYNC                      = 400;

    private static final String SELECT_LESSON_RES_FOR_LESSON =
            DbContract.LessonResEntry.TABLE_NAME + "." +
            DbContract.LessonResEntry.COLUMN_LESSON_ID + " = ?";

    // Database helper
    private DbHelper mDbHelper;

    /**
     * Constructs DB Content Provider
     */
    public DbContentProvider() {
    }

    /**
     * Implement this to initialize your content provider on startup.
     * This method is called for all registered content providers on the application main thread
     * at application launch time. It must not perform lengthy operations,
     * or application startup will be delayed.
     *
     * @return
     */
    @Override
    public boolean onCreate() {
        mDbHelper = new DbHelper(getContext());
        return true;
    }

    /**
     * Implement this to handle requests for the MIME type of the data at the given URI.
     * The returned MIME type should start with vnd.android.cursor.item for a single record,
     * or vnd.android.cursor.dir/ for multiple items.
     */
    @Override
    public String getType(Uri uri) {
        final int type = mUriMatcher.match(uri);

        switch (type) {
            case TYPE_RAGA:
                return DbContract.RagaEntry.CONTENT_TYPE;

            case TYPE_LESSON:
                return DbContract.LessonEntry.CONTENT_TYPE;

            case TYPE_LESSON_RES:
                return DbContract.LessonResEntry.CONTENT_TYPE;

            case TYPE_SYNC:
                return DbContract.SyncEntry.CONTENT_TYPE;

            default:
                String msg = getContext().getString(R.string.err_invalid_provider_uri) + uri;
                throw new IllegalArgumentException(msg);
        }
    }

    /**
     * Query method
     */
    @Override
    public Cursor query (
            Uri uri,
            String[] projection,
            String selection,
            String[] selectionArgs,
            String sortOrder) {

        String tableName    = getTableName(uri);
        Cursor cursor = mDbHelper.getReadableDatabase().query(
                            tableName,                          // table
                            projection,                         // columns
                            selection,                          // selection
                            selectionArgs,                      // selectionArgs
                            null,                               // groupBy
                            null,                               // having
                            sortOrder);                         // OrderBy

        cursor.setNotificationUri(getContext().getContentResolver(), uri);

        return cursor;
    }

    /**
     * Insert operation
     */
    public Uri insert (
            Uri uri,
            ContentValues values) {

        final SQLiteDatabase db = mDbHelper.getWritableDatabase();
        int type = mUriMatcher.match(uri);
        Uri insertUri   = null;
        long primaryKey;

        switch(type) {
            case TYPE_RAGA:
                primaryKey = db.insert(DbContract.RagaEntry.TABLE_NAME, null, values);
                if (primaryKey > 0) {
                    insertUri = DbContract.RagaEntry.buildRagaUri(primaryKey);
                }
                break;

            case TYPE_LESSON:
                primaryKey = db.insert(DbContract.LessonEntry.TABLE_NAME, null, values);
                if (primaryKey > 0) {
                    insertUri = DbContract.LessonEntry.buildLessonUri(primaryKey);
                }
                break;

            case TYPE_LESSON_RES:
                primaryKey = db.insert(DbContract.LessonResEntry.TABLE_NAME, null, values);
                if (primaryKey > 0) {
                    insertUri = DbContract.LessonResEntry.buildLessonResUri(primaryKey);
                }
                break;

            case TYPE_SYNC:
                primaryKey = db.insert(DbContract.SyncEntry.TABLE_NAME, null, values);
                if (primaryKey > 0) {
                    insertUri = DbContract.SyncEntry.buildSyncUri(primaryKey);
                }
                break;

            default:
                String msg = getContext().getString(R.string.err_invalid_provider_uri) + uri;
                throw new IllegalArgumentException(msg);
        }

        if (primaryKey <= 0) {
            String msg = getContext().getString(R.string.err_insertion_failed) + uri;
            throw new android.database.SQLException(msg);
        }

        getContext().getContentResolver().notifyChange(insertUri, null);

        return insertUri;
    }

    /**
     * Bulk Insert
     */
    public int bulkInsert (
            Uri uri,
            ContentValues[] values) {

        final SQLiteDatabase db = mDbHelper.getWritableDatabase();
        String tableName = getTableName(uri, false);

        if (tableName == null) {
            return super.bulkInsert(uri, values);
        }

        int count = 0;
        db.beginTransaction();

        try {
            for (ContentValues value : values) {
                long primaryKey = db.insert(tableName, null, value);
                if (primaryKey != -1) {
                    count++;
                }
            }
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }

        getContext().getContentResolver().notifyChange(uri, null);

        return count;
    }

    /**
     * Updates table
     */
    @Override
    public int update (
            Uri uri,
            ContentValues values,
            String selection,
            String[] selectionArgs) {

        final SQLiteDatabase db = mDbHelper.getWritableDatabase();
        String tableName = getTableName(uri);

        int count = db.update(tableName, values, selection, selectionArgs);
        Log.d(LOG_TAG, "Update touched " + count + " rows");

        if (count > 0) {
            getContext().getContentResolver().notifyChange(uri, null);
        }

        return count;
    }


    /**
     * Delete Api
     */
    @Override
    public int delete(
            Uri uri,
            String selection,
            String[] selectionArgs) {

        final SQLiteDatabase db = mDbHelper.getWritableDatabase();
        String tableName = getTableName(uri);

        int count = db.delete(tableName, selection, selectionArgs);

        if (count > 0) {
            getContext().getContentResolver().notifyChange(uri, null);
        }

        return count;
    }


    /**
     * Utility function returns table name given uri.
     * Throws IllegalArgumentException if type does not match
     */
    private String getTableName(
            Uri uri)
    {
        return getTableName(uri, true);
    }

    /**
     * Utility function returns table name given uri.
     * Throws IllegalArgumentException if type does not match and throwIfNotFond is true
     *
     */
    private String getTableName(
            Uri uri,
            boolean throwIfNotFound) {

        int type = mUriMatcher.match(uri);
        String tableName = null;

        switch(type) {
            case TYPE_RAGA:
                tableName = DbContract.RagaEntry.TABLE_NAME;
                break;

            case TYPE_LESSON:
                tableName = DbContract.LessonEntry.TABLE_NAME;
                break;

            case TYPE_LESSON_RES:
                tableName = DbContract.LessonResEntry.TABLE_NAME;
                break;

            case TYPE_SYNC:
                tableName = DbContract.SyncEntry.TABLE_NAME;
                break;

            default:
                if (throwIfNotFound) {
                    String msg = getContext().getString(R.string.err_invalid_provider_uri) + uri;
                    throw new IllegalArgumentException(msg);
                }
        }

        return tableName;
    }

    /**
     * Builds URIs for matcher
     * @return
     */
    private static UriMatcher buildUriMatcher() {
        final UriMatcher matcher = new UriMatcher(UriMatcher.NO_MATCH);
        final String authority = DbContract.CONTENT_AUTHORITY;

        matcher.addURI(authority, DbContract.PATH_RAGA, TYPE_RAGA);
        matcher.addURI(authority, DbContract.PATH_LESSON, TYPE_LESSON);
        matcher.addURI(authority, DbContract.PATH_LESSON_RES, TYPE_LESSON_RES);
        matcher.addURI(authority, DbContract.PATH_SYNC, TYPE_SYNC);

        return matcher;
    }
}
