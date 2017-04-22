package barqsoft.footballscores;

import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.support.annotation.NonNull;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import barqsoft.footballscores.service.MyFetchService;
import barqsoft.footballscores.service.ScoreWidgetService;

//
// AppWidgetProvider merely parses the relevant fields out of the Intent that is received in
// onReceive(Context,Intent), and calls hook methods with the received extras.
//
public class ScoresWidgetProvider extends AppWidgetProvider {
    private static final String LOG_TAG = ScoresWidgetProvider.class.getSimpleName();

    @Override
    public void onUpdate(Context context, AppWidgetManager appWidgetManager, int[] appWidgetIds) {
        Log.d(LOG_TAG, "onUpdate");
        context.startService(new Intent(context, MyFetchService.class));
    }

    //
    // Start the ScoreWidget service
    //
    @Override
    public void onReceive(@NonNull Context context, @NonNull Intent intent) {
        Log.d(LOG_TAG, "onReceive");
        super.onReceive(context, intent);
        if (Utilies.INTENT_ACTION_SCORE_UPDATED.equals(intent.getAction())) {
            context.startService(new Intent(context, ScoreWidgetService.class));
        }
    }
}
