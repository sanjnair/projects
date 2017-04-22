package com.xmatrix.melange.backend;
import java.util.Date;

/**
 * Created by xmatrix on 6/19/2016.
 *
 * Data for User lessons
 *
 */
public class UserLessonBean {
    private String mLessonId        = null;
    private Long mLessonNumber      = -1L;
    private Date mStartDate         = null;
    private Date mEndDate           = null;
    private Date mUpdateDate        = null;
    //----------------------------------------------------------------------------------------------

    public String getLessonId() {
        return mLessonId;
    }
    public void setLessonId(String id) {
        mLessonId = id;
    }

    public Long getLessonNumber() {
        return mLessonNumber;
    }
    public void setLessonNumber(Long number) {mLessonNumber = number; }

    public Date getStartDate() { return mStartDate; }
    public void setStartDate(Date date) {
        mStartDate = date;
    }

    public Date getEndDate() {
        return mEndDate;
    }
    public void setEndDate(Date date) {
        mEndDate = date;
    }

    public Date getUpdateDate() {
        return mUpdateDate;
    }
    public void setUpdateDate(Date date) {
        mUpdateDate = date;
    }
}
