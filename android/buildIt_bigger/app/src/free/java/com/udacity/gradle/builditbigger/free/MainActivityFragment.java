package com.udacity.gradle.builditbigger.free;

import android.content.Intent;
import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;
import com.udacity.gradle.builditbigger.R;
import com.xmatrix.jokeslib.EndpointsAsyncTask;
import com.xmatrix.jokeslib.JokesActivity;

/**
 * Main Activity Fragment
 */
public class MainActivityFragment extends Fragment implements EndpointsAsyncTask.JokesNotifier {
    private ProgressBar mSpinner;
    private InterstitialAd mInterstitialAd;

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

        AdView mAdView = (AdView) root.findViewById(R.id.adView);
        // Create an ad request. Check logcat output for the hashed device ID to
        // get test ads on a physical device. e.g.
        // "Use AdRequest.Builder.addTestDevice("ABCDEF012345") to get test ads on this device."
        AdRequest adRequest = new AdRequest.Builder()
                .addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
                .build();
        mAdView.loadAd(adRequest);

        // Interstitial ad
        mInterstitialAd = new InterstitialAd(getActivity());
        String interstitial_id = this.getString(R.string.interstitial_id);
        mInterstitialAd.setAdUnitId(interstitial_id);
        mInterstitialAd.setAdListener(new AdListener() {
            @Override
            public void onAdClosed() {
                requestNewInterstitial();
                fetchJoke();
            }
        });
        requestNewInterstitial();

        // Button for joke
        Button btnJoke = (Button) root.findViewById(R.id.btn_joke);
        btnJoke.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mInterstitialAd.isLoaded()) {
                    mInterstitialAd.show();
                } else {
                    fetchJoke();
                }
            }
        });

        return root;
    }

    private void requestNewInterstitial() {
        AdRequest adRequest = new AdRequest.Builder()
                .addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
                        .build();
        mInterstitialAd.loadAd(adRequest);
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
        mSpinner.setVisibility(View.VISIBLE);
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
