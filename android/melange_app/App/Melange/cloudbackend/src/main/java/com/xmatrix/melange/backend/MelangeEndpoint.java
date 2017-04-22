package com.xmatrix.melange.backend;

import com.google.api.server.spi.config.Api;
import com.google.api.server.spi.config.ApiMethod;
import com.google.api.server.spi.config.ApiNamespace;
import com.google.api.server.spi.config.Named;
import com.google.api.server.spi.response.CollectionResponse;
import com.google.appengine.api.NamespaceManager;
import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.appengine.api.datastore.PreparedQuery;
import com.google.appengine.api.datastore.Query;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Created by xmatrix on 6/25/2016.
 *
 * Endpoint for Melange backend
 */

@Api(
        name = "melanageApi",
        version = "v1",
        scopes = {Const.EMAIL_SCOPE_URL},
        clientIds = {Const.ID_WEB_CLIENT, Const.ID_ANDROID_CLIENT, Const.ID_ANDROID_CLIENT_RELEASE, Const.ID_ANDROID_CLIENT_RLS_INTERNAL},
        audiences = {Const.AUDIENCE_ANDROID},
        namespace = @ApiNamespace(
                ownerDomain = Const.API_OWNER_DOMAIN,
                ownerName = Const.API_OWNER_NAME,
                packagePath = ""
        )
)

public class MelangeEndpoint {
    private static final Logger mLogger = Logger.getLogger(MelangeEndpoint.class.getName());

    //----------------------------------------------------------------------------------------------
    // User Lessons API
    //----------------------------------------------------------------------------------------------

    /**
     * API method for getting all lessons from datastore
     */
    @ApiMethod(name = "getLessons")
    public CollectionResponse<UserLessonBean> getLessons(
            @Named("syncDate") Long syncDate) {

        ArrayList<UserLessonBean> lessons = new ArrayList<>();

        try {
            NamespaceManager.set(Const.DS_NAMESPACE);
            DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();

            // get items from the data store after the last sync date
            Query query = new Query(Const.DS_KIND_USER_LESSON);
            query.addSort(Const.DS_EN_USER_LESSON_P_LESSON_NUM, Query.SortDirection.ASCENDING);
            PreparedQuery preparedQuery = dsService.prepare(query);

            for (Entity entity : preparedQuery.asIterable()) {
                if (entity != null) {
                    UserLessonBean userLessonBean = new UserLessonBean();
                    Key lessonKey = (Key) entity.getProperty(Const.DS_EN_USER_LESSON_P_LESSON_ID);
                    String lessonId = getLessonId(lessonKey);

                    userLessonBean.setLessonId(lessonId);
                    userLessonBean.setLessonNumber((Long) entity.getProperty(Const.DS_EN_USER_LESSON_P_LESSON_NUM));
                    userLessonBean.setStartDate((Date) entity.getProperty(Const.DS_EN_USER_LESSON_P_START_DATE));
                    userLessonBean.setEndDate((Date) entity.getProperty(Const.DS_EN_USER_LESSON_P_END_DATE));
                    userLessonBean.setUpdateDate((Date) entity.getProperty(Const.DS_EN_USER_LESSON_P_UPDATE_DATE));

                    // Add lesson to list
                    lessons.add(userLessonBean);
                }
            }
        } catch (PreparedQuery.TooManyResultsException e) {
            mLogger.log(Level.SEVERE, e.toString());
        }

        // Return the response
        return CollectionResponse.<UserLessonBean>builder().setItems(lessons).build();
    }

    //----------------------------------------------------------------------------------------------
    // Raga API
    //----------------------------------------------------------------------------------------------
    @ApiMethod(name = "getRaga")
    public RagaBean getRaga(
            @Named("ragaId") String ragaId) {

        NamespaceManager.set(Const.DS_NAMESPACE);
        DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();

        Query query = new Query(Const.DS_KIND_RAGA);
        Query.Filter keyFilter = new Query.FilterPredicate(
                Const.DS_EN_RAGA_P_UID,
                Query.FilterOperator.EQUAL,
                ragaId);

        query.setFilter(keyFilter);
        PreparedQuery preparedQuery = dsService.prepare(query);
        Entity entity = preparedQuery.asSingleEntity();

        RagaBean ragaBean = new RagaBean();

        if (entity != null) {
            ragaBean.setId((String)entity.getProperty(Const.DS_EN_RAGA_P_UID));
            ragaBean.setName((String)entity.getProperty(Const.DS_EN_RAGA_P_NAME));
            ragaBean.setArohana((String)entity.getProperty(Const.DS_EN_RAGA_P_AROHANA));
            ragaBean.setAvarohana((String)entity.getProperty(Const.DS_EN_RAGA_P_AVAROHANA));
            ragaBean.setMelakartha((String)entity.getProperty(Const.DS_EN_RAGA_P_MELAKARTHA));
            ragaBean.setPicture((String)entity.getProperty(Const.DS_EN_RAGA_P_PIC));
            ragaBean.setUpdateDate((Date)entity.getProperty(Const.DS_EN_RAGA_P_UPDATE_DATE));
        }
        return ragaBean;
    }

