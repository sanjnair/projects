package barqsoft.footballscores.service;

import android.app.IntentService;
import android.app.PendingIntent;
import android.appwidget.AppWidgetManager;
import android.content.ComponentName;
import android.content.Intent;
import android.database.Cursor;
import android.util.Log;
import android.widget.RemoteViews;
import java.text.SimpleDateFormat;
import java.util.Date;

import barqsoft.footballscores.DatabaseContract;
import barqsoft.footballscores.MainActivity;
import barqsoft.footballscores.R;
import barqsoft.footballscores.ScoresAdapter;
import barqsoft.footballscores.Utilies;
import barqsoft.footballscores.ScoresWidgetProvider;

//
// Some code snippet referred from
// https://github.com/zacharytamas/android-sunshine/blob/master/app/src/main/java/com/example/android/sunshine/app/widget/TodayWidgetIntentService.java
//

//
// IntentService handles asynchronous requests (expressed as Intents) on demand.
// Clients send requests through startService(Intent) calls; the service is started as needed,
// handles each Intent in turn using a worker thread, and stops itself when it runs out of work.
//
public class ScoreWidgetService extends IntentService {
    private static final String LOG_TAG = ScoreWidgetService.class.getSimpleName();

    //
    // Constructor
    //
    public ScoreWidgetService() {
        super(ScoreWidgetService.class.getSimpleName());
    }

    //
    // This method is invoked on the worker thread with a request to process.
    //
    @Override
    protected void onHandleIntent(Intent intent) {

        Log.d(LOG_TAG, "onHandleIntent called");

                AppWidgetManager appWidgetManager = AppWidgetManager.getInstance(this);
        int[] appWidgetIds = appWidgetManager.getAppWidgetIds(
                new ComponentName(this,
                        ScoresWidgetProvider.class));

        // Get today's date
        Date curDate = new Date(System.currentTimeMillis());
        SimpleDateFormat dateFormat = new SimpleDateFormat(this.getString(R.string.date_format));
        String sToday = dateFormat.format(curDate);

        // Today's data from database
        Cursor data = getContentResolver().query(
                            DatabaseContract.scores_table.buildScoreWithDate(),
                            null,
                            null,
                            new String[]{sToday},
                            null);

        if (data == null) {
            return;
        }
        if (!data.moveToFirst()) {
            data.close();
            return;
        }

        // Get the game information from the database cursor
        String league           = data.getString(ScoresAdapter.COL_LEAGUE);
        String homeTeam         = data.getString(ScoresAdapter.COL_HOME);
        String awayTeam         = data.getString(ScoresAdapter.COL_AWAY);
        String matchTime        = data.getString(ScoresAdapter.COL_MATCHTIME);
        String homeScore        = Integer.toString(data.getInt(ScoresAdapter.COL_HOME_GOALS));
        String awayScore        = Integer.toString(data.getInt(ScoresAdapter.COL_AWAY_GOALS));

        data.close();

        if (Utilies.isKnownLeaguue(league))
        {
            league = Utilies.getLeague(league);
        }
        else
        {
            league = "Unknown";
        }

        // Perform this loop procedure for each Today widget
        for (int appWidgetId : appWidgetIds) {
            RemoteViews views = new RemoteViews(getPackageName(), R.layout.scores_widget);

            // Add data to remove views
            views.setTextViewText(R.id.league_name, league);
            views.setTextViewText(R.id.match_time, matchTime);
            views.setTextViewText(R.id.home_team, homeTeam);
            views.setTextViewText(R.id.away_team, awayTeam);
            views.setTextViewText(R.id.home_team_Score, homeScore);
            views.setTextViewText(R.id.away_team_Score, awayScore);

            // Create an Intent to launch MainActivity
            Intent launchIntent = new Intent(this, MainActivity.class);
            PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, launchIntent, 0);
            views.setOnClickPendingIntent(R.id.id_top_layout, pendingIntent);

            // Tell the AppWidgetManager to perform an update on the current app widget
            appWidgetManager.updateAppWidget(appWidgetId, views);
        }
    }
}
