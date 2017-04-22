package xmatrix.com.movies;

import java.util.ArrayList;
import java.util.List;

class MovieList {
    int mTotalPages;
    int mTotalResults;
    int mCurrentPage;
    ArrayList<Movie> mMovieList;

    MovieList() {
        mMovieList = new ArrayList<Movie>();
    }

    void setTotalPages(int pages) {
        mTotalPages = pages;
    }

    int getTotalPages() {
        return mTotalPages;
    }

    void setTotalResults(int results) {
        mTotalResults = results;
    }

    int getTotalResults() {
        return mTotalResults;
    }

    void setCurrentPage(int page) {
        mCurrentPage = page;
    }

    void addMovie(Movie movie) {
        mMovieList.add(movie);
    }

    List<Movie> getMovieList() {
        return mMovieList;
    }

    void addMovies(List<Movie> list) {
        mMovieList.addAll(list);
    }

    void clear() {
        mTotalPages = 0;
        mTotalResults = 0;
        mCurrentPage = 0;
        mMovieList.clear();
    }
}
