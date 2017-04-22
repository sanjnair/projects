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

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import com.example.xmatrix.popularmovies.BundleKeys;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

public class MovieDataSource
{
    // Database fields
    SQLiteDatabase m_database;
    MovieDbHelper m_dbHelper;

    String[] MOVIE_LIST_COLUMNS = {
        MovieDbHelper.COLUMN_ID,
        MovieDbHelper.COLUMN_MOVIE_ID,
        MovieDbHelper.COLUMN_TITLE,
        MovieDbHelper.COLUMN_IMAGE_URL,
        MovieDbHelper.COLUMN_THUMBNAIL_URL,
        MovieDbHelper.COLUMN_OVERVIEW,
        MovieDbHelper.COLUMN_RATING,
        MovieDbHelper.COLUMN_RELEASE_DATE
        };

    /**
     * Creates a Datasource object
     */
    public MovieDataSource(Context context) {
        m_dbHelper = new MovieDbHelper(context);
    }

    /**
     * Opens the database connection
     */
    public void open() throws SQLException
    {
        m_database = m_dbHelper.getWritableDatabase();
    }

    /**
     * Closes the open connection
     */
    public void close()
    {
        if (m_database != null)
        {
            m_database.close();
            m_database = null;
        }
    }

    /**
     * Inserts movie information to database and returns the unique id
     */
    public long insertMovie(Bundle bundle)
    {
        ContentValues values = new ContentValues();
        values.put(MovieDbHelper.COLUMN_MOVIE_ID, bundle.getLong(BundleKeys.MOVIE_ID));
        values.put(MovieDbHelper.COLUMN_TITLE, bundle.getString(BundleKeys.TITLE));
        values.put(MovieDbHelper.COLUMN_IMAGE_URL, bundle.getString(BundleKeys.IMAGE_URL));
        values.put(MovieDbHelper.COLUMN_THUMBNAIL_URL, bundle.getString(BundleKeys.THUMBNAIL_URL));
        values.put(MovieDbHelper.COLUMN_OVERVIEW, bundle.getString(BundleKeys.OVERVIEW));
        values.put(MovieDbHelper.COLUMN_RATING, bundle.getString(BundleKeys.RATING));
        values.put(MovieDbHelper.COLUMN_RELEASE_DATE, bundle.getString(BundleKeys.RELEASE_DATE));

        long id = m_database.insert(MovieDbHelper.TABLE_MOVIE_LIST, null, values);
        return id;
    }

    /**
     * Deletes the movie information from the database
     */
    public void deleteMovie(long id) {
        m_database.delete(
                MovieDbHelper.TABLE_MOVIE_LIST,
                MovieDbHelper.COLUMN_ID + " = " + id, null);
    }

    public ArrayList<Bundle> getAllMovies() {
        ArrayList<Bundle> lstMovies = new ArrayList<Bundle>();

        Cursor cursor = m_database.query(
                            MovieDbHelper.TABLE_MOVIE_LIST,
                            MOVIE_LIST_COLUMNS,
                            null,
                            null,
                            null,
                            null,
                            null);

        if (cursor.getCount() > 0)
        {
            cursor.moveToFirst();
            while (!cursor.isAfterLast()) {
                Bundle movieBundle = cursorToMovie(cursor);
                lstMovies.add(movieBundle);

                cursor.moveToNext();
            }
        }

        // Close the cursor
        cursor.close();

        return lstMovies;
    }

    /**
     * Get movie bundle for the given id. If not found, it returns null
     */
    public Bundle getMovie(long movieId)
    {
        Bundle movieBundle = null;

        Cursor cursor = m_database.query(
                            MovieDbHelper.TABLE_MOVIE_LIST,
                            MOVIE_LIST_COLUMNS,
                            MovieDbHelper.COLUMN_MOVIE_ID + " = " + movieId,
                            null,
                            null,
                            null,
                            null);

        if (cursor.getCount() > 0) {
            cursor.moveToFirst();
            movieBundle = cursorToMovie(cursor);
        }

        // Close the cursor
        cursor.close();

        return movieBundle;
    }

    /**
     * Creates Movie bundle given a valid cursor
     */
    Bundle cursorToMovie(Cursor cursor) {
        Bundle bundle = new Bundle();

        bundle.putLong(
                BundleKeys.MOVIE_DB_ID,
                cursor.getLong(cursor.getColumnIndex(MovieDbHelper.COLUMN_ID)));

        bundle.putLong(
                BundleKeys.MOVIE_ID,
                cursor.getLong(cursor.getColumnIndex(MovieDbHelper.COLUMN_MOVIE_ID)));

        bundle.putString(
                BundleKeys.TITLE,
                cursor.getString(cursor.getColumnIndex(MovieDbHelper.COLUMN_TITLE)));

        bundle.putString(
                BundleKeys.IMAGE_URL,
                cursor.getString(cursor.getColumnIndex(MovieDbHelper.COLUMN_IMAGE_URL)));

        bundle.putString(
                BundleKeys.THUMBNAIL_URL,
                cursor.getString(cursor.getColumnIndex(MovieDbHelper.COLUMN_THUMBNAIL_URL)));

        bundle.putString(
                BundleKeys.OVERVIEW,
                cursor.getString(cursor.getColumnIndex(MovieDbHelper.COLUMN_OVERVIEW)));

        bundle.putString(
                BundleKeys.RATING,
                cursor.getString(cursor.getColumnIndex(MovieDbHelper.COLUMN_RATING)));

        bundle.putString(
                BundleKeys.RELEASE_DATE,
                cursor.getString(cursor.getColumnIndex(MovieDbHelper.COLUMN_RELEASE_DATE)));

        return bundle;
    }
}
