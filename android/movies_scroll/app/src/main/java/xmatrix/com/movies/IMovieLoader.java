package xmatrix.com.movies;

/**
 * Created by xmatrix on 11/16/2016.
 */
public interface IMovieLoader {
    public void load(int page, IMovieLoaderListener listener);
}
