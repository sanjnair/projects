package com.xmatrix.melange.widget;

import android.app.IntentService;
import android.app.PendingIntent;
import android.appwidget.AppWidgetManager;
import android.content.ComponentName;
import android.content.Intent;
import android.database.Cursor;
import android.util.Log;
import android.widget.RemoteViews;

import com.xmatrix.melange.MainActivity;
import com.xmatrix.melange.R;
import com.xmatrix.melange.data.DbContract;

/**
 * An {@link IntentService} subclass for handling asynchronous task requests in
 * a service on a separate handler thread.
 * <p/>
 * TODO: Customize class - update intent actions and extra parameters.
 */
public class MelangeWidgetService extends IntentService {

    private static final String LOG_TAG =  MelangeWidgetService.class.getSimpleName();

    //-- DATABASE Queries---------------------------------------------------------------------------
    private static final String COL_COUNT = "COUNT";
    private static final String[] LESSON_COUNT_PROJECTION = { "count(*) AS " + COL_COUNT };

    private static final String LESSON_WATCHED_SELECTION =
            String.format("%s > ?", DbContract.LessonEntry.COLUMN_WATCHED);

    private  static final String[] LESSON_WATCHED_SELECTION_VALUE = { "0" };

    //----------------------------------------------------------------------------------------------

    /**
     * Constructs Widget Service
     */
    public MelangeWidgetService() {
        super("MelangeWidgetService");
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        Log.d(LOG_TAG, "onHandleIntent called");

        AppWidgetManager widgetManager = AppWidgetManager.getInstance(this);
        int[] widgetIds = widgetManager.getAppWidgetIds(
                            new ComponentName(this, MelangeWidgetProvider.class));

        int numLessons          = getNumLessons();
        int numLessonsWatched   = getNumWatchedLessons();
        int numLessonsPending   = numLessons - numLessonsWatched;

        Log.d(LOG_TAG, "numLessons = " + numLessons + "numLessonsWatched = " + numLessonsWatched);
        Log.d(LOG_TAG, "widgetIds = " + widgetIds.length);

        for (int widgetId : widgetIds) {
            RemoteViews remoteViews = new RemoteViews(getPackageName(), R.layout.melange_widget);
            remoteViews.setTextViewText(R.id.id_widget_lessons_completed, String.valueOf(numLessonsWatched));
            remoteViews.setTextViewText(R.id.id_widget_pending_lessons, String.valueOf(numLessonsPending));

            try {
                // Create Intent to launch the Main Activity
                Intent launchIntent = new Intent(this, MainActivity.class);
                PendingIntent pendingIntent = PendingIntent.getActivity(
                                                this,                                               // context
                                                0,                                                  // requestCode
                                                launchIntent,                                       // intent
                                                PendingIntent.FLAG_UPDATE_CURRENT);                 // flags

                remoteViews.setOnClickPendingIntent(R.id.id_widget_layout, pendingIntent);

                // Set the RemoteViews to use for all AppWidget instances for
                // the supplied AppWidget provider.
                widgetManager.updateAppWidget(widgetId, remoteViews);
            } catch (Exception ex) {
                Log.e(LOG_TAG, ex.toString());
            }
        }
    }

    /**
     * Returns the number of lessons in the database
     */
    private int getNumLessons() {
        Cursor cursor = null;
        int numLessons = 0;

        try {
            cursor = getContentResolver().query(
                        DbContract.LessonEntry.CONTENT_URI,                                         // uri
                        LESSON_COUNT_PROJECTION,                                                    // projection
                        null,                                                                       // selection
                        null,                                                                       // values
                        null);                                                                      // sort

            if (cursor.moveToFirst()) {
                numLessons = cursor.getInt(cursor.getColumnIndex(COL_COUNT));
            }

        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        return numLessons;
    }

    /**
     * Returns the number of lessons in the database
     */
    private int getNumWatchedLessons() {
        Cursor cursor = null;
        int numLessons = 0;

        try {
            cursor = getContentResolver().query(
                        DbContract.LessonEntry.CONTENT_URI,                                         // uri
                        LESSON_COUNT_PROJECTION,                                                    // projection
                        LESSON_WATCHED_SELECTION,                                                   // selection
                        LESSON_WATCHED_SELECTION_VALUE,                                             // values
                        null);                                                                      // sort

            if (cursor.moveToFirst()) {
                numLessons = cursor.getInt(cursor.getColumnIndex(COL_COUNT));
            }

        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        return numLessons;
    }
}
