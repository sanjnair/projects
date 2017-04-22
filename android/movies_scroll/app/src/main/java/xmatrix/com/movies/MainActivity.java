package xmatrix.com.movies;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.NavigationView;
import android.support.v4.app.Fragment;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.google.android.gms.auth.api.Auth;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.common.api.Status;

public class MainActivity extends AppCompatActivity implements
        NavigationView.OnNavigationItemSelectedListener,
        GoogleApiClient.OnConnectionFailedListener,
        MovieListFragment.MovieClickListener {
        //LessonDetailFragment.OnFragmentInteractionListener //TODO

    //----------------------------------------------------------------------------------------------
    private static final String LOG_TAG = MainActivity.class.getSimpleName();
    private GoogleApiClient mGoogleApiClient;
    private MovieListFragment mMovieListFragment;
    private boolean mTwoPaneMode;
    //----------------------------------------------------------------------------------------------


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);


//        if (findViewById(R.id.lesson_detail_fragment_container) != null) {
//            mTwoPaneMode = true;
//        }

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

        //-- Select the navigation item ------------------------------------------------------------
        //if (savedInstanceState == null) {
        int itemId = AppSingleton.getInstance().getSelectedDrawerItemId(this);
        if (itemId >= 0) {
            menuItem = navigationView.getMenu().findItem(itemId);
            if (menuItem == null) {
                menuItem = navigationView.getMenu().getItem(0);
            }
            setNavigationFragment(itemId);
        }
        //}
    }

    /**
     * LessonsFragment.LessonClickListener interface
     *
     * @param lessonId
     */
    @Override
    public void onMovieClick(int lessonId) {
        Log.d(LOG_TAG, "onLessonClick called");

//        if (mTwoPaneMode) {
//            // Replace the container fragment
//            LessonDetailFragment fragment = LessonDetailFragment.newInstance(lessonId);
//            getSupportFragmentManager().beginTransaction().replace(
//                    R.id.lesson_detail_fragment_container, fragment).commit();
//        } else {
//            Log.d(LOG_TAG, "Starting LessonDetail Activity");
//            Intent intent = new Intent(this, LessonDetailActivity.class);
//            intent.putExtra(LessonDetailActivity.LESSON_ID, lessonId);
//            this.startActivity(intent);
//        }
    }


    @Override
    public boolean onNavigationItemSelected(MenuItem item) {

        try {
            // Handle navigation view item clicks here.
            int id = item.getItemId();

            switch (id) {
                case R.id.nav_top_rated_movies:
                case R.id.nav_upcoming_movies:
                case R.id.nav_now_playing_movies:
                case R.id.nav_popular_movies:
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
     * Set Navigation fragment as main fragement
     */
    private void setNavigationFragment(int id) {
        switch(id) {
            case R.id.nav_top_rated_movies:
            case R.id.nav_upcoming_movies:
            case R.id.nav_now_playing_movies:
            case R.id.nav_popular_movies:
                //if (mMovieListFragment == null) {
                    mMovieListFragment = new MovieListFragment();
                //}
                mMovieListFragment.setMovieType(id);

                Log.d(LOG_TAG, "nav_lessons: replacing container");
                getSupportFragmentManager().beginTransaction().replace(
                        R.id.drawer_list_fragment_container, mMovieListFragment).commit();

                //Set the selected in app singleton
                AppSingleton.getInstance().setSelectedDrawerItemId(this, id);

            default:
                Log.d(LOG_TAG, "Unknown fragment id " + id);
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

            if (account.getPhotoUrl() != null) {
                ImageView imageView = (ImageView) headerView.findViewById(R.id.imageUser);

                Glide.with(getApplicationContext())
                        .load(account.getPhotoUrl())
                        .error(R.drawable.ic_account_circle_black_24dp)
                        .fitCenter()
                        .into(imageView);
            }

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
