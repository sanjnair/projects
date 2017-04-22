package com.xmatrix.melange.sync;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

/**
 * Created by xmatrix on 6/30/2016.
 */
public class DataSyncAuthService extends Service {

    private DataSyncAuthenticator mAuthenticator;

    public DataSyncAuthService() {}

    @Override
    public void onCreate() {

        mAuthenticator = new DataSyncAuthenticator(this);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mAuthenticator.getIBinder();
    }
}
