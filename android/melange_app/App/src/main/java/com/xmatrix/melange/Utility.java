package com.xmatrix.melange;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.media.MediaPlayer;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;

/**
 * Created by xmatrix on 7/2/2016.
 */
public final class Utility {

    private static final String LOG_TAG =  Utility.class.getSimpleName();

    static public boolean isNetworkAvailable(Context context) {
        ConnectivityManager manager = (ConnectivityManager)
                                        context.getSystemService(Context.CONNECTIVITY_SERVICE);

        NetworkInfo networkInfo = manager.getActiveNetworkInfo();
        return ((networkInfo != null) && networkInfo.isConnected());
    }

    /**
     * Start youtube activity
     * @param activity
     * @param id
     */
    public static void playYoutubeVideo(
            Activity activity,
            String id){
        try {
            Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse("vnd.youtube:" + id));
            activity.startActivity(intent);
        } catch (ActivityNotFoundException ex) {
            Intent intent = new Intent(Intent.ACTION_VIEW,
                    Uri.parse("http://www.youtube.com/watch?v=" + id));
            activity.startActivity(intent);
        }
    }
}
