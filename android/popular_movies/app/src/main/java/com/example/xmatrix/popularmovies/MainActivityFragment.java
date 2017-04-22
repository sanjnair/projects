package com.example.xmatrix.popularmovies;

import android.content.SharedPreferences;
import android.net.Uri;
import android.os.AsyncTask;
import android.preference.PreferenceManager;
import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.GridView;
import android.widget.Toast;

import org.json.JSONArray;
import org.json.JSONObject;
import java.sql.SQLException;
import java.util.ArrayList;
import com.example.xmatrix.popularmovies.data.MovieDataSource;

/**
 * A placeholder fragment containing a simple view.
 */
public class MainActivityFragment extends Fragment {

    // !! PLEASE PROVIDE YOUR API KEY HERE !!
    private final String API_KEY    ="2cdf3a5c7cf412421485f89ace91e373";                            // ADD YOUR API KEY HERE

    private final String LOG_TAG = MainActivityFragment.class.getSimpleName();
    private final String KEY_SAVED_INSTANCE = "key_savedinstance";
    /**
     * Whether or not the activity is in two-pane mode, i.e. running on a tablet
     * device.
     */
    private static String m_SortOrder       = null;
    private boolean m_TwoPane               = false;
    private ImageAdapter m_ImgAdapter       = null;
    private View m_RootView                 = null;
    private MovieDataSource m_dataSource    = null;

