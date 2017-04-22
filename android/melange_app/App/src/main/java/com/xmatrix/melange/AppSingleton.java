package com.xmatrix.melange;

import android.content.Context;
import android.content.SharedPreferences;

import com.google.android.gms.auth.api.signin.GoogleSignInAccount;

/**
 * Created by xmatrix on 6/15/2016.
 */
public class AppSingleton {

    private GoogleSignInAccount mUserAccount;
    private static AppSingleton mInstance = new AppSingleton();

    public static AppSingleton getInstance() {
        return mInstance;
    }

    private AppSingleton() {
    }

    /**
     * Get shared preferences
     */
    public SharedPreferences getSharedPreferences(Context context) {
        SharedPreferences preferences = context.getSharedPreferences(
                context.getString(R.string.key_shared_preferences),
                Context.MODE_PRIVATE);
        return preferences;
    }

    /**
     * Gets Logged in user's account information
     *
     * @return
     */
    public GoogleSignInAccount getUserAccount() {
        return mUserAccount;
    }

    /**
     * Sets user's account information
     */
    public void setUserAccount(
            Context context,
            GoogleSignInAccount account) {

        mUserAccount = account;

        // We'll put user's email ID in shared preferences so that it can be use for auto-sync
        SharedPreferences preferences = getSharedPreferences(context);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(context.getString(R.string.key_user_email), account.getEmail());
        editor.commit();
    }

    /**
     * Get user's email
     * @param context
     * @return
     */
    public String getUserEmail(Context context) {
        String email;

        if (mUserAccount != null) {
            email = mUserAccount.getEmail();
        }
        else {
            email = getSharedPreferences(context).getString(context.getString(R.string.key_user_email), "");
        }
        return email;
    }

    /**
     * Sets the last selected drawer item id
     */
    public void setSelectedDrawerItemId(
            Context context,
            int id) {

        SharedPreferences preferences = getSharedPreferences(context);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putInt(context.getString(R.string.key_selected_drawer_item_id), id);
        editor.commit();
    }

    /**
     * Returns the selected drawer item id
     */
    public int getSelectedDrawerItemId(
            Context context) {

        String key = context.getString(R.string.key_selected_drawer_item_id);
        SharedPreferences preferences = getSharedPreferences(context);
        return preferences.getInt(key, R.id.nav_lessons);
    }
}