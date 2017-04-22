package com.example.xmatrix.popularmovies;

import android.app.Activity;
import android.content.Context;
import android.media.Image;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.widget.ArrayAdapter;
import android.view.ViewGroup;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import java.util.ArrayList;


/**
 * Created by xmatrix on 8/11/2015.
 */
public class TrailerListAdapter extends ArrayAdapter<String> {

    ArrayList<Bundle> m_Trailers;

    public TrailerListAdapter(Context c) {
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
            rowView = inflater.inflate(R.layout.trailer_item_layout, parent, false);
        }

        TextView txtTitle = (TextView) rowView.findViewById(R.id.trailer_text);
        txtTitle.setText(getItem(position));

       return rowView;
    }

    /**
     * Set items for the array and notifies dataset changed
     */
    public void setData(ArrayList<Bundle> oTrailers)
    {
        m_Trailers = oTrailers;
        clear();

        for (int i=0; i < oTrailers.size(); ++i) {
            Bundle trailer = oTrailers.get(i);
            if (trailer.containsKey(BundleKeys.TRAILER_NAME)) {
                add(trailer.getString(BundleKeys.TRAILER_NAME));
            }
        }

        notifyDataSetChanged();
    }

    /**
     * Get data for the given index
     */
    Bundle GetDataItem(int index)
    {
        if ((m_Trailers != null) && (index >= 0) && (index < m_Trailers.size()))
        {
            return m_Trailers.get(index);
        }
        return null;
    }
}
