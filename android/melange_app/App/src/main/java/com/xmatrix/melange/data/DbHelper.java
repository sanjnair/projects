package com.xmatrix.melange.data;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.support.annotation.NonNull;
import android.util.Log;

import com.xmatrix.melange.data.DbContract.LessonEntry;
import com.xmatrix.melange.data.DbContract.LessonResEntry;
import com.xmatrix.melange.data.DbContract.RagaEntry;
import com.xmatrix.melange.data.DbContract.SyncEntry;

/**
 * Created by xmatrix on 6/26/2016.
 *
 * Db Helper class
 */
public class DbHelper extends SQLiteOpenHelper {
    private static final String LOG_TAG     = DbHelper.class.getSimpleName();

    // SQL constants
    private static final String DB_NAME             = "melange.db";
    private static final int DB_VERSION             = 1;
    static final String SQL_CREATE_TABLE            = " CREATE TABLE ";
    static final String SQL_PRIMARY_KEY             = " INTEGER PRIMARY KEY AUTOINCREMENT ";
    static final String SQL_TEXT_UNIQUE_NOT_NULL    = " TEXT UNIQUE NOT NULL ";
    static final String SQL_TEXT_NOT_NULL           = " TEXT NOT NULL ";
    static final String SQL_INT                     = " INTEGER ";
    static final String SQL_INT_NOT_NULL            = " INTEGER NOT NULL ";
    static final String SQL_FOREIGN_KEY             = " FOREIGN KEY ";
    static final String SQL_REFERENCES              =  " REFERENCES ";
    static final String SQL_COMMA                   = ", ";

    /**
     * DB Helper Constructor
     * Create a helper object to create, open, and/or manage a database.
     * This method always returns very quickly. The database is not actually created or opened
     * until one of getWritableDatabase() or getReadableDatabase() is called.
     *
     * @param context
     */
    public DbHelper(Context context) {
        super(context, DB_NAME, null, DB_VERSION);
    }

    /**
     * Override onCreate method
     * Called when the database is created for the first time.
     * This is where the creation of tables and the initial population of the tables should happen.
     */
    @Override
    public void onCreate (SQLiteDatabase db) {
        final String sqlRagaTable       = getCreateRagaTableSql();
        final String sqlLessonTable     = getCreateLessonTableSql();
        final String sqlLessonResTable  = getCreateLessonResTableSql();
        final String sqlSyncTable       = getCreateSyncTableSql();

        Log.d(LOG_TAG, "-------------------------------------------------------------------------");
        Log.d(LOG_TAG, sqlRagaTable);
        Log.d(LOG_TAG, sqlLessonTable);
        Log.d(LOG_TAG, sqlLessonResTable);
        Log.d(LOG_TAG, sqlSyncTable);
        Log.d(LOG_TAG, "-------------------------------------------------------------------------");

        db.execSQL(sqlRagaTable);
        db.execSQL(sqlLessonTable);
        db.execSQL(sqlLessonResTable);
        db.execSQL(sqlSyncTable);

        Log.d(LOG_TAG, "Successfully created tables");
    }

    /**
     * On upgrade
     */
    @Override
    public void onUpgrade (
            SQLiteDatabase db,
            int oldVersion,
            int newVersion) {
        // We are not supporting database upgrade. We'll delete and re-create all data
        dropTable(db, RagaEntry.TABLE_NAME);
        dropTable(db, LessonEntry.TABLE_NAME);
        dropTable(db, LessonResEntry.TABLE_NAME);
        dropTable(db, SyncEntry.TABLE_NAME);

        onCreate(db);
    }

    /**
     * Drop table given the table name
     */
    private void dropTable(
            @NonNull SQLiteDatabase db,
            String tableName) {
        db.execSQL("DROP TABLE IF EXISTS " + tableName);
    }

    /**
     * Get SQL for creating raga table
     */
    private String getCreateRagaTableSql() {
        String sql = SQL_CREATE_TABLE + RagaEntry.TABLE_NAME + "("                                  +
                        RagaEntry._ID                   + SQL_PRIMARY_KEY           + SQL_COMMA     +
                        RagaEntry.COLUMN_UID            + SQL_TEXT_UNIQUE_NOT_NULL  + SQL_COMMA     +
                        RagaEntry.COLUMN_NAME           + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        RagaEntry.COLUMN_AROHANA        + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        RagaEntry.COLUMN_AVAROHANA      + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        RagaEntry.COLUMN_MELAKARTHA     + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        RagaEntry.COLUMN_PIC_URL        + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        RagaEntry.COLUMN_UPDATE_DATE    + SQL_INT_NOT_NULL                          +
                     ");";

        return sql;
    }

