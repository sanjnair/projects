package com.xmatrix.melange.sync;

import android.accounts.AbstractAccountAuthenticator;
import android.accounts.Account;
import android.accounts.AccountAuthenticatorResponse;
import android.content.Context;
import android.os.Bundle;

/**
 * Created by xmatrix on 6/30/2016.
 */
public class DataSyncAuthenticator extends AbstractAccountAuthenticator {

    public DataSyncAuthenticator(Context context) {
        super(context);
    }

    @Override
    public Bundle addAccount (
            AccountAuthenticatorResponse response,
            String accountType,
            String authTokenType,
            String[] requiredFeatures,
            Bundle options) {

        return null;
    }

    @Override
    public Bundle confirmCredentials(
            AccountAuthenticatorResponse response,
            Account account,
            Bundle options) {
        return null;
    }

    @Override
    public Bundle editProperties(
            AccountAuthenticatorResponse response,
            String accountType) {

        return null;
    }


    @Override
    public Bundle getAuthToken (
            AccountAuthenticatorResponse response,
            Account account,
            String authTokenType,
            Bundle options) {

        return null;
    }


    @Override
    public String getAuthTokenLabel(
            String authTokenType) {

        return null;
    }

    @Override
    public Bundle hasFeatures(
            AccountAuthenticatorResponse response,
            Account account,
            String[] features) {

        return null;
    }

    @Override
    public Bundle updateCredentials(
            AccountAuthenticatorResponse response,
            Account account,
            String authTokenType,
            Bundle options) {

        return null;
    }
}
