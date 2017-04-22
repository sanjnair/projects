package com.xmatrix.melange;

import android.content.Intent;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.google.android.gms.analytics.HitBuilders;
import com.google.android.gms.analytics.Tracker;
import com.google.android.gms.auth.api.Auth;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInResult;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.SignInButton;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.OptionalPendingResult;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.common.api.Status;

public class LoginActivity
        extends AppCompatActivity
        implements GoogleApiClient.OnConnectionFailedListener,
        View.OnClickListener
{
    //----------------------------------------------------------------------------------------------
    private static final String LOG_TAG = LoginActivity.class.getSimpleName();
    private static final int RC_SIGN_IN = 9001;

    private View mProgressDlg;
    private GoogleApiClient mGoogleApiClient;
    private Tracker mTracker;

    //----------------------------------------------------------------------------------------------

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        // Initialize the tracker
        MelangeApplication application = (MelangeApplication) getApplication();
        mTracker = application.getDefaultTracker();

        // Configure sign-in to request the user's ID, email address, and basic
        // profile. ID and basic profile are included in DEFAULT_SIGN_IN.
        GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                                        .requestEmail()
                                        .build();

        // Build a GoogleApiClient with access to the Google Sign-In API and the
        // options specified by gso.

        mGoogleApiClient = new GoogleApiClient.Builder(this)
                .enableAutoManage(this /* FragmentActivity */, this /* OnConnectionFailedListener */)
                .addApi(Auth.GOOGLE_SIGN_IN_API, gso)
                .build();

        mProgressDlg = findViewById(R.id.login_progress);

        SignInButton signInButton = (SignInButton) findViewById(R.id.sign_in_button);
        signInButton.setSize(SignInButton.SIZE_WIDE);
        signInButton.setScopes(gso.getScopeArray());
        signInButton.setOnClickListener(this);
    }

    /**
     * onStart call
     */
    @Override
    public void onStart() {
        super.onStart();

        if (mGoogleApiClient != null) {
            mGoogleApiClient.connect();
        }

        OptionalPendingResult<GoogleSignInResult> pendingResult =
                Auth.GoogleSignInApi.silentSignIn(mGoogleApiClient);
        if (pendingResult.isDone()) {
            Log.d(LOG_TAG, "onStart - Login - Immediate");
            // There's immediate result available.
            updateSignInResult(pendingResult.get());
        } else {
            // There's no immediate result ready, displays some progress indicator and waits for the
            // async callback.
            Log.d(LOG_TAG, "onStart - Login - Async");
            mProgressDlg.setVisibility(View.VISIBLE);
            pendingResult.setResultCallback(new ResultCallback<GoogleSignInResult>() {
                @Override
                public void onResult(@NonNull GoogleSignInResult result) {
                    updateSignInResult(result);
                    mProgressDlg.setVisibility(View.INVISIBLE);
                }
            });
        }
    }

    /**
     * onResume call
     */
    @Override
    protected void onResume() {
        Log.d(LOG_TAG, "onResume");
        super.onResume();
        mTracker.setScreenName(getString(R.string.tracker_login));
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());
    }


    /**
     * onActivityResult callback
     * @param requestCode
     * @param resultCode
     * @param data
     */
    @Override
    public void onActivityResult(
            int requestCode,
            int resultCode,
            Intent data) {

        Log.d(LOG_TAG, "onActivityResult. ResultCode = " + resultCode);
        super.onActivityResult(requestCode, resultCode, data);

        // from GoogleSignInApi.getSignInIntent(...);
        if (requestCode == RC_SIGN_IN) {
            GoogleSignInResult result = Auth.GoogleSignInApi.getSignInResultFromIntent(data);
            updateSignInResult(result);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mGoogleApiClient.stopAutoManage(this);
        mGoogleApiClient.disconnect();
    }

    /**
     * Interface method
     * @param result
     */
    @Override
    public void onConnectionFailed(@NonNull ConnectionResult result) {
        Log.d(LOG_TAG, "onConnectionFailed called. Result =  " + result);
    }



    /**
     * OnClick handler
     * @param view
     */
    @Override
    public void onClick(View view) {
        Log.d(LOG_TAG, "onClick");
        if (view.getId() == R.id.sign_in_button) {
            signIn();
        }
    }


    /**
     * Update the signin result
     * @param result
     */
    private void updateSignInResult(GoogleSignInResult result) {
        Log.d(LOG_TAG, "updateSignInResult: Success=" + result.isSuccess());
        if (result.isSuccess()) {
            GoogleSignInAccount loginAccount = result.getSignInAccount();
            AppSingleton.getInstance().setUserAccount(this, loginAccount);

            displayMainActivity();
            updateScreenState(true);
        } else {
            // Signed out, show unauthenticated UI.
            updateScreenState(false);
        }
    }


    /**
     * Helper method for Login
     */
    private void signIn() {
        Log.d(LOG_TAG, "signIn");
        Intent signInIntent = Auth.GoogleSignInApi.getSignInIntent(mGoogleApiClient);
        startActivityForResult(signInIntent, RC_SIGN_IN);
    }

    /**
     * Helper method for Logout
     */
    private void signOut() {
        Log.d(LOG_TAG, "signOut");
        Auth.GoogleSignInApi.signOut(mGoogleApiClient).setResultCallback(
                new ResultCallback<Status>() {
                    @Override
                    public void onResult(Status status) {
                        updateScreenState(false);
                    }
                });
    }

    /**
     * Displays main activity
     */
    private void displayMainActivity() {
        Log.d(LOG_TAG, "displayMainActivity");
        Intent intent = new Intent(this, MainActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(intent);
        finish();
    }


    /**
     * Updates the screen state
     */
    private void updateScreenState(boolean loggedIn) {
        Log.d(LOG_TAG, "updateScreenState: loggedin? " + loggedIn);
        SignInButton signInButton = (SignInButton) findViewById(R.id.sign_in_button);

        if (loggedIn) {
            signInButton.setVisibility(View.GONE);
        } else {
            signInButton.setVisibility(View.VISIBLE);
        }
    }
}
