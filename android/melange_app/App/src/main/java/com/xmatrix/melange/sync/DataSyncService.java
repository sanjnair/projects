package com.xmatrix.melange.sync;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;


/**
 * Created by xmatrix on 6/30/2016.
 */
public class DataSyncService extends Service {

    private static final Object mSyncAdapterLock    = new Object();
    private static DataSyncAdapter mDataSyncAdapter = null;

    @Override
    public void onCreate() {
        synchronized (mSyncAdapterLock) {
            if (mDataSyncAdapter == null) {
                mDataSyncAdapter = new DataSyncAdapter(getApplicationContext());
            }
        }
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return mDataSyncAdapter.getSyncAdapterBinder();
    }

}
