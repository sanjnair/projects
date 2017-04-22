package xmatrix.com.movies;

/**
 * Created by xmatrix on 8/20/2015.
 */
public class Movie{
    int mId;
    String mPosterPath;
    String mTitle;
    String mOverview;
    boolean mIsAdult;
    String mReleaseDate;
    int mVoteCount;
    double mVoteAverage;

    void setId(int id) {
        mId = id;
    }

    int getId() {
        return mId;
    }

    void setPosterPath(String path) {
        mPosterPath = path;
    }

    String getPosterPath() {
        return mPosterPath;
    }

    void setTitle(String title) {
        mTitle = title;
    }

    String getTitle() {
        return mTitle;
    }

    void setOverview(String overview) {
        mOverview = overview;
    }

    String getOverview() {
        return mOverview;
    }

    void setAdult(boolean flag) {
        mIsAdult = flag;
    }

    boolean isAdult() {
        return mIsAdult;
    }

    void setReleaseDate(String date) {
        mReleaseDate = date;
    }

    String getReleaseDate() {
        return mReleaseDate;
    }

    void setVoteCount(int count) {
        mVoteCount = count;
    }

    int getVoteCount() {
        return mVoteCount;
    }

    void setVoteAverage(double average) {
        mVoteAverage = average;
    }

    double getVoteAverage() {
        return mVoteAverage;
    }
}