    //----------------------------------------------------------------------------------------------
    // Lessons API
    //----------------------------------------------------------------------------------------------
    /**
     * API method for getting Details of a particular lesson
     */
    @ApiMethod(name = "getLesson")
    public LessonBean getLesson(
            @Named("lessonId") String lessonId) {

        NamespaceManager.set(Const.DS_NAMESPACE);
        DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();

        Query query = new Query(Const.DS_KIND_LESSON);
        Query.Filter keyFilter = new Query.FilterPredicate(
                Const.DS_EN_LESSON_P_UID,
                Query.FilterOperator.EQUAL,
                lessonId);

        query.setFilter(keyFilter);
        PreparedQuery preparedQuery = dsService.prepare(query);
        Entity entity = preparedQuery.asSingleEntity();

        LessonBean lessonBean = null;

        if (entity != null) {
            lessonBean      = new LessonBean();
            Key ragaKey     = (Key)entity.getProperty(Const.DS_EN_LESSON_P_RAGA_ID);
            String ragaId   = getRagaId(ragaKey);

            lessonBean.setId((String)entity.getProperty(Const.DS_EN_LESSON_P_UID));
            lessonBean.setRagaId(ragaId);
            lessonBean.setAuthor((String)entity.getProperty(Const.DS_EN_LESSON_P_AUTHOR));
            lessonBean.setName((String)entity.getProperty(Const.DS_EN_LESSON_P_NAME));
            lessonBean.setLessonUrl((String)entity.getProperty(Const.DS_EN_LESSON_P_LESSON));
            lessonBean.setLessonPicUrl((String)entity.getProperty(Const.DS_EN_LESSON_P_LESSON_PIC));
            lessonBean.setThala((String)entity.getProperty(Const.DS_EN_LESSON_P_THALA));
            lessonBean.setUpdateDate((Date)entity.getProperty(Const.DS_EN_LESSON_P_UPDATE_DATE));
        }

        return lessonBean;
    }

    /**
     * API method for getting all resources for a given lesson
     */
    @ApiMethod(name = "getLessonResources")
    public CollectionResponse<LessonResourceBean> getLessonResources(
            @Named("lessonId") String lessonId) {

        ArrayList<LessonResourceBean> resources = new ArrayList<>();

        try {
            NamespaceManager.set(Const.DS_NAMESPACE);
            DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();

            Key lessonKey = getLessonKey(lessonId);

            Query query = new Query(Const.DS_KIND_LESSON_RES);
            Query.Filter keyFilter = new Query.FilterPredicate(
                    Const.DS_EN_USER_LESSON_P_LESSON_ID,
                    Query.FilterOperator.EQUAL,
                    lessonKey);
            query.setFilter(keyFilter);
            PreparedQuery preparedQuery = dsService.prepare(query);

            for (Entity entity : preparedQuery.asIterable()) {
                if (entity != null) {
                    LessonResourceBean resourceBean = new LessonResourceBean();
                    Key typeKey         = (Key)entity.getProperty(Const.DS_EN_LESSON_RES_P_TYPE_ID);

                    Long resType        = getLessonResourceType(typeKey);

                    resourceBean.setId((String)entity.getProperty(Const.DS_EN_LESSON_RES_P_UID));
                    resourceBean.setType(resType);
                    resourceBean.setResval((String)entity.getProperty(Const.DS_EN_LESSON_RES_P_RESVAL));
                    resourceBean.setDescription((String)entity.getProperty(Const.DS_EN_LESSON_RES_P_DESC));
                    resourceBean.setNum((Long)entity.getProperty(Const.DS_EN_LESSON_RES_P_NUM));
                    resourceBean.setUpdateDate((Date)entity.getProperty(Const.DS_EN_LESSON_RES_P_UPDATE_DATE));

                    resources.add(resourceBean);
                }
            }

            // Sort ASC based on num
            Collections.sort(resources, new Comparator<LessonResourceBean>() {
                @Override
                public int compare(LessonResourceBean o1, LessonResourceBean o2) {
                    return o1.getNum().compareTo(o2.getNum());
                }
            });

        } catch (PreparedQuery.TooManyResultsException e) {
            mLogger.log(Level.SEVERE, e.toString());
        }

        // Return the response
        return CollectionResponse.<LessonResourceBean>builder().setItems(resources).build();
    }


