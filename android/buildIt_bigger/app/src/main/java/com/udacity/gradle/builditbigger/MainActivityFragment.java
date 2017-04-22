package com.udacity.gradle.builditbigger;

import android.content.Intent;
import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;

import com.xmatrix.jokeslib.EndpointsAsyncTask;
import com.xmatrix.jokeslib.JokesActivity;

/**
 * Main Activity Fragment
 */
public class MainActivityFragment extends Fragment implements EndpointsAsyncTask.JokesNotifier {
    private ProgressBar mSpinner;

    /**
     * Constructor
     */
    public MainActivityFragment() {
    }

    @Override
    public View onCreateView(
            LayoutInflater inflater,
            ViewGroup container,
            Bundle savedInstanceState) {

        getActivity().setTitle(R.string.main_title);
        View root = inflater.inflate(R.layout.fragment_main, container, false);

        mSpinner = (ProgressBar) root.findViewById(R.id.prgBarWait);
        mSpinner.setVisibility(View.GONE);

        Button btnJoke = (Button) root.findViewById(R.id.btn_joke);
        btnJoke.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mSpinner.setVisibility(View.VISIBLE);
                fetchJoke();
            }
        });

        return root;
    }

    @Override
    public void onStart()
    {
        super.onStart();
        mSpinner.setVisibility(View.GONE);
    }

    /**
     * Fetches Joke from Endpoint
     */
    private void fetchJoke() {
        new EndpointsAsyncTask(this).execute(getActivity());
    }

    /**
     * EndpointsAsyncTask.JokesNotifier - interface function
     */
    @Override
    public void notifyJoke(String sJoke) {
        Intent intent = new Intent(getActivity(), com.xmatrix.jokeslib.JokesActivity.class);
        intent.putExtra(JokesActivity.INTENT_JOKE_VALUE, sJoke);
        startActivity(intent);
    }
}
