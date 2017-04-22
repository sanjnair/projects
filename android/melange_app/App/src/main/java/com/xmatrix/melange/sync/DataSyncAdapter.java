package com.xmatrix.melange.sync;

import android.accounts.Account;
import android.accounts.AccountManager;
import android.content.AbstractThreadedSyncAdapter;
import android.content.ContentProviderClient;
import android.content.ContentResolver;
import android.content.Context;
import android.content.SyncRequest;
import android.content.SyncResult;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import com.xmatrix.melange.R;
import com.xmatrix.melange.backend.melanageApi.model.LessonBean;
import com.xmatrix.melange.backend.melanageApi.model.LessonResourceBean;
import com.xmatrix.melange.backend.melanageApi.model.RagaBean;
import com.xmatrix.melange.backend.melanageApi.model.UserLessonBean;
import com.xmatrix.melange.data.DbLessonResUtil;
import com.xmatrix.melange.data.DbLessonUtil;
import com.xmatrix.melange.data.DbRagaUtil;
import com.xmatrix.melange.data.DbSyncUtil;
import com.xmatrix.melange.cloud.MelangeBackend;

import java.util.Date;
import java.util.List;

/**
 * Created by xmatrix on 6/29/2016.
 *
 * Adapter for Syncing data from clould backend
 */
public class DataSyncAdapter extends AbstractThreadedSyncAdapter {
    private static final String LOG_TAG = DataSyncAdapter.class.getSimpleName();

    // Configure sync intervals
    private static final int HOUR_IN_SECS = 60 * 60;
    private static final int MIN_IN_SECS = 60;
    private static final int SYNC_FREQUENCY = 5 * HOUR_IN_SECS;                                 // 5 hours
    private static final int SYNC_FLEXTIME = 20 * MIN_IN_SECS;                                 // 20 mins


    /**
     * Construcs Melange Sync Adapter
     *
     * @param context
     */
    public DataSyncAdapter(
            Context context) {
        super(context, true);                                                                       // set autoinitialize to true
    }

    /**
     * Perform a sync for this account. SyncAdapter-specific parameters may be specified in extras,
     * which is guaranteed to not be null. Invocations of this method are guaranteed to be serialized.
     *
     * @param account    Account: the account that should be synced
     * @param extras     SyncAdapter-specific parameters
     * @param authority  the authority of this sync request
     * @param provider   a ContentProviderClient that points to the ContentProvider for this authority
     * @param syncResult SyncAdapter-specific parameters
     */
    public void onPerformSync(
            Account account,
            Bundle extras,
            String authority,
            ContentProviderClient provider,
            SyncResult syncResult) {
        Log.d(LOG_TAG, "onPerformSync called");

        try {
            Context context = getContext();
            boolean bSuccess = syncDataFromBackend(context);

            Log.d(LOG_TAG, "Data Syncd to database. Success = " + bSuccess);

            if (bSuccess) {
                DbSyncUtil.setSyncDateFromServer(context, new Date());
                Log.d(LOG_TAG, "Sync Completed. ");
            } else {
                Log.d(LOG_TAG, "Sync Failed.");
            }

        } catch (Exception e) {
            Log.e(LOG_TAG, "Sync Failed. " + e.toString());
        }
    }

    /**
     * Initializes the  Data Sync Adapter
     *
     * @param context
     */
    public static void initializeSync(Context context) {
        Account account = getAccountToSync(context);

        if (account == null) {
            Log.e(LOG_TAG, "Unable to get account to Initialize Sync.");
        } else {
            Log.d(LOG_TAG, "Got account to Initialize Sync.");
        }
    }

    /**
     * Method to get/create an account for sync adapter.
     * <p/>
     * For now we'll use a dummy account
     */
    public static Account getAccountToSync(Context context) {

        AccountManager manager = (AccountManager) context.getSystemService(Context.ACCOUNT_SERVICE);
        String accountName = context.getString(R.string.app_name);
        String accountType = context.getString(R.string.sync_account_type);
        Account account = new Account(accountName, accountType);

        // Check if the account exists
        String password = manager.getPassword(account);
        if (password == null) {
            // We need to add account
            password = context.getString(R.string.sync_account_password);

            // attempt to add a new user
            if (false == manager.addAccountExplicitly(account, password, null)) {
                account = null;
            } else {
                // Account is created. Notify about account creation
                notifyAccountCreated(context, account);
            }
        }
        return account;
    }

