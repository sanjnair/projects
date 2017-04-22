package com.example.xmatrix.popularmovies;

import android.content.Intent;
//import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements GridImageSelectedListener {
    private final String LOG_TAG = MainActivity.class.getSimpleName();

    private boolean mTwoPane;
    private DetailFragment m_fragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (findViewById(R.id.movie_detail_container) != null) {
            // The detail container view will be present only in the
            // large-screen layouts (res/values-large and
            // res/values-sw600dp). If this view is present, then the
            // activity should be in two-pane mode.
            mTwoPane = true;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            startActivity(new Intent(this, SettingsActivity.class));
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * GridImageSelectedListener
     * @param bundle
     */
    public void OnSelectionChanged(Bundle bundle)
    {
        Log.d(LOG_TAG, "OnSelectionChanged mTwoPane is " + mTwoPane);

        if (mTwoPane) {
            // In two-pane mode, show the detail view in this activity by
            // adding or replacing the detail fragment using a
            // fragment transaction.
            if (m_fragment != null)
            {
                getFragmentManager().beginTransaction().remove(m_fragment).commit();
                m_fragment = null;
            }

            m_fragment = new DetailFragment();
            m_fragment.setArguments(bundle);
            getFragmentManager().beginTransaction()
                    .replace(R.id.movie_detail_container, m_fragment)
                    .commit();

        } else {
            // In single-pane mode, simply start the detail activity
            // for the selected item ID.
            Intent detailIntent = new Intent(this, DetailActivity.class);
            detailIntent.putExtra(DetailActivity.ARG_ITEM_ID, bundle);
            startActivity(detailIntent);
        }
    }

    public void onPreferenceChange()
    {
        if(m_fragment != null)
        {
            getFragmentManager().beginTransaction().remove(m_fragment).commit();
        }
    }
}
