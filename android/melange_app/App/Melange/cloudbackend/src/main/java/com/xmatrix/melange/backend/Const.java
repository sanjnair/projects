package com.xmatrix.melange.backend;

/**
 * Created by xmatrix on 6/20/2016.
 *
 * Constant values
 */
public final class Const {

    public static final String ID_WEB_CLIENT                    = "34053092298-i6aslium3k91t4b1bscrpejcb6e4s4co.apps.googleusercontent.com";
    public static final String ID_ANDROID_CLIENT                = "34053092298-gnah0rd6c0m4b0nm4rh247c8gl6925ll.apps.googleusercontent.com";
    public static final String ID_ANDROID_CLIENT_RELEASE        = "34053092298-81nvkd056tgldnhue77t4imk36qsk5io.apps.googleusercontent.com";
    public static final String ID_ANDROID_CLIENT_RLS_INTERNAL   = "34053092298-ihisnrsm15g5al6t7kjnlvmjqoidhjh8.apps.googleusercontent.com";
    public static final String AUDIENCE_ANDROID                 = ID_WEB_CLIENT;
    public static final String EMAIL_SCOPE_URL                  = "https://www.googleapis.com/auth/userinfo.email";

    // Datastore constants--------------------------------------------------------------------------
    public static final String DS_NAMESPACE                     = "melange_ds";

    // Entities
    public static final String DS_KIND_USER                     = "user";
    public static final String DS_KIND_RAGA                     = "raga";
    public static final String DS_KIND_RES_TYPE                 = "resource_type";
    public static final String DS_KIND_LESSON                   = "lesson";
    public static final String DS_KIND_LESSON_RES               = "lesson_resource";
    public static final String DS_KIND_USER_LESSON              = "user_lesson";

    // Properties for DS_KIND_USER
    public static final String DS_EN_USER_P_NAME                = "name";
    public static final String DS_EN_USER_P_EMAIL               = "email";

    // Properties for DS_KIND_RAGA
    public static final String  DS_EN_RAGA_P_UID                = "uid";
    public static final String  DS_EN_RAGA_P_NAME               = "name";
    public static final String  DS_EN_RAGA_P_AROHANA            = "arohana";
    public static final String  DS_EN_RAGA_P_AVAROHANA          = "avarohana";
    public static final String  DS_EN_RAGA_P_MELAKARTHA         = "melakartha";
    public static final String  DS_EN_RAGA_P_PIC                = "picture";
    public static final String  DS_EN_RAGA_P_UPDATE_DATE        = "update_date";

    // Properties for DS_KIND_RES_TYPE
    public static final String  DS_EN_RES_TYPE_P_TYPE           = "type";
    public static final String  DS_EN_RES_TYPE_P_DESC           = "description";

    // Properties for DS_EN_LESSON
    public static final String  DS_EN_LESSON_P_UID              = "uid";
    public static final String  DS_EN_LESSON_P_AUTHOR           = "author";
    public static final String  DS_EN_LESSON_P_LESSON           = "lesson";
    public static final String  DS_EN_LESSON_P_NAME             = "name";
    public static final String  DS_EN_LESSON_P_RAGA_ID          = "id_raga";
    public static final String  DS_EN_LESSON_P_THALA            = "thala";
    public static final String  DS_EN_LESSON_P_LESSON_PIC       = "picture";
    public static final String  DS_EN_LESSON_P_UPDATE_DATE      = "update_date";

    // Properties for DS_KIND_LESSON_RES
    public static final String DS_EN_LESSON_RES_P_UID           = "uid";
    public static final String DS_EN_LESSON_RES_P_TYPE_ID       = "id_type";
    public static final String DS_EN_LESSON_RES_P_RESVAL        = "resval";
    public static final String DS_EN_LESSON_RES_P_DESC          = "description";
    public static final String DS_EN_LESSON_RES_P_NUM           = "num";
    public static final String DS_EN_LESSON_RES_P_UPDATE_DATE   = "update_date";

    // Properties for DS_EN_USER_LESSON
    public static final String  DS_EN_USER_LESSON_P_END_DATE    = "end_date";
    public static final String  DS_EN_USER_LESSON_P_LESSON_ID   = "id_lesson";
    public static final String  DS_EN_USER_LESSON_P_LESSON_NUM  = "lesson_number";
    public static final String  DS_EN_USER_LESSON_P_START_DATE  = "start_date";
    public static final String  DS_EN_USER_LESSON_P_UPDATE_DATE = "update_date";
    public static final String  DS_EN_USER_LESSON_P_USER_ID     = "id_user";

    //--API Related Constants-----------------------------------------------------------------------
    public static final String API_OWNER_DOMAIN                 = "backend.melange.xmatrix.com";
    public static final String API_OWNER_NAME                   = "backend.melange.xmatrix.com";

    //--Value Constants-----------------------------------------------------------------------------
    public static final int LESSON_RES_TYPE_AUDIO_MP3          = 1;
    public static final int LESSON_RES_TYPE_VIDEO_YOUTUBE      = 10;
}
