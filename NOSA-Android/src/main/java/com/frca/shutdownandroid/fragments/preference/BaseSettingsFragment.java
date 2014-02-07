package com.frca.shutdownandroid.fragments.preference;

import android.os.Bundle;
import android.preference.Preference;
import android.widget.Toast;

import com.frca.shutdownandroid.R;
import com.frca.shutdownandroid.network.NetworkThread;

public class BaseSettingsFragment extends SummaryPreferenceFragment {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.xml.fragment_settings);

        Preference preference = findPreference(getString(R.string.edit_text_host_key));
        if (preference == null)
            return;

        preference.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object o) {
                String newValue = o.toString();
                if (NetworkThread.IP_ADDRESS.matcher(newValue).matches())
                    return true;

                if (newValue.equals(NetworkThread.LOCALHOST))
                    return true;

                if (NetworkThread.HOST_ADDRESS.matcher(newValue).matches())
                    return true;

                Toast.makeText(getActivity(), "Invalid remote address.", Toast.LENGTH_LONG).show();
                return false;
            }
        });
    }
}