package xmatrix.com.movies;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.IntegerRes;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * A simple {@link Fragment} subclass.
 */
public class MovieListFragment extends Fragment
        implements MovieListAdapter.MovieListViewHolderListener
{
    /**
     * Interface for Notifying about Lesson Click
     */
    public interface MovieClickListener {
        void onMovieClick(int lessonId);
    }

    private static final String LOG_TAG         = MovieListFragment.class.getSimpleName();
    private static final String API_KEY         = "2cdf3a5c7cf412421485f89ace91e373";
    private int mMovieType = -1;
    private MovieListAdapter mListAdapter;
    private TextView mTxtNotification;
    private WebMovieLoader mWebMovieLoader;

    private static final Map<Integer, WebMovieLoader.Category> MOVIE_CATEGORY_MAP = new HashMap<Integer, WebMovieLoader.Category>() {
        {
            put(R.id.nav_top_rated_movies, WebMovieLoader.Category.TopRated);
            put(R.id.nav_popular_movies, WebMovieLoader.Category.Popular);
            put(R.id.nav_upcoming_movies, WebMovieLoader.Category.NowPlaying);
            put(R.id.nav_now_playing_movies, WebMovieLoader.Category.Upcoming);
        }
    };

    /**
     * Constructs Lesson Fragment
     */
    public MovieListFragment() {
        // required
    }

    @Override
    public View onCreateView(
            LayoutInflater inflater,
            ViewGroup container,
            Bundle savedInstanceState) {

        Log.d(LOG_TAG, "MoviesLessonsFragment: onCreateView");

        View fragmentView = inflater.inflate(R.layout.fragment_movies, container, false);
        RecyclerView movieView = (RecyclerView) fragmentView.findViewById(R.id.movie_recycler_view);
        movieView.addItemDecoration(new MarginDecoration(movieView.getContext()));
        movieView.setHasFixedSize(true);

        mTxtNotification = (TextView)fragmentView.findViewById(R.id.movie_notificationText);
        mListAdapter = new MovieListAdapter(getContext(), this);
        movieView.setAdapter(mListAdapter);

        // sets up movie loader
        setupMovieLoader();

        // Inflate the layout for this fragment
        return fragmentView;
    }

    /**
     * Sets the movie type
     * @param type
     */
    public void setMovieType(int type) {
        mMovieType = type;
        setupMovieLoader();
    }


    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
    }

    @Override
    public void onMovieViewClick(int lessonId) {
        Log.d(LOG_TAG, "onmovieViewClick called");

        MovieClickListener listener = (MovieClickListener) getActivity();
        listener.onMovieClick(lessonId);
    }

    /**
     * Set the notification Text
     */
    private void setNotificationTextView(int count) {
        if (count > 0) {
            mTxtNotification.setVisibility(View.GONE);
        } else {
            mTxtNotification.setVisibility(View.VISIBLE);

            // Set the message if network is not available
            Context context = getContext();

            String msg;

            if (!Utility.isNetworkAvailable(context)) {
                msg = context.getString(R.string.no_movies_no_network);
            } else {
                msg = context.getString(R.string.no_movies_syncing);
            }
            mTxtNotification.setText(msg);
        }
    }

    /**
     * Sets up movie loader
     */
    private void setupMovieLoader() {
        Log.d(LOG_TAG, "setupMovieLoader");
        if (mListAdapter != null) {
            switch (mMovieType) {
                case R.id.nav_top_rated_movies:
                case R.id.nav_upcoming_movies:
                case R.id.nav_now_playing_movies:
                case R.id.nav_popular_movies:

                    if (null == mWebMovieLoader) {
                        mWebMovieLoader = new WebMovieLoader(getContext(), API_KEY);
                    }

                    WebMovieLoader.Category category = MOVIE_CATEGORY_MAP.get(mMovieType);
                    mWebMovieLoader.setCategory(category);
                    mListAdapter.setMovieLoader(mWebMovieLoader);
                    Log.d(LOG_TAG, "setupMovieLoader called on adapter");
                    break;

                case R.id.nav_favorite_movies:
                    break;
            }
        }
    }

    /**
     * Load movies from Network
     */
    void loadMoviesFromNetwork(int page) {
        switch(mMovieType) {
            case R.id.nav_top_rated_movies:
                mWebMovieLoader.setCategory(WebMovieLoader.Category.TopRated);
                break;
            case R.id.nav_upcoming_movies:
                mWebMovieLoader.setCategory(WebMovieLoader.Category.Upcoming);
                break;
            case R.id.nav_now_playing_movies:
                mWebMovieLoader.setCategory(WebMovieLoader.Category.NowPlaying);
                break;
            case R.id.nav_popular_movies:
                mWebMovieLoader.setCategory(WebMovieLoader.Category.Popular);
                break;
        }

        mWebMovieLoader.load(
        page,
        new IMovieLoaderListener() {
            public void loadCompleted(boolean success, MovieList movieList) {
                if (success) {
                    if (mListAdapter.getMovieList() == null) {
                        mListAdapter.setMovieList(movieList);
                    } else {
                        mListAdapter.addMovies(movieList.getMovieList());
                    }
                } else {
                    Log.e(LOG_TAG, "Unable to load movie from network");
                }
            }
        });
    }
}
