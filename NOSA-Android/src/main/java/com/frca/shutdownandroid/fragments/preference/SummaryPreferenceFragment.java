package com.frca.shutdownandroid.fragments.preference;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.text.InputType;
import android.util.Log;

public class SummaryPreferenceFragment extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener {
    @Override
    public void onActivityCreated(Bundle savedInstanceBundle) {
        super.onActivityCreated(savedInstanceBundle);

        SharedPreferences preferences = getPreferenceScreen().getSharedPreferences();
        for (String key : preferences.getAll().keySet()) {
            setValueAsSummary(preferences, key);
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        getPreferenceScreen()
            .getSharedPreferences()
            .registerOnSharedPreferenceChangeListener(this);
    }

    @Override
    public void onPause() {
        super.onPause();

        getPreferenceScreen()
            .getSharedPreferences()
            .unregisterOnSharedPreferenceChangeListener(this);
    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        setValueAsSummary(sharedPreferences, key);
    }

    private void setValueAsSummary(SharedPreferences sharedPreferences, String key) {
        Preference pref = findPreference(key);

        if (pref instanceof ListPreference) {
            ListPreference listPreference = (ListPreference) pref;
            pref.setSummary(listPreference.getEntry());
        }

        if (pref instanceof EditTextPreference) {
            EditTextPreference editTextPreference = (EditTextPreference) pref;
            int variation = editTextPreference.getEditText().getInputType() & InputType.TYPE_MASK_VARIATION;
            boolean isPassword = ((variation  == InputType.TYPE_NUMBER_VARIATION_PASSWORD)
                                ||(variation  == InputType.TYPE_TEXT_VARIATION_PASSWORD)
                                ||(variation  == InputType.TYPE_TEXT_VARIATION_WEB_PASSWORD));

            String text = editTextPreference.getText();
            if (isPassword)
                text = text.replaceAll(".", "*");

            editTextPreference.setSummary(text);
        }
    }
}
