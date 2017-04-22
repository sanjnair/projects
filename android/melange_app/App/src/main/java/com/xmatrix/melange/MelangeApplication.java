package com.xmatrix.melange;

import android.app.Application;
import com.google.android.gms.analytics.GoogleAnalytics;
import com.google.android.gms.analytics.Tracker;

/**
 * Created by xmatrix on 6/15/2016.
 *
 * * This is a subclass of {@link Application} used to provide shared objects for this app, such as
 * the {@link Tracker}.
 *
 * reference: https://developers.google.com/analytics/devguides/collection/android/v4/#activity-or-fragment
 */
public class MelangeApplication extends Application{

    private static final String PROPERTY_ID = "UA-79460413-1";  // From Google Analytics
    private Tracker mTracker;

    /**
     * Gets the default {@link Tracker} for this {@link Application}
     * @return tracker
     */
    synchronized public Tracker getDefaultTracker() {
        if (mTracker == null) {
            GoogleAnalytics analytics = GoogleAnalytics.getInstance(this);
            // To enable debug logging use: adb shell setprop log.tag.GAv4 DEBUG
            mTracker = analytics.newTracker(PROPERTY_ID);
        }
        return mTracker;
    }
}
