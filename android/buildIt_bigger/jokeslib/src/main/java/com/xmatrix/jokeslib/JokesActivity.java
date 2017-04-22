package com.xmatrix.jokeslib;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class JokesActivity extends AppCompatActivity {

    // Intent ID
    public static final String INTENT_JOKE_VALUE = "com.xmatrix.jokeslib.joke_value";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(com.xmatrix.jokeslib.R.layout.activity_jokes);

        Intent intent = getIntent();
        if ((intent != null) && intent.hasExtra(INTENT_JOKE_VALUE)) {
            TextView txtJoke = (TextView) findViewById(R.id.joke);
            txtJoke.setText(intent.getStringExtra(INTENT_JOKE_VALUE));
        }
    }
}
