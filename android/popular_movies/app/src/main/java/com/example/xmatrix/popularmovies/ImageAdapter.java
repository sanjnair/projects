package com.example.xmatrix.popularmovies;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.view.ViewGroup;
import android.view.View;
import com.squareup.picasso.Picasso;
import java.util.ArrayList;


/**
 * Created by xmatrix on 8/11/2015.
 */
public class ImageAdapter extends BaseAdapter{
    private Context m_Context;
    private ArrayList<Bundle> m_BundleList;

    public ImageAdapter(Context c) {
        m_Context = c;
        m_BundleList = null;
    }

    public int getCount() {
        if (m_BundleList == null) {
            return 0;
        }
        return m_BundleList.size();
    }

    public Object getItem(int position) {
        if (position < m_BundleList.size()) {
            return m_BundleList.get(position);
        }

        return null;
    }

    public long getItemId(int position) {
        return 0;
    }

    public void clear() {
        if (m_BundleList != null) {
            m_BundleList.clear();
        }
    }

    public void setBundleList(ArrayList<Bundle> items) {
        m_BundleList = items;
    }

    public ArrayList<Bundle> getBundleList()
    {
        return m_BundleList;
    }

    // create a new ImageView for each item referenced by the Adapter
    public View getView(int position, View convertView, ViewGroup parent) {
        View row_img = convertView;

        if (row_img == null) {
            LayoutInflater inflater = ((Activity) m_Context).getLayoutInflater();
            row_img = inflater.inflate(R.layout.grid_item_layout, parent, false);
        }

        ImageView imageView = (ImageView)row_img.findViewById(R.id.grid_image);

        if (position < m_BundleList.size())
        {
            Bundle bundle = m_BundleList.get(position);
            String url = bundle.getString(BundleKeys.IMAGE_URL);

            Picasso.with(m_Context)
                    .load(url)
                    .fit()
                    .centerInside()
                    .placeholder(R.drawable.progress)
                    .error(R.drawable.load_error)
                    .into(imageView);
        }

       return imageView;
    }
}
