package com.udacity.gradle.builditbigger;

import android.test.AndroidTestCase;

import com.xmatrix.jokeslib.EndpointsAsyncTask;

/**
 * Created by xmatrix on 1/13/2016.
 */
public class AsyncTaskTest extends AndroidTestCase {

    /**
     * Function tests Async task functionality
     */
    public void testAsyncTask() {
        EndpointsAsyncTask asyncTask = new EndpointsAsyncTask(null);
        asyncTask.execute(mContext);

        String sJoke = null;

        // Get the joke from the task
        try {
            sJoke = asyncTask.get();
        } catch (Exception exp)
        {
            exp.printStackTrace();
        }

        // Check if we got a valid joke
        assertNotNull(sJoke);
    }
}
