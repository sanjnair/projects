package com.example.xmatrix.popularmovies;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import com.squareup.picasso.Picasso;
import org.json.JSONArray;
import org.json.JSONObject;
import java.util.ArrayList;
import com.example.xmatrix.popularmovies.data.MovieDataSource;

import android.support.v7.widget.ShareActionProvider;
import android.support.v4.view.MenuItemCompat;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MenuInflater;

/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link DetailFragment.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link DetailFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class DetailFragment extends Fragment {
    final String LOG_TAG            = DetailFragment.class.getSimpleName();
    final String SITE_YOUTUBE       = "youtube";
    final String YOUTUBE_NATIVE     = "vnd.youtube:";
    final String YOUTUBE_BROWSER    = "http://www.youtube.com/watch?v=";

    ShareActionProvider mShareActionProvider;
    Bundle m_Items;
    TrailerListAdapter m_TrailerAdapter;
    ReviewListAdapter m_ReviewAdapter;
    MovieDataSource m_dataSource;

    View m_RootView;
    View m_TrailerLayoutView;
    View m_ReviewLayoutView;

    /**
     *
     * @param savedInstanceState
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Bundle args = getArguments();
        Log.d(LOG_TAG, "DetailFragment:onCreate. args = " + args);

        if (args.containsKey(BundleKeys.MOVIE_ID)) {
            Log.d(LOG_TAG, "DetailFragment:onCreate. KEY = " + BundleKeys.MOVIE_ID);
            m_Items = args;
        }
        setHasOptionsMenu(true);
    }

    /**
     *
     * @param inflater
     * @param container
     * @param savedInstanceState
     * @return
     */
    @Override
    public View onCreateView(
            LayoutInflater inflater,
            ViewGroup container,
            Bundle savedInstanceState) {

        Log.d(LOG_TAG, "DetailFragment:onCreateView. m_Items = " + m_Items);

        m_RootView = inflater.inflate(R.layout.fragment_detail, container, false);

        m_TrailerLayoutView = m_RootView.findViewById(R.id.detail_trailer_layout);
        m_TrailerLayoutView.setVisibility(View.GONE);

        m_ReviewLayoutView = m_RootView.findViewById(R.id.detail_review_layout);
        m_ReviewLayoutView.setVisibility(View.GONE);

        if (m_Items != null)
        {
            m_dataSource = new MovieDataSource(m_RootView.getContext());

            //-----------------------------------------------------------
            TextView txtTitle = (TextView)m_RootView.findViewById(R.id.detail_title);
            txtTitle.setText(m_Items.getString(BundleKeys.TITLE));

            //-----------------------------------------------------------
            ImageView imageView = (ImageView)m_RootView.findViewById(R.id.detail_thumbnail);
            String url = m_Items.getString(BundleKeys.THUMBNAIL_URL);
            Picasso.with(
                    m_RootView.getContext())
                    .load(url)
                    .fit()
                    .centerInside()
                    .placeholder(R.drawable.progress)
                    .error(R.drawable.load_error)
                    .into(imageView);

            //-----------------------------------------------------------
            TextView txtReleaseDate = (TextView)m_RootView.findViewById(R.id.detail_release_date);
            txtReleaseDate.setText(m_Items.getString(BundleKeys.RELEASE_DATE));

            //-----------------------------------------------------------
            TextView txtRating = (TextView)m_RootView.findViewById(R.id.detail_rating);
            String text = m_Items.getString(BundleKeys.RATING) + " / 10";
            txtRating.setText(text);

            //-----------------------------------------------------------
            TextView txtOverview = (TextView)m_RootView.findViewById(R.id.detail_overview);
            txtOverview.setText(m_Items.getString(BundleKeys.OVERVIEW));

            //-----------------------------------------------------------
            ListView lstTrailerView = (ListView)m_RootView.findViewById(R.id.trailer_list);
            m_TrailerAdapter = new TrailerListAdapter(m_RootView.getContext());
            lstTrailerView.setAdapter(m_TrailerAdapter);
            lstTrailerView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(
                        AdapterView<?> parent,
                        View view,
                        int position,
                        long id) {

                    Bundle trailerItem = m_TrailerAdapter.GetDataItem(position);
                    if (null != trailerItem) {
                        PlayTrailer(trailerItem);
                    }
                }
            });

            //-----------------------------------------------------------
            ListView lstReviewView = (ListView)m_RootView.findViewById(R.id.review_list);
            m_ReviewAdapter = new ReviewListAdapter(m_RootView.getContext());
            lstReviewView.setAdapter(m_ReviewAdapter);

            //------------------------------------------------------------
            final Button btn_favorite = (Button)m_RootView.findViewById(R.id.btn_favorite);

            btn_favorite.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    updateMovieToDb();
                }
            });

            //------------------------------------------------------------
            verifyMovieInDb();
            updateMovieDetails();
            updateFavoriteButtonStatus();
        }

        return m_RootView;
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.menu_detail, menu);
        super.onCreateOptionsMenu(menu,inflater);

        // Locate MenuItem with ShareActionProvider
        MenuItem item = menu.findItem(R.id.menu_action_share);

        // Fetch and store ShareActionProvider
        mShareActionProvider = (ShareActionProvider) MenuItemCompat.getActionProvider(item);
    }

    // Call to update the share intent
    private void setShareIntent(Intent shareIntent) {
        if (mShareActionProvider != null) {
            mShareActionProvider.setShareIntent(shareIntent);
        }
    }

    /**
     * Creates a Share Intent for Trailers
     * @return
     */
    private Intent createShareIntentForTrailer(String sUrl) {
        Intent shareIntent = new Intent(Intent.ACTION_SEND);
        shareIntent.setType("text/plain");
        shareIntent.putExtra(Intent.EXTRA_SUBJECT, "Movie Trailer");
        shareIntent.putExtra(Intent.EXTRA_TEXT, sUrl);

        return shareIntent;
    }

    /**
     * Verify if movie in DB
     */
    void verifyMovieInDb()
    {
        if (! m_Items.containsKey(BundleKeys.MOVIE_DB_ID))
        {
            try {
                m_dataSource.open();
                long movie_id = m_Items.getLong(BundleKeys.MOVIE_ID);
                Bundle bundle = m_dataSource.getMovie(movie_id);

                if (bundle != null)
                {
                    m_Items.putLong(BundleKeys.MOVIE_DB_ID, bundle.getLong(BundleKeys.MOVIE_ID));
                }
            } catch (Exception ex) {
                Log.e(LOG_TAG, ex.getMessage());
            }
            finally {
                m_dataSource.close();
            }
        }
    }

    /**
     * Returns true if Movie in DB
     */
    boolean isMovieInDb()
    {
        if ((m_Items != null) && m_Items.containsKey(BundleKeys.MOVIE_DB_ID))
        {
            return true;
        }
        return false;
    }


    /**
     * Play Trailer for the given Trailer item
     */
    void PlayTrailer(Bundle trailerBundle)
    {
        String site = trailerBundle.getString(BundleKeys.TRAILER_SITE);
        String key = trailerBundle.getString(BundleKeys.TRAILER_KEY);

        if (! site.equalsIgnoreCase(SITE_YOUTUBE))
        {
            String msg = "Unable to play trailer. Not from " +  SITE_YOUTUBE;
            Toast.makeText(m_RootView.getContext(), msg, Toast.LENGTH_SHORT).show();
        }
        try{
            Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(YOUTUBE_NATIVE + key));
            startActivity(intent);
        }catch (ActivityNotFoundException ex){
            Intent intent= new Intent(Intent.ACTION_VIEW, Uri.parse(YOUTUBE_BROWSER + key));
            startActivity(intent);
        }
    }

    /**
     * Insert/Delete movie information in database
     */
    private void updateMovieToDb()
    {
        if (m_Items == null || (m_dataSource == null))
        {
            return;
        }

        try {
            m_dataSource.open();
            String msg;

            if (isMovieInDb()) {
                long id = m_Items.getLong(BundleKeys.MOVIE_DB_ID);
                m_dataSource.deleteMovie(id);
                m_Items.remove(BundleKeys.MOVIE_DB_ID);
            }
            else {
                long id = m_dataSource.insertMovie(m_Items);
                m_Items.putLong(BundleKeys.MOVIE_DB_ID, id);
            }
            updateFavoriteButtonStatus();

        } catch (Exception ex)
        {
            String msg = "Unable to add movie as favorite. " + ex.getMessage();
            Toast.makeText(m_RootView.getContext(), msg, Toast.LENGTH_SHORT).show();
        }
        finally {
            m_dataSource.close();
        }
    }

    /**
     * Updates favorite button status based on whether the movie in database or not
     */
    private void updateFavoriteButtonStatus()
    {
        final Button btn_favorite = (Button)m_RootView.findViewById(R.id.btn_favorite);

        if (isMovieInDb())
        {
            btn_favorite.setText(getString(R.string.btn_unmark_favorite));
        }
        else
        {
            btn_favorite.setText(getString(R.string.btn_mark_favorite));
        }
    }

    /**
     * Update Movie Details
     *
     */
    private void updateMovieDetails()
    {
        if (isMovieInDb())
        {
            // Update here in case we fetch the trailers and details from db as well
        }
        else {
            FetchTrailerTask trailerTask = new FetchTrailerTask();
            trailerTask.execute(m_Items.getString(BundleKeys.TRAILER_URL));
            FetchReviewTask reviewTask = new FetchReviewTask();
            reviewTask.execute(m_Items.getString(BundleKeys.REVIEW_URL));
        }
    }

    /**
     * Update movie trailer details
     */
    private void updateMovieTrailerDetails(ArrayList<Bundle> result)
    {
        if ((result != null) && !result.isEmpty() && (m_TrailerAdapter != null)) {
            m_TrailerAdapter.setData(result);
            m_TrailerLayoutView.setVisibility(View.VISIBLE);

            // Share the First Trailer if available
            Bundle trailerItem = m_TrailerAdapter.GetDataItem(0);
            String sUrl = trailerItem.getString(BundleKeys.TRAILER_KEY);
            sUrl = YOUTUBE_BROWSER + sUrl;
            Intent intent = createShareIntentForTrailer(sUrl);
            setShareIntent(intent);
        }
        else
        {
            m_TrailerLayoutView.setVisibility(View.GONE);
            // Locate MenuItem with ShareActionProvider
            setShareIntent(new Intent());
        }
    }

    /**
     * Update movie trailer details
     */
    private void updateMovieReviewDetails(ArrayList<Bundle> result) {
        if ((result != null) && !result.isEmpty() && (m_ReviewAdapter != null)) {
            m_ReviewAdapter.setData(result);
            m_ReviewLayoutView.setVisibility(View.VISIBLE);
        }
        else
        {
            m_ReviewLayoutView.setVisibility(View.GONE);
        }
    }


    /**
     * Class for fetching Trailer Information
     */
    //
    // Creating Async task to fetch the movie information
    //
    public class FetchTrailerTask extends AsyncTask<String, Void, ArrayList<Bundle> > {
        /**
         * Override AsyncTask method for doing task in the background
         * @param params  Expects Sort Order parameter from the caller
         * @return
         */
        @Override
        protected ArrayList<Bundle>  doInBackground(String... params) {

            if (params.length == 0) {
                Log.e(LOG_TAG, "Parameter length is 0. Expected > 0");
                return null;
            }

            String sUrl = params[0];
            try
            {
                String jSonResponse = HttpJson.FetchJson(sUrl);

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
         * Parses the JSon Response and returns the Bundle array for the query
         * @param jSonResponse
         */
        private ArrayList<Bundle>  parseJSonResponse(String jSonResponse)
        {
            final String JSON_RESULTS       = "results";
            final String JSON_ID            = "id";
            final String JSON_KEY           = "key";
            final String JSON_NAME          = "name";
            final String JSON_SITE          = "site";
            final String JSON_SIZE          = "size";

            ArrayList<Bundle> bundleList = new ArrayList<Bundle>();

            try {
                JSONObject movieListJson = new JSONObject(jSonResponse);
                JSONArray resultsArray = movieListJson.getJSONArray(JSON_RESULTS);

                for (int i=0; i < resultsArray.length(); ++i) {
                    JSONObject oJason = resultsArray.getJSONObject(i);

                    Bundle bundle = new Bundle();
                    bundle.putString(BundleKeys.TRAILER_ID, oJason.getString(JSON_ID));
                    bundle.putString(BundleKeys.TRAILER_KEY, oJason.getString(JSON_KEY));
                    bundle.putString(BundleKeys.TRAILER_NAME,  oJason.getString(JSON_NAME));
                    bundle.putString(BundleKeys.TRAILER_SITE, oJason.getString(JSON_SITE));
                    bundle.putString(BundleKeys.TRAILER_SIZE, oJason.getString(JSON_SIZE));

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
            updateMovieTrailerDetails(result);
        }
    }

    /**
     * Class for fetching Review Information
     */
    //
    // Creating Async task to fetch the movie information
    //
    public class FetchReviewTask extends AsyncTask<String, Void, ArrayList<Bundle> > {
        /**
         * Override AsyncTask method for doing task in the background
         * @param params  Expects Sort Order parameter from the caller
         * @return
         */
        @Override
        protected ArrayList<Bundle>  doInBackground(String... params) {

            if (params.length == 0) {
                Log.e(LOG_TAG, "Parameter length is 0. Expected > 0");
                return null;
            }

            String sUrl = params[0];
            try
            {
                String jSonResponse = HttpJson.FetchJson(sUrl);

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
         * Parses the JSon Response and returns the Bundle array for the query
         * @param jSonResponse
         */
        private ArrayList<Bundle>  parseJSonResponse(String jSonResponse)
        {
            final String JSON_RESULTS       = "results";
            final String JSON_ID            = "id";
            final String JSON_AUTHOR        = "author";
            final String JSON_CONTENT       = "content";
            final String JSON_URL           = "url";

            ArrayList<Bundle> bundleList = new ArrayList<Bundle>();

            try {
                JSONObject movieListJson = new JSONObject(jSonResponse);
                JSONArray resultsArray = movieListJson.getJSONArray(JSON_RESULTS);

                for (int i=0; i < resultsArray.length(); ++i) {
                    JSONObject oJason = resultsArray.getJSONObject(i);

                    Bundle bundle = new Bundle();
                    bundle.putString(BundleKeys.REVIEW_ID,        oJason.getString(JSON_ID));
                    bundle.putString(BundleKeys.REVIEW_AUTHOR,    oJason.getString(JSON_AUTHOR));
                    bundle.putString(BundleKeys.REVIEW_CONTENT,   oJason.getString(JSON_CONTENT));
                    bundle.putString(BundleKeys.REVIEW_MORE_URL,  oJason.getString(JSON_URL));

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
            updateMovieReviewDetails(result);
        }
    }
}
