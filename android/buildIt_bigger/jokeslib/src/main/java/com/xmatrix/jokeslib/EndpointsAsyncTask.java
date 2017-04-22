package com.xmatrix.jokeslib;

import android.content.Context;
import android.os.AsyncTask;
import android.util.Pair;

import java.io.IOException;

import com.google.api.client.googleapis.services.AbstractGoogleClientRequest;
import com.google.api.client.extensions.android.json.AndroidJsonFactory;
import com.google.api.client.googleapis.services.GoogleClientRequestInitializer;
import com.google.api.client.extensions.android.http.AndroidHttp;
import com.xmatrix.backend.myApi.MyApi;

/**
 * Created by xmatrix on 1/5/2016.
 * Reference:
 * https://github.com/GoogleCloudPlatform/gradle-appengine-templates/tree/master/HelloEndpoints
 */
public class EndpointsAsyncTask extends AsyncTask<Context, Void, String> {

    /**
     * Interface for Joke Notifier
     */
    public interface JokesNotifier {
        void notifyJoke(String sJoke);
    }

    private static final String ROOT_URL = "https://udacity-builditbigger-ssnair.appspot.com/_ah/api/";
    private static MyApi mApiService = null;
    private Context mContext;
    JokesNotifier mNotifier;

    /**
     * Creates EndpointsAsyncTask
     */
    public EndpointsAsyncTask(JokesNotifier notifier) {
        mNotifier = notifier;
    }

    /**
     * Runs on background
     */
    @Override
    protected String doInBackground(Context... params) {

        if(mApiService == null) {  // Only do this once
            MyApi.Builder builder = new MyApi.Builder(
                                        AndroidHttp.newCompatibleTransport(),
                                        new AndroidJsonFactory(),
                                        null)
                                            .setRootUrl(ROOT_URL)
                                            .setGoogleClientRequestInitializer(new GoogleClientRequestInitializer() {

                                                @Override
                                                public void initialize(AbstractGoogleClientRequest<?> abstractGoogleClientRequest) throws IOException {
                                                    abstractGoogleClientRequest.setDisableGZipContent(true);
                                                }
                                            });

            mApiService = builder.build();
        }

        mContext = params[0];

        try {
            return mApiService.getJoke().execute().getData();
        } catch (IOException e) {
            return e.getMessage();
        }
    }

    /**
     * After the data is fetched
     * @param result
     */
    @Override
    protected void onPostExecute(String result) {
        if (mNotifier != null) {
            mNotifier.notifyJoke(result);
        }
    }
}