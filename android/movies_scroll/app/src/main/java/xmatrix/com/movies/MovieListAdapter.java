package xmatrix.com.movies;

import android.content.Context;
import android.net.Uri;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import java.util.List;

/**
 * Created by xmatrix on 7/2/2016.
 *
 * Adapter for movie list
 */
public class MovieListAdapter
        extends RecyclerView.Adapter<MovieListAdapter.ViewHolder>
        implements IMovieLoaderListener
{

    //----------------------------------------------------------------------------------------------
    // Interface for ViewHolder to inform about Click from ViewHolder
    //----------------------------------------------------------------------------------------------
    public interface MovieListViewHolderListener {
        void onMovieViewClick(int lessonId);
    }

    //----------------------------------------------------------------------------------------------
    // ViewHolder  Inner Class
    //----------------------------------------------------------------------------------------------
    public class ViewHolder
            extends RecyclerView.ViewHolder
            implements View.OnClickListener {

        private final String LOG_TAG =  ViewHolder.class.getSimpleName();

        private final View mLayout;
        private final ImageView mMovieImage;
        private final ProgressBar mProgressbar;

        /**
         * ViewHolder constructor
         */
        public ViewHolder(View itemView) {
            super(itemView);

            mLayout             = itemView.findViewById(R.id.lesson_recycler_item_layout);
            mMovieImage         = (ImageView) itemView.findViewById(R.id.movie_image);
            mProgressbar        = (ProgressBar) itemView.findViewById(R.id.movie_progesss);

            itemView.setOnClickListener(this);
        }

        /**
         * Click listener
         */
        @Override
        public void onClick(View view) {
//            int position = getAdapterPosition();
//            if (position >= 0) {
//                mCursor.moveToPosition(position);
//                int lessonId = mCursor.getInt(mCursor.getColumnIndex(DbContract.LessonEntry._ID));
//                if (mLessonClickListener != null) {
//                    mLessonClickListener.onLessonViewClick(lessonId);
//                }
//            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // LessonListAdapter implementation
    //----------------------------------------------------------------------------------------------
    // Class variables
    private static final String LOG_TAG     = MovieListAdapter.class.getSimpleName();
    private static final int NORMAL_ITEM    = 0;
    private static final int PROGRESS_ITEM  = 1;
    private static final int INVALID_ID     = -1;
    private static final int LOAD_DELAY     = 3000; // 3 seconds

    private MovieList mMovieList;
    private MovieListViewHolderListener mMovieListClickListener;
    private IMovieLoader mMovieLoader;
    private Context mContext;
    private boolean mLoading = false;
    private int mSleepTime = 0;

    /**
     * MovieListAdapter constructor
     */
    public MovieListAdapter(Context context, MovieListViewHolderListener listener) {
        mContext = context;
        mMovieListClickListener = listener;
    }

    /**
     * Sets the Movie Loader object
     */
    void setMovieLoader(IMovieLoader movieLoader) {
        mMovieLoader = movieLoader;
        if (mMovieList != null) {
            mMovieList.clear();
        }
    }

    /**
     * Returns View Type
     * @param position
     * @return
     */
    @Override
    public int getItemViewType(int position) {
        if (mMovieList == null || position >= mMovieList.getMovieList().size()) {
            return PROGRESS_ITEM;
        }
        return NORMAL_ITEM;
    }

    /**
     * Creates ViewHolder
     */
    @Override
    public ViewHolder onCreateViewHolder(
            ViewGroup parent,
            int viewType) {

        int layoutId = R.layout.movie_recycler_item;
        if (viewType == PROGRESS_ITEM) {
            layoutId = R.layout.movie_recyler_progress_item;
        }
        View view = LayoutInflater.from(parent.getContext()).inflate(layoutId, parent, false);
        return new ViewHolder(view);
    }

    /**
     * Adapters provide a binding from an app-specific data set to views that are
     * displayed within a RecyclerView.
     */
    @Override
    public void onBindViewHolder(
            MovieListAdapter.ViewHolder holder,
            int position) {

        int type = getItemViewType(position);
        Context context = holder.mLayout.getContext();

        if (type == NORMAL_ITEM) {
            Movie movie = mMovieList.getMovieList().get(position);
            Glide.with(context)
                    .load(Uri.parse(movie.getPosterPath()))
                    .error(R.drawable.no_image_available)
                    .fitCenter()
                    .into(holder.mMovieImage);
        } else {
            holder.mProgressbar.setIndeterminate(true);
            if (!mLoading) {
                mLoading = true;
                int page = (mMovieList == null) ? 1 : mMovieList.mCurrentPage + 1;
                mMovieLoader.load(page, this);
            }
        }
    }

    /**
     * Load completed callback
     * @param success
     * @param movieList
     */
    public void loadCompleted(boolean success, MovieList movieList) {
        if (!success) {
            Toast.makeText(mContext, "Unable to fetch data", Toast.LENGTH_LONG).show();
        } else {
            if (mMovieList == null) {
                mMovieList = movieList;
                notifyDataSetChanged();
            } else {
                int startIndex = mMovieList.getMovieList().size();
                mMovieList.addMovies(movieList.getMovieList());
                mMovieList.mCurrentPage += 1;
                notifyItemRangeInserted(startIndex, movieList.getMovieList().size());
            }
        }
        mLoading = false;
    }

    /**
     * Returns the item count
     */
    @Override
    public int getItemCount() {
        int count = (mMovieList != null) ? mMovieList.getMovieList().size() : 0;
        count += 1; // for progress bar
        return count;
}

    public void setMovieList(MovieList movieList) {
        mMovieList = movieList;
        // The adapter needs to know that the data has changed. If we don't call this, app will crash.
        notifyDataSetChanged();
    }

    public void addMovies(List<Movie> movies) {
        int startIndex = mMovieList.getMovieList().size();
        mMovieList.addMovies(movies);
        notifyItemRangeInserted(startIndex, movies.size());
    }

    public MovieList getMovieList() {
        return mMovieList;
    }
}
