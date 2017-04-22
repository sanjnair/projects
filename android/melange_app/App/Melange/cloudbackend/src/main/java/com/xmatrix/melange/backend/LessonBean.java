package com.xmatrix.melange.backend;
import java.util.Date;

/**
 * Created by xmatrix on 6/25/2016.
 *
 * Bean for lessons
 */
public class LessonBean {
    private String mId              = null;
    private String mRagaId          = null;
    private String mAuthor          = null;
    private String mName            = null;
    private String mLessonUrl       = null;
    private String mLessonPicUrl    = null;
    private String mThala           = null;
    private Date mUpdateDate        = null;
    //----------------------------------------------------------------------------------------------

    public void setId(String id) { mId = id; }
    public String getId() { return mId; }

    public void setRagaId(String id) { mRagaId = id;}
    public String getRagaId() { return mRagaId; }

    public void setAuthor(String author) { mAuthor = author;}
    public String getAuthor() { return mAuthor; }

    public void setName(String name) { mName = name;}
    public String getName() { return mName; }

    public void setLessonUrl(String url) { mLessonUrl = url;}
    public String getLessonUrl() { return mLessonUrl; }

    public void setLessonPicUrl(String url) { mLessonPicUrl = url;}
    public String getLessonPicUrl() { return mLessonPicUrl; }

    public void setThala(String thala) { mThala = thala;}
    public String getThala() { return mThala; }

    public void setUpdateDate(Date date) { mUpdateDate = date;}
    public Date getUpdateDate() { return mUpdateDate; }
}