    /**
     * Notification about account creation
     */
    private static void notifyAccountCreated(
            Context context,
            Account account) {

        DataSyncAdapter.configSyncRequest(context, account);
        ContentResolver.setSyncAutomatically(account, context.getString(R.string.content_authority), true);
        DataSyncAdapter.syncNow(context, account);
    }

    /**
     * Configure SyncRequest
     *
     * @param context
     * @param syncInterval
     * @param flexTime
     */
    private static void configSyncRequest(
            Context context,
            Account account) {

        // Request Sync
        String contentAuthority = context.getString(R.string.content_authority);
        int sdkVersion = Build.VERSION.SDK_INT;

        if (sdkVersion >= Build.VERSION_CODES.KITKAT) {
            SyncRequest syncRequest = new SyncRequest.Builder()
                    .syncPeriodic(SYNC_FREQUENCY, SYNC_FLEXTIME)
                    .setSyncAdapter(account, contentAuthority)
                    .setExtras(new Bundle())
                    .build();
            ContentResolver.requestSync(syncRequest);
        } else {
            ContentResolver.addPeriodicSync(account, contentAuthority, new Bundle(), SYNC_FREQUENCY);
        }
    }

    /**
     * Request a sync immediately
     *
     * @param context
     * @param account
     */
    public static void syncNow(
            Context context,
            Account account) {

        String contentAuthority = context.getString(R.string.content_authority);
        Bundle bundle = new Bundle();

        bundle.putBoolean(ContentResolver.SYNC_EXTRAS_EXPEDITED, true);
        bundle.putBoolean(ContentResolver.SYNC_EXTRAS_MANUAL, true);

        ContentResolver.requestSync(account, contentAuthority, bundle);
    }

    //----------------------------------------------------------------------------------------------

    /**
     * Validates user lesson bean for proper data
     */
    boolean isValidBean(UserLessonBean bean) {
        boolean valid = (null != bean) &&
                (null != bean.getLessonId()) &&
                (null != bean.getStartDate()) &&
                (null != bean.getEndDate()) &&
                (null != bean.getUpdateDate()) &&
                (null != bean.getLessonNumber()) &&
                (bean.getLessonNumber().intValue() >= 0);

        if (!valid) {
            Log.d(LOG_TAG, "UserLessonBean is not valid. " + bean);
            if (bean != null) {
                Log.d(
                        LOG_TAG,
                        ", id = " + bean.getLessonId() +
                                ", StartDate " + bean.getStartDate() +
                                ", EndDate " + bean.getEndDate() +
                                ", UpdateDate " + bean.getUpdateDate() +
                                ", LessonNum " + bean.getLessonNumber());
            }
        }

        return valid;
    }

    boolean isValidBean(LessonBean bean) {
        boolean valid = (null != bean) &&
                (null != bean.getId()) &&
                (null != bean.getRagaId()) &&
                (null != bean.getAuthor()) &&
                (null != bean.getName()) &&
                (null != bean.getLessonUrl()) &&
                (null != bean.getLessonPicUrl()) &&
                (null != bean.getThala()) &&
                (null != bean.getUpdateDate());

        if (!valid) {
            Log.d(LOG_TAG, "LessonBean is not valid. " + bean);
            if (bean != null) {
                Log.d(
                        LOG_TAG,
                        " id = " + bean.getId() +
                                ", RagaId " + bean.getRagaId() +
                                ", Author " + bean.getAuthor() +
                                ", Name " + bean.getName() +
                                ", LessonURL " + bean.getLessonUrl() +
                                ", PicURL " + bean.getLessonPicUrl() +
                                ", Thala " + bean.getThala() +
                                ", UpdateDate " + bean.getUpdateDate());
            }
        }

        return valid;
    }

    boolean isValidBean(RagaBean bean) {
        boolean valid = (null != bean) &&
                (null != bean.getId()) &&
                (null != bean.getName()) &&
                (null != bean.getArohana()) &&
                (null != bean.getAvarohana()) &&
                (null != bean.getMelakartha()) &&
                (null != bean.getPicture()) &&
                (null != bean.getUpdateDate());

        if (!valid) {
            Log.d(LOG_TAG, "RagaBean is not valid. " + bean);
            if (bean != null) {
                Log.d(
                        LOG_TAG,
                        " id = " + bean.getId() +
                                ", Name " + bean.getName() +
                                ", Arohana " + bean.getArohana() +
                                ", Avarohana " + bean.getAvarohana() +
                                ", Melakartha " + bean.getMelakartha() +
                                ", Picture " + bean.getPicture() +
                                ", UpdateDate " + bean.getUpdateDate());
            }
        }

        return valid;
    }

