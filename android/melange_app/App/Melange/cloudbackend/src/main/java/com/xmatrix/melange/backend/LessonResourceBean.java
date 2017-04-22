package com.xmatrix.melange.backend;

import java.util.Date;

/**
 * Created by xmatrix on 6/25/2016.
 */
public class LessonResourceBean {
    private String mId          = null;
    private Long mType          = -1L;
    private String mResVal      = null;
    private String mDescription = null;
    private Long mNum           = -1L;
    private Date mUpdateDate    = null;
    //----------------------------------------------------------------------------------------------

    public void setId(String id) { mId = id;}
    public String getId() { return mId; }

    public void setType(Long type) { mType = type;}
    public Long getType() { return mType; }

    public void setResval(String value) { mResVal = value;}
    public String getResval() { return mResVal; }

    public void setDescription(String value) { mDescription = value;}
    public String getDescription() { return mDescription; }

    public void setNum(Long num) { mNum = num;}
    public Long getNum() { return mNum; }

    public void setUpdateDate(Date date) { mUpdateDate = date;}
    public Date getUpdateDate() { return mUpdateDate; }
}
