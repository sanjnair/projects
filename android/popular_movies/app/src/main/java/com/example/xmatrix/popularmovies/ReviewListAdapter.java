package com.example.xmatrix.popularmovies;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.text.Html;
import android.text.method.LinkMovementMethod;
import android.text.util.Linkify;
import android.view.LayoutInflater;
import android.widget.ArrayAdapter;
import android.view.ViewGroup;
import android.view.View;
import android.widget.TextView;
import java.util.ArrayList;
import java.util.regex.Pattern;


/**
 * Created by xmatrix on 8/11/2015.
 */
public class ReviewListAdapter extends ArrayAdapter<String> {

    final int MAX_REVIEW_LEN = 500;
    ArrayList<Bundle> m_Reviews;

    public ReviewListAdapter(Context c) {
        super(c, R.layout.trailer_item_layout);
    }

    /**
     * Create view for each referenced item in the array
     * @param position
     * @param convertView
     * @param parent
     * @return
     */
    public View getView(
            int position,
            View convertView,
            ViewGroup parent) {

        View rowView = convertView;

        if (rowView == null)
        {
            LayoutInflater inflater = ((Activity)getContext()).getLayoutInflater();
            rowView = inflater.inflate(R.layout.review_item_layout, parent, false);
        }

        TextView txtAuthor = (TextView) rowView.findViewById(R.id.review_author);
        txtAuthor.setText(m_Reviews.get(position).getString(BundleKeys.REVIEW_AUTHOR));

        TextView txtTitle = (TextView) rowView.findViewById(R.id.review_text);
        txtTitle.setText(getItem(position));

        TextView txtWebUrl = (TextView) rowView.findViewById(R.id.review_web_url);
        txtWebUrl.setMovementMethod(LinkMovementMethod.getInstance());
        String sUrl = m_Reviews.get(position).getString(BundleKeys.REVIEW_MORE_URL);
        String html = "<a href='" + sUrl + "'>Info on web</a>";
        txtWebUrl.setText(Html.fromHtml(html));

       return rowView;
    }

    /**
     * Set items for the array and notifies dataset changed
     */
    public void setData(ArrayList<Bundle> oReviews)
    {
        m_Reviews = oReviews;
        clear();

        for (int i=0; i < oReviews.size(); ++i) {
            Bundle review = oReviews.get(i);
            if (review.containsKey(BundleKeys.REVIEW_CONTENT)) {
                String sContent = review.getString(BundleKeys.REVIEW_CONTENT);
                if (sContent.length() > MAX_REVIEW_LEN)
                {
                    sContent = sContent.substring(0, MAX_REVIEW_LEN) + "...";
                }

                add(sContent);
            }
        }

        notifyDataSetChanged();
    }

    /**
     * Get data for the given index
     */
    Bundle GetDataItem(int index)
    {
        if ((m_Reviews != null) && (index >= 0) && (index < m_Reviews.size()))
        {
            return m_Reviews.get(index);
        }
        return null;
    }
}
