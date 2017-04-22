package com.xmatrix.melange.widget;

import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.xmatrix.melange.R;

/**
 * Created by xmatrix on 7/8/2016.
 *
 * Application Widget provider
 */
public class MelangeWidgetProvider extends AppWidgetProvider {
    private static final String LOG_TAG = MelangeWidgetProvider.class.getSimpleName();
    @Override
    public void onUpdate(
            Context context,
            AppWidgetManager appWidgetManager,
            int[] appWidgetIds) {

        Log.d(LOG_TAG, "OnUpdate called");
        // Start Melange Widget Service
        context.startService(new Intent(context, MelangeWidgetService.class));

        super.onUpdate(context, appWidgetManager, appWidgetIds);
    }

    @Override
    public void onReceive (
            Context context,
            Intent intent) {

        super.onReceive(context, intent);

        String intentAction = intent.getAction();
        String actionStr = context.getString(R.string.widget_action);
        Log.d(LOG_TAG, "onReceive called. action= " + actionStr);

        if (intentAction.equals(actionStr)) {
            Log.d(LOG_TAG, "Calling startService on MelangeWidgetService");
            context.startService(new Intent(context, MelangeWidgetService.class));
        }
    }
}
