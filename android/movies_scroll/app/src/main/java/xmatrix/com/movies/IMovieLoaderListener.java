package xmatrix.com.movies;

/**
 * Created by xmatrix on 11/16/2016.
 */
public interface IMovieLoaderListener {
    public void loadCompleted(boolean success, MovieList movieList);
}