    boolean isValidBean(LessonResourceBean bean) {
        boolean valid = (null != bean) &&
                (null != bean.getId()) &&
                (null != bean.getType()) &&
                (null != bean.getResval()) &&
                (null != bean.getNum()) &&
                (null != bean.getUpdateDate());

        if (!valid) {
            Log.d(LOG_TAG, "RagaBean is not valid. " + bean);
            if (bean != null) {
                Log.d(
                        LOG_TAG,
                        " id = " + bean.getId() +
                                ", Type " + bean.getType() +
                                ", ResVal " + bean.getResval() +
                                ", Num " + bean.getNum() +
                                ", UpdateDate " + bean.getUpdateDate());
            }
        }

        return valid;
    }

    /**
     * Syncs data from backend and saves in the database
     */
    private boolean syncDataFromBackend(Context context) {
        Date prevSyncDate = DbSyncUtil.getSyncDateFromServer(context);
        boolean bSuccess = false;
        MelangeBackend backend = new MelangeBackend();
        List<UserLessonBean> lessonList = backend.getUserLessons(prevSyncDate);

        if (null != lessonList) {
            Log.d(LOG_TAG, "Number of lessons = " + lessonList.size());
            for (UserLessonBean userLessonBean : lessonList) {
                if (isValidBean(userLessonBean)) {
                    bSuccess = syncUserLesson(context, backend, userLessonBean);
                }
            }
        } else {
            Log.d(LOG_TAG, "Lesson List is NULL");
        }

        return bSuccess;
    }

    /**
     * Syncs Lesson from Backend
     */
    private boolean syncUserLesson(
            Context context,
            MelangeBackend backend,
            UserLessonBean userLessonBean) {

        boolean bSuccess = false;

        // Get the lesson from backend
        String lessonUid = userLessonBean.getLessonId();
        LessonBean lessonBean = backend.getLesson(lessonUid);
        if (!isValidBean(lessonBean)) {
            return bSuccess;
        }

        // Sync Raga
        String ragaUid = lessonBean.getRagaId();
        RagaBean ragaBean = backend.getRaga(ragaUid);
        if (!isValidBean(ragaBean)) {
            return bSuccess;
        }

        DbRagaUtil.syncRagaFromBackend(context, ragaBean);
        int ragaPk = DbRagaUtil.getPrimaryKey(context, ragaUid);
        if (ragaPk < 0) {
            Log.e(LOG_TAG, "Raga Primary key is not valid " + ragaPk);
            return bSuccess;
        }

        // Sync Lesson
        DbLessonUtil.syncLessonFromBackend(
                context,
                userLessonBean,
                lessonBean,
                ragaPk);

        int lessonPk = DbLessonUtil.getPrimaryKey(context, lessonUid);
        if (lessonPk < 0) {
            Log.e(LOG_TAG, "Lesson Primary key is not valid " + lessonPk);
            return bSuccess;
        }

        bSuccess = syncLessonResources(context, backend, lessonUid, lessonPk);

        return bSuccess;
    }

    /**
     * Syncs User Lesson Resources
     */
    private boolean syncLessonResources(
            Context context,
            MelangeBackend backend,
            String lessonUid,
            int lessonPk) {

        List<LessonResourceBean> list = backend.getLessonResources(lessonUid);
        if (list == null) {
            Log.d(LOG_TAG, "No resources found for lesson " + lessonUid);
            return true;
        }

        boolean bSuccess = false;

        for (LessonResourceBean resourceBean : list) {
            bSuccess = isValidBean(resourceBean);

            if (bSuccess) {
                DbLessonResUtil.syncLessonResourceFromBackend(
                        context,
                        resourceBean,
                        lessonPk);

                String uid = resourceBean.getId();
                int lessonResPk = DbLessonResUtil.getPrimaryKey(context, uid);
                if (lessonResPk < 0) {
                    Log.e(LOG_TAG, "LessonRes Primary key is not valid " + lessonResPk + " for uid " + uid);
                    bSuccess = false;
                    break;
                }
            } else {
                break;
            }
        }

        return bSuccess;
    }
}