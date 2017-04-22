package com.xmatrix.melange;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.View;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.widget.TextView;

import com.google.android.gms.analytics.HitBuilders;
import com.google.android.gms.analytics.Tracker;
import com.google.android.gms.auth.api.Auth;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.Status;
import com.google.android.gms.common.api.ResultCallback;
import com.xmatrix.melange.sync.DataSyncAdapter;

public class MainActivity extends AppCompatActivity implements
        NavigationView.OnNavigationItemSelectedListener,
        GoogleApiClient.OnConnectionFailedListener,
        LessonsFragment.LessonClickListener,
        LessonDetailFragment.OnFragmentInteractionListener{

    //----------------------------------------------------------------------------------------------
    private static final String LOG_TAG = MainActivity.class.getSimpleName();

    private GoogleApiClient mGoogleApiClient;
    private boolean mTwoPaneMode;
    private Tracker mTracker;
    //----------------------------------------------------------------------------------------------

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        // Set the content view
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        // Retrieve the tracker from the application
        MelangeApplication application = (MelangeApplication) getApplication();
        mTracker = application.getDefaultTracker();

        if (findViewById(R.id.lesson_detail_fragment_container) != null) {
            mTwoPaneMode = true;
        }

        // Set Drawer settings
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle drawerToggle = new ActionBarDrawerToggle(
                this,
                drawer,
                toolbar,
                R.string.navigation_drawer_open,
                R.string.navigation_drawer_close);
        drawer.addDrawerListener(drawerToggle);
        drawerToggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        MenuItem menuItem = navigationView.getMenu().findItem(AppSingleton.getInstance().getSelectedDrawerItemId(this));
        if (menuItem == null) {
            menuItem = navigationView.getMenu().getItem(0);
        }
        menuItem.setChecked(true);
        navigationView.setNavigationItemSelectedListener(this);

        // Initialize GSO
        GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                .requestEmail()
                .build();

        mGoogleApiClient = new GoogleApiClient.Builder(this)
                .enableAutoManage(this, this)
                .addApi(Auth.GOOGLE_SIGN_IN_API, gso)
                .build();

        setUserInformation(navigationView);

        // Initialize Sync Adapter
        DataSyncAdapter.initializeSync(this);

        //-- Select the navigation item ------------------------------------------------------------
        //if (savedInstanceState == null) {
            int itemId = AppSingleton.getInstance().getSelectedDrawerItemId(this);
            if (itemId >= 0) {
//                menuItem = navigationView.getMenu().findItem(itemId);
//                if (menuItem == null) {
//                    menuItem = navigationView.getMenu().getItem(0);
//                }
                setNavigationFragment(itemId);
            }
        //}
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onNavigationItemSelected(MenuItem item) {

        try {
            // Handle navigation view item clicks here.
            int id = item.getItemId();

            switch (id) {
                case R.id.nav_lessons:
                    setNavigationFragment(id);
                    break;

                case R.id.nav_logout:
                    logout();
                    break;

                default:
                    Log.d(LOG_TAG, "Unknown navigation ID " + id);
            }

            DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
            drawer.closeDrawer(GravityCompat.START);
        } catch (Exception ex) {
            Log.e(LOG_TAG, ex.toString());
        }

        return true;
    }

    @Override
    public void onConnectionFailed(@NonNull ConnectionResult connectionResult) {
        Log.d(LOG_TAG, "onConnectionFailed - called");
    }


    /**
     * LessonsFragment.LessonClickListener interface
     *
     * @param lessonId
     */
    @Override
    public void onLessonClick(int lessonId) {
        Log.d(LOG_TAG, "onLessonClick called");

        if (mTwoPaneMode) {
            // Replace the container fragment
            LessonDetailFragment fragment = LessonDetailFragment.newInstance(lessonId);
            getSupportFragmentManager().beginTransaction().replace(
                    R.id.lesson_detail_fragment_container, fragment).commit();
        } else {
            Log.d(LOG_TAG, "Starting LessonDetail Activity");
            Intent intent = new Intent(this, LessonDetailActivity.class);
            intent.putExtra(LessonDetailActivity.LESSON_ID, lessonId);
            this.startActivity(intent);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        // Tracker
        mTracker.setScreenName(getString(R.string.tracker_lessons));
        mTracker.send(new HitBuilders.ScreenViewBuilder().build());

        // TODO: Implement here
    }

    /**
     * LessonDetailFragment.OnFragmentInteractionListener{
     * Empty for now
     */
    public void onFragmentInteraction(Uri uri) {
        Log.d(LOG_TAG, "onFragmentInteraction called");
    }


    /**
     * Set Navigation fragment as main fragement
     */
    private void setNavigationFragment(int id) {
        Fragment fragment = null;

        if (id == R.id.nav_lessons) {
            fragment = new LessonsFragment();
        } else {
            Log.d(LOG_TAG, "Unknown fragment id " + id);
        }

        if (fragment != null) {
            Log.d(LOG_TAG, "nav_lessons: replacing container");
            getSupportFragmentManager().beginTransaction().replace(
                    R.id.drawer_list_fragment_container, fragment).commit();

            //Set the selected in app singleton
            AppSingleton.getInstance().setSelectedDrawerItemId(this, id);
        }
    }


    /**
     * Set the looged in user's information
     */
    private void setUserInformation(NavigationView navView) {

        View headerView = navView.getHeaderView(0);
        TextView nameView = (TextView) headerView.findViewById(R.id.user_name);
        TextView emailView = (TextView) headerView.findViewById(R.id.user_email);

        GoogleSignInAccount account = AppSingleton.getInstance().getUserAccount();
        if (account != null) {
            nameView.setText(account.getDisplayName());
            emailView.setText(account.getEmail());
        }
    }

    /**
     * Logout from the app
     */
    private void logout() {
        if (!mGoogleApiClient.isConnected()) {
            return;
        }
        Auth.GoogleSignInApi.signOut(mGoogleApiClient).setResultCallback(
                new ResultCallback<Status>() {
                    @Override
                    public void onResult(Status status) {
                        Intent intent = new Intent(MainActivity.this, LoginActivity.class);
                        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
                        MainActivity.this.startActivity(intent);
                        finish();
                    }
                });
    }
}
