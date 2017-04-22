package com.xmatrix.melange.backend;
import java.util.Date;

/**
 * Created by xmatrix on 6/25/2016.
 */
public class RagaBean {
    private String mId          = null;
    private String mName        = null;
    private String mArohana     = null;
    private String mAvarohana   = null;
    private String mMelakartha  = null;
    private String mPicUrl      = null;
    private Date mUpdateDate    = null;

    //----------------------------------------------------------------------------------------------

    public void setId(String id) { mId = id; }
    public String getId() { return mId; }

    public void setName(String name) { mName = name;}
    public String getName() { return mName; }

    public void setArohana(String arohana) { mArohana = arohana;}
    public String getArohana() { return mArohana; }

    public void setAvarohana(String avarohana) { mAvarohana = avarohana;}
    public String getAvarohana() { return mAvarohana; }

    public void setMelakartha(String melakartha) { mMelakartha = melakartha;}
    public String getMelakartha() { return mMelakartha; }

    public void setPicture(String picUrl) { mPicUrl = picUrl;}
    public String getPicture() { return mPicUrl; }

    public void setUpdateDate(Date date) { mUpdateDate = date;}
    public Date getUpdateDate() { return mUpdateDate; }
}
