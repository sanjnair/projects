package com.xmatrix.melange.cloud;

/**
 * Backend class for accessing UserLessonEndPoint
 */

import java.io.IOException;
import java.util.Date;
import java.util.List;
import android.content.Context;
import android.util.Log;

import com.google.api.client.extensions.android.http.AndroidHttp;
import com.google.api.client.extensions.android.json.AndroidJsonFactory;
import com.google.api.client.googleapis.services.AbstractGoogleClientRequest;
import com.google.api.client.googleapis.services.GoogleClientRequestInitializer;

import com.xmatrix.melange.backend.melanageApi.MelanageApi;
import com.xmatrix.melange.backend.melanageApi.model.LessonResourceBean;
import com.xmatrix.melange.backend.melanageApi.model.UserLessonBean;
import com.xmatrix.melange.backend.melanageApi.model.LessonBean;
import com.xmatrix.melange.backend.melanageApi.model.RagaBean;

import javax.annotation.Nonnull;

/**
 * Created by xmatrix on 6/21/2016.
 */
public class MelangeBackend {
    private static final String LOG_TAG = MelangeBackend.class.getSimpleName();
    private static final String ENDPOINT_URL = "https://melanage-890aa.appspot.com/_ah/api/";
    public static final Long LESSON_RES_TYPE_AUDIO_MP3          = 1L;
    public static final Long LESSON_RES_TYPE_VIDEO_YOUTUBE      = 10L;

    private MelanageApi mMelanageApi = null;

    public MelangeBackend() {}
    /**
     * Returns the UserLessons - Synchronous call
     */
    public List<UserLessonBean> getUserLessons(
        Date syncDate) {

        List<UserLessonBean> list = null;
        Long syncTime = (syncDate == null) ? 0 : syncDate.getTime();

        try {
            initUserLessonApi();
            list = mMelanageApi.getLessons(syncTime).execute().getItems();
        } catch (Exception ex) {
            Log.e(LOG_TAG, ex.toString());
        }

        return list;
    }

    /**
     * Returns Raga for the ragaId
     */
    public RagaBean getRaga(
        @Nonnull String id) {

        RagaBean ragaBean = null;

        try {
            initUserLessonApi();
            ragaBean = mMelanageApi.getRaga(id).execute();

        } catch (Exception ex) {
            Log.e(LOG_TAG, ex.toString());
        }

        return ragaBean;
    }

    /**
     * Get Lesson for the lessonId
     */
    public LessonBean getLesson(
        @Nonnull String id) {
        LessonBean lessonBean = null;

        try {
            initUserLessonApi();
            lessonBean = mMelanageApi.getLesson(id).execute();

        } catch (Exception ex) {
            Log.e(LOG_TAG, ex.toString());
        }

        return lessonBean;
    }

    public List<LessonResourceBean> getLessonResources(
            @Nonnull String lessonId) {

        List<LessonResourceBean> list = null;

        try {
            initUserLessonApi();
            list = mMelanageApi.getLessonResources(lessonId).execute().getItems();
        } catch (Exception ex) {
            Log.e(LOG_TAG, ex.toString());
        }

        return list;
    }

    /**
     * Initialize User Lesson API
     */
    private void initUserLessonApi() {
        if (mMelanageApi == null) {
            MelanageApi.Builder builder = new MelanageApi.Builder(AndroidHttp.newCompatibleTransport(),
                    new AndroidJsonFactory(), null)
                    // options for running against local devappserver
                    // - 10.0.2.2 is localhost's IP address in Android emulator
                    // - turn off compression when running against local devappserver
                    .setRootUrl(ENDPOINT_URL)
                    .setGoogleClientRequestInitializer(new GoogleClientRequestInitializer() {
                        @Override
                        public void initialize(AbstractGoogleClientRequest<?> abstractGoogleClientRequest) throws IOException {
                            abstractGoogleClientRequest.setDisableGZipContent(true);
                        }
                    });

            mMelanageApi = builder.build();
        }
    }
}