    /**
     * Get Raga ID for the Raga Key
     */
    private String getRagaId(Key ragaKey) {
        DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();
        Query query = new Query(Const.DS_KIND_RAGA);
        Query.Filter keyFilter = new Query.FilterPredicate(
                Entity.KEY_RESERVED_PROPERTY,
                Query.FilterOperator.EQUAL,
                ragaKey);

        query.setFilter(keyFilter);
        PreparedQuery preparedQuery = dsService.prepare(query);
        Entity entity = preparedQuery.asSingleEntity();

        String ragaId = null;

        if (entity != null) {
            ragaId = (String)entity.getProperty(Const.DS_EN_RAGA_P_UID);
        }

        return ragaId;
    }

    /**
     * Get Raga Key for the Raga ID
     */
    private Key getRagaKey(Key ragaId) {
        DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();
        Query query = new Query(Const.DS_KIND_RAGA);
        Query.Filter keyFilter = new Query.FilterPredicate(
                Const.DS_EN_RAGA_P_UID,
                Query.FilterOperator.EQUAL,
                ragaId);

        query.setFilter(keyFilter);
        PreparedQuery preparedQuery = dsService.prepare(query);
        Entity entity = preparedQuery.asSingleEntity();

        Key  ragaKey = null;

        if (entity != null) {
            ragaKey = entity.getKey();
        }

        return ragaKey;
    }

    /**
     * Returns the lesson_id given lesson key
     * @param lessonKey
     * @return
     */
    private String getLessonId(Key lessonKey) {
        DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();
        Query query = new Query(Const.DS_KIND_LESSON);
        Query.Filter keyFilter = new Query.FilterPredicate(
                Entity.KEY_RESERVED_PROPERTY,
                Query.FilterOperator.EQUAL,
                lessonKey);

        query.setFilter(keyFilter);
        PreparedQuery preparedQuery = dsService.prepare(query);
        Entity entity = preparedQuery.asSingleEntity();

        String lessonId = null;

        if (entity != null) {
            lessonId = (String)entity.getProperty(Const.DS_EN_LESSON_P_UID);
        }

        return lessonId;
    }

    /**
     * Returns the lesson_id given lesson key
     * @param lessonId
     * @return
     */
    private Key getLessonKey(String lessonId) {
        DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();
        Query query = new Query(Const.DS_KIND_LESSON);
        Query.Filter keyFilter = new Query.FilterPredicate(
                Const.DS_EN_LESSON_P_UID,
                Query.FilterOperator.EQUAL,
                lessonId);

        query.setFilter(keyFilter);
        PreparedQuery preparedQuery = dsService.prepare(query);
        Entity entity = preparedQuery.asSingleEntity();

        Key lessonKey = null;

        if (entity != null) {
            lessonKey = entity.getKey();
        }

        return lessonKey;
    }


    /**
     * Returns the LessonResourceType for the given key
     */
    private Long getLessonResourceType(Key key) {
        DatastoreService dsService = DatastoreServiceFactory.getDatastoreService();

        Query query = new Query(Const.DS_KIND_RES_TYPE);
        Query.Filter keyFilter = new Query.FilterPredicate(
                Entity.KEY_RESERVED_PROPERTY,
                Query.FilterOperator.EQUAL,
                key);

        query.setFilter(keyFilter);
        PreparedQuery preparedQuery = dsService.prepare(query);
        Entity entity = preparedQuery.asSingleEntity();
        Long resType = -1L;

        if (entity != null) {
            resType = (Long)entity.getProperty(Const.DS_EN_RES_TYPE_P_TYPE);
        }

        return resType;
    }
}
