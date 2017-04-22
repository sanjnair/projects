package xmatrix.com.movies;

import android.content.Context;
import android.net.Uri;
import android.util.Log;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by xmatrix on 11/16/2016.
 */

public class WebMovieLoader implements IMovieLoader {

    public enum Category
    {
        TopRated,
        Popular,
        NowPlaying,
        Upcoming
    }

    private static final String BASE_URL            = "https://api.themoviedb.org/3/movie/";
    private static final String IMAGE_URL           = "https://image.tmdb.org/t/p/w500";
    private static final String REQ_API_KEY         = "api_key";
    private static final String REQ_PAGE            = "page";

    private static final String RES_PAGE            = "page";
    private static final String RES_TOTAL_RESULTS   = "total_results";
    private static final String RES_TOTAL_PAGES     = "total_pages";
    private static final String RES_RESULTS         = "results";
    private static final String RES_ID              = "id";
    private static final String RES_POSTER_PATH     = "poster_path";
    private static final String RES_ADULT           = "adult";
    private static final String RES_OVERVIEW        = "overview";
    private static final String RES_RELEASE_DATE    = "release_date";
    private static final String RES_TITLE           = "title";
    private static final String RES_VOTE_COUNT      = "vote_count";
    private static final String RES_VOTE_AVERAGE    = "vote_average";

    private static final Map<Category, String> CATEGORY_MAP = new HashMap<Category, String>() {
        {
            put(Category.TopRated, "top_rated");
            put(Category.Popular, "popular");
            put(Category.NowPlaying, "now_playing");
            put(Category.Upcoming, "upcoming");
        }
    };

    private static final String LOG_TAG = WebMovieLoader.class.getName();

    //----------------------------------------------------------------------------------------------
    private Category mCategory     = Category.TopRated;
    private String mMovidDbUrl;
    private String mApiKey;
    private Context mContext;
    private RequestQueue mRequestQueue;

    public WebMovieLoader(Context context, String apiKey)
    {
        mApiKey = apiKey;
        mContext = context;
        mRequestQueue = Volley.newRequestQueue(mContext);
    }


    public void setCategory(Category category) {
        mCategory = category;
    }

    /**
     * Loads the data from web
     * @param page
     */
    public void load(int page, final IMovieLoaderListener listener)
    {
        Uri.Builder builder = Uri.parse(BASE_URL)
                .buildUpon()
                .appendEncodedPath(CATEGORY_MAP.get(mCategory))
                .appendQueryParameter(REQ_API_KEY, mApiKey)
                .appendQueryParameter(REQ_PAGE, String.valueOf(page));

        String sUrl = builder.build().toString();
        Log.d(LOG_TAG, sUrl);

        // The third parameter Listener overrides the method onResponse() and passes
        //JSONObject as a parameter
        JsonObjectRequest obreq = new JsonObjectRequest(
            Request.Method.GET,
            sUrl,
            null,
            new Response.Listener<JSONObject>() {
                // Takes the response from the JSON request
                @Override
                public void onResponse(JSONObject response) {
                    MovieList movieList = new MovieList();

                    try {
                        movieList.setCurrentPage(response.getInt(RES_PAGE));
                        movieList.setTotalPages(response.getInt(RES_TOTAL_PAGES));
                        movieList.setTotalResults(response.getInt(RES_TOTAL_RESULTS));

                        JSONArray jsonArray = response.getJSONArray(RES_RESULTS);

                        for (int i = 0; i < jsonArray.length(); i++) {
                            JSONObject jsonObject = jsonArray.getJSONObject(i);
                            Movie movie = new Movie();

                            String posterUrl = IMAGE_URL + jsonObject.getString(RES_POSTER_PATH);
                            String releaseDate = jsonObject.getString(RES_RELEASE_DATE);

                            movie.setId(jsonObject.getInt(RES_ID));
                            movie.setPosterPath(posterUrl);
                            movie.setAdult(jsonObject.getBoolean(RES_ADULT));
                            movie.setOverview(jsonObject.getString(RES_OVERVIEW));
                            movie.setReleaseDate(releaseDate);
                            movie.setTitle(jsonObject.getString(RES_TITLE));
                            movie.setVoteCount(jsonObject.getInt(RES_VOTE_COUNT));
                            movie.setVoteAverage(jsonObject.getDouble(RES_VOTE_AVERAGE));

                            movieList.addMovie(movie);
                        }

                        listener.loadCompleted(true, movieList);
                    }
                    // Try and catch are included to handle any errors due to JSON
                    catch (JSONException e) {
                        Log.e(LOG_TAG, e.toString());
                        listener.loadCompleted(false, null);
                    }
                }
            },
            // The final parameter overrides the method onErrorResponse() and passes VolleyError
            //as a parameter
            new Response.ErrorListener() {
                @Override
                // Handles errors that occur due to Volley
                public void onErrorResponse(VolleyError error) {
                    Log.e(LOG_TAG, error.getMessage());
                    listener.loadCompleted(false, null);
                }
            }
        );
        // Adds the JSON object request "obreq" to the request queue
        mRequestQueue.add(obreq);
    }
}