    /**
     * Get SQL for creating lesson table
     */
    private String getCreateLessonTableSql() {
        String sqlFk = getSqlForeignKey(
                            LessonEntry.COLUMN_RAGA_ID,
                            RagaEntry.TABLE_NAME,
                            RagaEntry._ID);

        String sql = SQL_CREATE_TABLE + LessonEntry.TABLE_NAME + "("                                +
                        LessonEntry._ID                 + SQL_PRIMARY_KEY           + SQL_COMMA     +
                        LessonEntry.COLUMN_UID          + SQL_TEXT_UNIQUE_NOT_NULL  + SQL_COMMA     +
                        LessonEntry.COLUMN_NUM          + SQL_INT_NOT_NULL          + SQL_COMMA     +
                        LessonEntry.COLUMN_START_DATE   + SQL_INT_NOT_NULL          + SQL_COMMA     +
                        LessonEntry.COLUMN_END_DATE     + SQL_INT_NOT_NULL          + SQL_COMMA     +
                        LessonEntry.COLUMN_AUTHOR       + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        LessonEntry.COLUMN_RAGA_ID      + SQL_INT_NOT_NULL          + SQL_COMMA     +
                        LessonEntry.COLUMN_DETAIL       + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        LessonEntry.COLUMN_NAME         + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        LessonEntry.COLUMN_PIC_URL      + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        LessonEntry.COLUMN_THALA        + SQL_TEXT_NOT_NULL         + SQL_COMMA     +
                        LessonEntry.COLUMN_WATCHED      + SQL_INT_NOT_NULL          + SQL_COMMA     +
                        LessonEntry.COLUMN_UPDATE_DATE  + SQL_INT_NOT_NULL          + SQL_COMMA     +
                        sqlFk                                                                       +
                    ");";

        return sql;
    }

    /**
     * Get SQL for creating lesson resource table
     */
    private String getCreateLessonResTableSql() {
        String sqlFk = getSqlForeignKey(
                            LessonResEntry.COLUMN_LESSON_ID,
                            LessonEntry.TABLE_NAME,
                            LessonEntry._ID);

        String sql = SQL_CREATE_TABLE + LessonResEntry.TABLE_NAME + "("                             +
                        LessonResEntry._ID                  + SQL_PRIMARY_KEY           + SQL_COMMA +
                        LessonResEntry.COLUMN_LESSON_ID     + SQL_INT_NOT_NULL          + SQL_COMMA +
                        LessonResEntry.COLUMN_UID           + SQL_TEXT_UNIQUE_NOT_NULL  + SQL_COMMA +
                        LessonResEntry.COLUMN_TYPE          + SQL_INT_NOT_NULL          + SQL_COMMA +
                        LessonResEntry.COLUMN_RES_VAL       + SQL_TEXT_NOT_NULL         + SQL_COMMA +
                        LessonResEntry.COLUMN_DESCRIPTION   + SQL_TEXT_NOT_NULL         + SQL_COMMA +
                        LessonResEntry.COLUMN_NUM           + SQL_INT_NOT_NULL          + SQL_COMMA +
                        LessonResEntry.COLUMN_UPDATE_DATE   + SQL_INT_NOT_NULL          + SQL_COMMA +
                        sqlFk                                                                       +
                    ");";

        return sql;
    }

    /**
     * Get SQL for creating table
     */
    private String getCreateSyncTableSql() {

        String sql = SQL_CREATE_TABLE + SyncEntry.TABLE_NAME + "("                                  +
                SyncEntry._ID                           + SQL_PRIMARY_KEY   + SQL_COMMA             +
                SyncEntry.COLUMN_SYNC_FROM_SERVER_DATE  + SQL_INT           + SQL_COMMA             +
                SyncEntry.COLUMN_SYNC_TO_SERVER_DATE    + SQL_INT                                   +
                ");";

        return sql;
    }

    /**
     * Get the Foreign key constraint SQL
     */
    static String getSqlForeignKey(
            String curColName,
            String refTableName,
            String refColName)
    {
        String sql = SQL_FOREIGN_KEY + "(" + curColName + ")" +
                     SQL_REFERENCES + refTableName + "(" +  refColName + ")";

        return sql;
    }
}