    /**
     * Empty constructor
     */
    public MainActivityFragment() {
    }

//    @Override
    public View onCreateView(
        LayoutInflater inflater,
        ViewGroup container,
        Bundle savedInstanceState) {

        Log.d(LOG_TAG, "MainActivityFragment:onCreateView called");

        super.onCreateView(inflater, container, savedInstanceState);

        m_RootView = inflater.inflate(R.layout.fragment_main, container, false);
        m_dataSource = new MovieDataSource(m_RootView.getContext());

        m_ImgAdapter = new ImageAdapter(m_RootView.getContext());
        GridView gridview = (GridView)m_RootView.findViewById((R.id.main_grid_view));
        gridview.setAdapter(m_ImgAdapter);

        if (m_RootView.findViewById(R.id.movie_detail_container) != null) {
            m_TwoPane = true;
        }
        else {
            m_TwoPane = false;
        }

        Log.d(LOG_TAG, "m_TwoPane is " + m_TwoPane);

        gridview.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int position, long l) {
                Bundle bundle = (Bundle) m_ImgAdapter.getItem(position);

                GridImageSelectedListener listener = (GridImageSelectedListener)getActivity();
                listener.OnSelectionChanged(bundle);
            }
        });

        if((savedInstanceState != null) && savedInstanceState.containsKey(KEY_SAVED_INSTANCE)) {
            ArrayList<Bundle> list = savedInstanceState.getParcelableArrayList(KEY_SAVED_INSTANCE);
            setMovieList(list);
        }

        if (API_KEY.length() <= 0)
        {
            Toast.makeText(m_RootView.getContext(), "Variable API_KEY not set", Toast.LENGTH_SHORT).show();
            setMovieList(null);
        }

        return m_RootView;
    }

    @Override
    public void onStart() {
        super.onStart();
        updateMovies();
    }

    /**
     * Saves the state
     * @param outState
     */
    @Override
    public void onSaveInstanceState(Bundle outState) {
        ArrayList<Bundle> list = m_ImgAdapter.getBundleList();

        if (list != null)
        {
            outState.putParcelableArrayList(KEY_SAVED_INSTANCE, list);
            super.onSaveInstanceState(outState);
        }
    }

    /**
     * Update Movies Asynchronously
     */
    private void updateMovies() {

        if (API_KEY.length() <= 0)
        {
            return;
        }

        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getActivity());
        String sort_order = prefs.getString(
                getString(R.string.pref_sort_order_key),
                getString(R.string.pref_sort_order_default));

        if (m_TwoPane) {
            if ((m_SortOrder == null) || (!sort_order.equals(m_SortOrder))) {
                GridImageSelectedListener listener = (GridImageSelectedListener)getActivity();
                listener.onPreferenceChange();
            }
        }

        if ((m_ImgAdapter.getCount() == 0) ||
                (m_SortOrder == null) ||
                !sort_order.equals(m_SortOrder)) {

            String sFavorites = getString(R.string.pref_sort_order_favorites_val);

            if (sort_order.equals(sFavorites)) {
                fetchMovieFromDb();

            } else {
                if (HttpJson.isNetworkAvailable(getActivity()))
                {
                    FetchMovieTask movieTask = new FetchMovieTask();
                    movieTask.execute(sort_order);
                }
                else
                {
                    // Clear the movie list
                    setMovieList(null);
                }
            }
            m_SortOrder = sort_order;
        }
    }

    //
    // Sets movie list in adapter and invalidates the view
    //
    void setMovieList(ArrayList<Bundle> movieList)
    {
        View noMovieLayout = m_RootView.findViewById(R.id.no_movies_layout);

        m_ImgAdapter.clear();
        if (movieList != null) {
            m_ImgAdapter.setBundleList(movieList);
        }

        GridView gridview = (GridView)m_RootView.findViewById((R.id.main_grid_view));
        gridview.invalidateViews();

        if (m_ImgAdapter.getCount() == 0) {
            noMovieLayout.setVisibility(View.VISIBLE);
        } else {
            noMovieLayout.setVisibility(View.GONE);
        }
    }

    //
    // Update the movie list from database
    //
    void fetchMovieFromDb()
    {
        try
        {
            m_dataSource.open();
            ArrayList<Bundle> movies = m_dataSource.getAllMovies();
            setMovieList(movies);

        } catch(SQLException ex) {
            String msg = "Unable to get movies from db. " + ex.getMessage();
            Toast.makeText(m_RootView.getContext(), msg, Toast.LENGTH_SHORT).show();
        }
        finally {
            m_dataSource.close();
        }
    }

    //
    // Creating Async task to fetch the movie information
    //
    public class FetchMovieTask extends AsyncTask<String, Void, ArrayList<Bundle> > {

        private final String LOG_TAG    = FetchMovieTask.class.getSimpleName();
        final String BASE_URL           = "https://api.themoviedb.org/3/movie";                     // Base URL of Movie DB
        final String API_PARAM          = "api_key";

        /**
         * Override AsyncTask method for doing task in the background
         * @param params  Expects Sort Order parameter from the caller
         * @return
         */
        @Override
        protected ArrayList<Bundle>  doInBackground(String... params) {

            final String SEARCH_CATEGORY_POPULAR    = "popular";
            final String SEARCH_CATEGORY_TOP_RATED  = "top_rated";

            // If there's no zip code, there's nothing to look up.  Verify size of params.
            if (params.length == 0) {
                Log.e(LOG_TAG, "Parameter length is 0. Expected > 0");
                return null;
            }

            String sort_order = params[0];
            String category = null;

            if (sort_order.equals(getString(R.string.pref_sort_order_most_popular_val))) {
                category = SEARCH_CATEGORY_POPULAR;
            } else if (sort_order.equals(getString(R.string.pref_sort_order_highest_rated_val))) {
                category = SEARCH_CATEGORY_TOP_RATED;
            } else {
                Log.e(LOG_TAG, "Unknown category " + sort_order);
                return null;
            }

            try
            {
                String jSonResponse = queryMovieDB(category);
                if (jSonResponse == null){
                    return null;
                }
                return parseJSonResponse(jSonResponse);

            } catch (Exception e)
            {
                Log.e(LOG_TAG, e.getStackTrace().toString());
            }

            return null;
        }

        /**
         * Return the Result (JSon) of the query from the Movie DB API
         * @param category      Category to query
         */
        private String queryMovieDB(String category)
        {
            try {
                Uri builtUri = Uri.parse(BASE_URL).buildUpon()
                        .appendPath(category)
                        .appendQueryParameter(API_PARAM, API_KEY)
                        .build();

                return HttpJson.FetchJson(builtUri.toString());

            } catch (Exception e) {
                Log.e(LOG_TAG, "Error ", e);
            }
            return null;
        }

        /**
         * Parses the JSon Response and returns the Bundle array for the query
         * @param jSonResponse
         */
        private ArrayList<Bundle>  parseJSonResponse(String jSonResponse)
        {
            final String JSON_RESULTS       = "results";
            final String JSON_ID            = "id";
            final String JSON_TITLE         = "original_title";
            final String JSON_OVERVIEW      = "overview";
            final String JSON_IMAGE_PATH    = "poster_path";
            final String JSON_RATING        = "vote_average";
            final String JSON_RELEASE_DATE  = "release_date";
            final String IMAGE_BASE_URL     = "http://image.tmdb.org/t/p";                          // Image Base URL
            final String IMAGE_SIZE         = "/w185";
            final String THUMBNAIL_SIZE     = "/w92";
            final String REVIEWS_SUB_PATH   = "reviews";
            final String TRAILERS_SUB_PATH  = "videos";

            ArrayList<Bundle> bundleList = new ArrayList<Bundle>();

            try {
                JSONObject movieListJson = new JSONObject(jSonResponse);
                JSONArray resultsArray = movieListJson.getJSONArray(JSON_RESULTS);

                for (int i=0; i < resultsArray.length(); ++i) {
                    JSONObject movieJson = resultsArray.getJSONObject(i);

                    String id           = movieJson.getString(JSON_ID);
                    String title        = movieJson.getString(JSON_TITLE);
                    String overview     = movieJson.getString(JSON_OVERVIEW);
                    String image_path   = movieJson.getString(JSON_IMAGE_PATH);
                    String rating       = movieJson.getString(JSON_RATING);
                    String release_date = movieJson.getString(JSON_RELEASE_DATE);
                    String image_url    = IMAGE_BASE_URL + IMAGE_SIZE + image_path;
                    String thumb_url    = IMAGE_BASE_URL + THUMBNAIL_SIZE + image_path;

                    Uri review_uri = Uri.parse(BASE_URL).buildUpon()
                                        .appendPath(id)
                                        .appendPath(REVIEWS_SUB_PATH)
                                        .appendQueryParameter(API_PARAM, API_KEY)
                                        .build();

                    Uri trailer_uri = Uri.parse(BASE_URL).buildUpon()
                                        .appendPath(id)
                                        .appendPath(TRAILERS_SUB_PATH)
                                        .appendQueryParameter(API_PARAM, API_KEY)
                                        .build();

                    Bundle bundle = new Bundle();

                    bundle.putLong(BundleKeys.MOVIE_ID, Long.parseLong(id));
                    bundle.putString(BundleKeys.TITLE, title);
                    bundle.putString(BundleKeys.IMAGE_URL, image_url);
                    bundle.putString(BundleKeys.THUMBNAIL_URL, thumb_url);
                    bundle.putString(BundleKeys.OVERVIEW, overview);
                    bundle.putString(BundleKeys.RATING, rating);
                    bundle.putString(BundleKeys.RELEASE_DATE, release_date);
                    bundle.putString(BundleKeys.REVIEW_URL, review_uri.toString());
                    bundle.putString(BundleKeys.TRAILER_URL, trailer_uri.toString());

                    bundleList.add(bundle);
                }

            } catch (Exception e) {
                Log.e(LOG_TAG, e.getMessage(), e);
                e.printStackTrace();

                return null;
            }

            return bundleList;
        }

        @Override
        protected void onPostExecute(ArrayList<Bundle> result) {
            Log.d(LOG_TAG, "onPostExecute:Result is " + result);
            setMovieList(result);
        }
    }
}

