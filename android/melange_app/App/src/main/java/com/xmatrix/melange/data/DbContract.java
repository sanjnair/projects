package com.xmatrix.melange.data;

import android.content.ContentResolver;
import android.content.ContentUris;
import android.net.Uri;
import android.provider.BaseColumns;

import java.util.List;

/**
 * Created by xmatrix on 6/26/2016.
 *
 * Defined db tables
 */
public class DbContract {
    public static final String CONTENT_AUTHORITY    = "com.xmatrix.melange";
    public static final Uri BASE_CONTENT_URI        = Uri.parse("content://" + CONTENT_AUTHORITY);

    public static final String PATH_LESSON          = "Lesson";
    public static final String PATH_LESSON_RES      = "LessonRes";
    public static final String PATH_RAGA            = "Raga";
    public static final String PATH_SYNC            = "Sync";

    //----------------------------------------------------------------------------------------------
    // Table for Raga Entry
    //----------------------------------------------------------------------------------------------
    public static final class RagaEntry implements BaseColumns {
        public static final Uri CONTENT_URI             = BASE_CONTENT_URI.buildUpon().appendPath(PATH_RAGA).build();
        public static final String CONTENT_TYPE         = ContentResolver.CURSOR_DIR_BASE_TYPE + "/" + CONTENT_AUTHORITY + "/" + PATH_RAGA;
        public static final String CONTENT_ITEM_TYPE    = ContentResolver.CURSOR_ITEM_BASE_TYPE + "/" + CONTENT_AUTHORITY + "/" + PATH_RAGA;

        // Name of the table
        public static final String TABLE_NAME                       = "raga";

        // columns
        public static final String COLUMN_UID                       = "uid";
        public static final String COLUMN_NAME                      = "name";
        public static final String COLUMN_AROHANA                   = "arohana";
        public static final String COLUMN_AVAROHANA                 = "avarohana";
        public static final String COLUMN_MELAKARTHA                = "melakartha";
        public static final String COLUMN_PIC_URL                   = "pic_url";
        public static final String COLUMN_UPDATE_DATE               = "update_date";

        public static Uri buildRagaUri(long id) {
            return ContentUris.withAppendedId(CONTENT_URI, id);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Table for Lesson Entry
    //----------------------------------------------------------------------------------------------
    public static final class LessonEntry implements BaseColumns {

        public static final Uri CONTENT_URI             = BASE_CONTENT_URI.buildUpon().appendPath(PATH_LESSON).build();
        public static final String CONTENT_TYPE         = ContentResolver.CURSOR_DIR_BASE_TYPE + "/" + CONTENT_AUTHORITY + "/" + PATH_LESSON;
        public static final String CONTENT_ITEM_TYPE    = ContentResolver.CURSOR_ITEM_BASE_TYPE + "/" + CONTENT_AUTHORITY + "/" + PATH_LESSON;

        // Name of the table
        public static final String TABLE_NAME                       = "lesson";

        // columns
        public static final String COLUMN_UID                       = "uid";
        public static final String COLUMN_NUM                       = "num";
        public static final String COLUMN_START_DATE                = "start_date";
        public static final String COLUMN_END_DATE                  = "end_date";
        public static final String COLUMN_AUTHOR                    = "author";
        public static final String COLUMN_RAGA_ID                   = "raga_id";
        public static final String COLUMN_DETAIL                    = "detail";
        public static final String COLUMN_NAME                      = "name";
        public static final String COLUMN_PIC_URL                   = "pic_url";
        public static final String COLUMN_THALA                     = "thala";
        public static final String COLUMN_WATCHED                   = "watched";
        public static final String COLUMN_UPDATE_DATE               = "update_date";

        public static Uri buildLessonUri(long id) {
            return ContentUris.withAppendedId(CONTENT_URI, id);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Table for Lesson Resources
    //----------------------------------------------------------------------------------------------
    public static final class LessonResEntry implements BaseColumns {

        public static final Uri CONTENT_URI             = BASE_CONTENT_URI.buildUpon().appendPath(PATH_LESSON_RES).build();
        public static final String CONTENT_TYPE         = ContentResolver.CURSOR_DIR_BASE_TYPE + "/" + CONTENT_AUTHORITY + "/" + PATH_LESSON_RES;
        public static final String CONTENT_ITEM_TYPE    = ContentResolver.CURSOR_ITEM_BASE_TYPE + "/" + CONTENT_AUTHORITY + "/" + PATH_LESSON_RES;

        // Name of the table
        public static final String TABLE_NAME           = "lesson_res";

        // columns
        public static final String COLUMN_LESSON_ID     = "lesson_id";
        public static final String COLUMN_UID           = "uid";
        public static final String COLUMN_TYPE          = "type";
        public static final String COLUMN_RES_VAL       = "res_val";
        public static final String COLUMN_DESCRIPTION   = "description";
        public static final String COLUMN_NUM           = "num";
        public static final String COLUMN_UPDATE_DATE   = "update_date";


        public static Uri buildLessonResUri(long id) {
            return ContentUris.withAppendedId(CONTENT_URI, id);
        }

        public static Uri buildLessonResForLessonId(long lessonId) {
            String sLessonId = Long.toString(lessonId);
            return CONTENT_URI.buildUpon().appendPath(COLUMN_LESSON_ID).appendPath(sLessonId).build();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Table for Sync Details
    //----------------------------------------------------------------------------------------------
    public static final class SyncEntry implements BaseColumns {
        public static final Uri CONTENT_URI             = BASE_CONTENT_URI.buildUpon().appendPath(PATH_SYNC).build();
        public static final String CONTENT_TYPE         = ContentResolver.CURSOR_DIR_BASE_TYPE + "/" + CONTENT_AUTHORITY + "/" + PATH_SYNC;
        public static final String CONTENT_ITEM_TYPE    = ContentResolver.CURSOR_ITEM_BASE_TYPE + "/" + CONTENT_AUTHORITY + "/" + PATH_SYNC;

        // Name of the table
        public static final String TABLE_NAME                       = "sync";

        // columns
        public static final String COLUMN_SYNC_FROM_SERVER_DATE     = "sync_from_server_date";
        public static final String COLUMN_SYNC_TO_SERVER_DATE       = "sync_to_server_date";

        public static Uri buildSyncUri(long id) {
            return ContentUris.withAppendedId(CONTENT_URI, id);
        }

    }
}
