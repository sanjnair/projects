package com.xmatrix.melange;

import android.appwidget.AppWidgetManager;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.TextView;

import com.xmatrix.melange.data.DbContract;
import com.xmatrix.melange.data.DbLessonResUtil;
import com.xmatrix.melange.data.DbLessonUtil;

import static org.apache.commons.lang3.StringEscapeUtils.unescapeHtml4;
import java.util.Date;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link LessonDetailFragment.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link LessonDetailFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class LessonDetailFragment extends Fragment {
    private static final String LOG_TAG         = LessonDetailFragment.class.getSimpleName();
    private static final String ARG_LESSON_ID   = "lesson_id";

    private static final String[] LESSON_DETAIL_PROJECTION = {
            DbContract.LessonEntry._ID,
            DbContract.LessonEntry.COLUMN_RAGA_ID,
            DbContract.LessonEntry.COLUMN_NAME,
            DbContract.LessonEntry.COLUMN_PIC_URL,
            DbContract.LessonEntry.COLUMN_NUM,
            DbContract.LessonEntry.COLUMN_AUTHOR,
            DbContract.LessonEntry.COLUMN_DETAIL,
            DbContract.LessonEntry.COLUMN_THALA,
            DbContract.LessonEntry.COLUMN_START_DATE,
            DbContract.LessonEntry.COLUMN_END_DATE,
            DbContract.LessonEntry.COLUMN_WATCHED
            };

    private String LESSON_DETAIL_SELECTION =
            String.format("%s = ?", DbContract.LessonEntry._ID);

    private static final String[] RAGA_DETAIL_PROJECTION = {
            DbContract.RagaEntry.COLUMN_NAME,
            DbContract.RagaEntry.COLUMN_AROHANA,
            DbContract.RagaEntry.COLUMN_AVAROHANA,
            DbContract.RagaEntry.COLUMN_MELAKARTHA,
            DbContract.RagaEntry.COLUMN_PIC_URL
            };

    private String RAGA_DETAIL_SELECTION =
            String.format("%s = ?", DbContract.RagaEntry._ID);


    /**
     * Inner class for Lesson Detail Data
     */
    private class LessonData {
        public int      lessonId;
        public String   lessonName;
        public int      lessonNum;
        public String   lessonAuthor;
        public String   lessonDetail;
        public String   lessonThala;
        public String   lessonPicUrl;
        public int      lessonWatched;
        public Date     startDate;
        public Date     endDate;

        public int      ragaId;
        public String   ragaName;
        public String   ragaArohana;
        public String   ragaAvarohana;
        public String   ragaMelakartha;
        public String   ragaPicUrl;
    }

    private int mLessonId;
    private LessonData mLessonData;
    private OnFragmentInteractionListener mListener;

    public LessonDetailFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @return A new instance of fragment LessonDetailFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static LessonDetailFragment newInstance(int lessonId) {
        LessonDetailFragment fragment = new LessonDetailFragment();
        Bundle args = new Bundle();
        args.putInt(ARG_LESSON_ID, lessonId);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);

        if (getArguments() != null) {
            mLessonId = getArguments().getInt(ARG_LESSON_ID);
        }
    }

    @Override
    public View onCreateView(
            LayoutInflater inflater,
            ViewGroup container,
            Bundle savedInstanceState) {

        Log.d(LOG_TAG, "onCreateView called");
        View rootView = inflater.inflate(R.layout.fragment_lesson_detail, container, false);

        populateUiElements(rootView);

        final Button resButton = (Button) rootView.findViewById(R.id.lesson_detail_resources);
        int lessonResources = DbLessonResUtil.getNumLessonResources(getContext(), mLessonId);

        Log.d(LOG_TAG, "lessonResources = " + lessonResources + " lessonId = " + mLessonId);

        if (lessonResources > 0) {
            resButton.setVisibility(View.VISIBLE);
            resButton.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    Log.d(LOG_TAG, "Starting LessonResources Activity");
                    Intent intent = new Intent(getActivity(), LessonResActivity.class);
                    intent.putExtra(LessonDetailActivity.LESSON_ID, mLessonId);
                    getActivity().startActivity(intent);
                }
            });
        } else {
            resButton.setVisibility(View.GONE);
        }

        // Inflate the layout for this fragment
        return rootView;
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onButtonPressed(Uri uri) {
        if (mListener != null) {
            mListener.onFragmentInteraction(uri);
        }
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnFragmentInteractionListener) {
            mListener = (OnFragmentInteractionListener) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }


    /**
     * This interface must be implemented by activities that contain this
     * fragment to allow an interaction in this fragment to be communicated
     * to the activity and potentially other fragments contained in that
     * activity.
     * <p/>
     * See the Android Training lesson <a href=
     * "http://developer.android.com/training/basics/fragments/communicating.html"
     * >Communicating with Other Fragments</a> for more information.
     */
    public interface OnFragmentInteractionListener {
        // TODO: Update argument type and name
        void onFragmentInteraction(Uri uri);

    }

    /**
     * Populate UI Elements
     */
    void populateUiElements(
            View rootView) {

        LessonData lessonData = getLessonData(getContext());
        String lessonHeader = String.valueOf(lessonData.lessonNum) + ". " + lessonData.lessonName;

        ((TextView) rootView.findViewById(R.id.lesson_detail_name)).setText(lessonHeader);
        ((TextView) rootView.findViewById(R.id.lesson_detail_author)).setText(lessonData.lessonAuthor);
        ((TextView) rootView.findViewById(R.id.lesson_detail_thala)).setText(lessonData.lessonThala);
        ((TextView) rootView.findViewById(R.id.lesson_detail_ragam)).setText(lessonData.ragaName);
        ((TextView) rootView.findViewById(R.id.lesson_detail_melakartha)).setText(lessonData.ragaMelakartha);
        ((TextView) rootView.findViewById(R.id.lesson_detail_arohana)).setText(lessonData.ragaArohana);
        ((TextView) rootView.findViewById(R.id.lesson_detail_avarohana)).setText(lessonData.ragaAvarohana);

        WebView webView = ((WebView) rootView.findViewById(R.id.lesson_detail_description));
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
        webView.loadData(lessonData.lessonDetail, "text/html", "utf-8");

        if (lessonData.lessonWatched <= 0) {
            updateLessonWatchedState(rootView.getContext());
        }
    }

    /**
     * Load data from database
     */
    LessonData getLessonData(Context context) {

        LessonData data = null;

        Cursor cursor = context.getContentResolver().query(
                            DbContract.LessonEntry.CONTENT_URI,
                            LESSON_DETAIL_PROJECTION,
                            LESSON_DETAIL_SELECTION,
                            new String[]{String.valueOf(mLessonId)},
                            null);

        if (cursor.moveToFirst()) {
            data = new LessonData();

            data.lessonId       = cursor.getInt(cursor.getColumnIndex(DbContract.LessonEntry._ID));
            data.ragaId         = cursor.getInt(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_RAGA_ID));
            data.lessonName     = cursor.getString(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_NAME));
            data.lessonNum      = cursor.getInt(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_NUM));
            data.lessonAuthor   = cursor.getString(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_AUTHOR));
            data.lessonDetail   = cursor.getString(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_DETAIL));
            data.lessonThala    = cursor.getString(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_THALA));
            data.lessonPicUrl   = cursor.getString(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_PIC_URL));
            long startDate      = cursor.getLong(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_START_DATE));
            data.startDate      = new Date(startDate);
            long endDate        =  cursor.getLong(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_END_DATE));
            data.endDate        = new Date(endDate);
            data.lessonWatched  = cursor.getInt(cursor.getColumnIndex(DbContract.LessonEntry.COLUMN_WATCHED));

            //unescape html data
            data.lessonDetail = unescapeHtml4(data.lessonDetail);

            fillRagaData(context, data);

        } else {
            Log.d(LOG_TAG, "Unable to get Lesson Detail");
        }


        return data;
    }

    /**
     * Fill the Raga information
     */
    void fillRagaData(
            Context context,
            LessonData data) {
        Cursor cursor = context.getContentResolver().query(
                DbContract.RagaEntry.CONTENT_URI,
                RAGA_DETAIL_PROJECTION,
                RAGA_DETAIL_SELECTION,
                new String[]{String.valueOf(data.ragaId)},
                null);

        if (cursor.moveToFirst()) {
            data.ragaName       = cursor.getString(cursor.getColumnIndex(DbContract.RagaEntry.COLUMN_NAME));
            data.ragaArohana    = cursor.getString(cursor.getColumnIndex(DbContract.RagaEntry.COLUMN_AROHANA));
            data.ragaAvarohana  = cursor.getString(cursor.getColumnIndex(DbContract.RagaEntry.COLUMN_AVAROHANA));
            data.ragaMelakartha = cursor.getString(cursor.getColumnIndex(DbContract.RagaEntry.COLUMN_MELAKARTHA));
            data.ragaPicUrl     = cursor.getString(cursor.getColumnIndex(DbContract.RagaEntry.COLUMN_PIC_URL));

        } else {
            Log.d(LOG_TAG, "Unable to get raga Detail");
        }
    }

    /**
     * Update lesson Watched property and broadcast
     */
    private void updateLessonWatchedState(Context context) {
        // update database
        DbLessonUtil.setLessonWatched(context, mLessonId);

        Log.d(LOG_TAG, "Broadcasting lessonWatched state");
        Intent intent = new Intent(context.getString(R.string.widget_action), null);
        context.sendBroadcast(intent);
    }
}
