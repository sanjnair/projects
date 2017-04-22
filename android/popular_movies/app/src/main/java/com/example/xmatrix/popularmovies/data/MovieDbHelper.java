/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.example.xmatrix.popularmovies.data;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

/**
 * Manages a local database for weather data.
 */
public class MovieDbHelper extends SQLiteOpenHelper {

    // If you change the database schema, you must increment the database version.
    static final int DATABASE_VERSION = 1;

    public static final String DATABASE_NAME           = "popular_movies.db";
    public static final String TABLE_MOVIE_LIST        = "movie_list";
    public static final String COLUMN_ID               = "_id";
    public static final String COLUMN_MOVIE_ID         = "movie_id";
    public static final String COLUMN_TITLE            = "title";
    public static final String COLUMN_IMAGE_URL        = "image_url";
    public static final String COLUMN_THUMBNAIL_URL    = "thumbnail_url";
    public static final String COLUMN_OVERVIEW         = "overview";
    public static final String COLUMN_RATING           = "rating";
    public static final String COLUMN_RELEASE_DATE     = "release_date";

    /**
     * Creates MovieDbHelper
     * @param context
     */
    public MovieDbHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase sqLiteDatabase) {
        final String SQL_CREATE_MOVIE_LIST_TABLE = "CREATE TABLE " +
                TABLE_MOVIE_LIST                + " (" +
                COLUMN_ID                       + " INTEGER PRIMARY KEY AUTOINCREMENT," +
                COLUMN_MOVIE_ID                 + " INTEGER NOT NULL,"  +
                COLUMN_TITLE                    + " TEXT NOT NULL, "    +
                COLUMN_IMAGE_URL                + " TEXT NOT NULL, "    +
                COLUMN_THUMBNAIL_URL            + " TEXT NOT NULL, "    +
                COLUMN_OVERVIEW                 + " TEXT NOT NULL, "    +
                COLUMN_RATING                   + " TEXT NOT NULL, "    +
                COLUMN_RELEASE_DATE             + " TEXT NOT NULL, "    +

                // To assure the application have just one movie entry with the id
                // it's created a UNIQUE constraint with REPLACE strategy
                " UNIQUE (" + COLUMN_MOVIE_ID + ") ON CONFLICT REPLACE);";

        sqLiteDatabase.execSQL(SQL_CREATE_MOVIE_LIST_TABLE);
    }

    @Override
    public void onUpgrade(
            SQLiteDatabase sqLiteDatabase,
            int oldVersion,
            int newVersion) {

        // This database is only a cache for online data, so its upgrade policy is
        // to simply to discard the data and start over
        // Note that this only fires if you change the version number for your database.
        // It does NOT depend on the version number for your application.
        // If you want to update the schema without wiping data, commenting out the next 2 lines
        // should be your top priority before modifying this method.
        sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + TABLE_MOVIE_LIST);
        onCreate(sqLiteDatabase);
    }
}
