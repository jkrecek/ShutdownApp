package com.frca.shutdownandroid.fragments;

import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;

import com.frca.shutdownandroid.Connection;
import com.frca.shutdownandroid.MainActivity;

public abstract class BaseFragment extends Fragment {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!(getActivity() instanceof MainActivity)) {
            Log.e(getClass().getName(), "This class must be child of MainActivity");
            getActivity().finish();
        }
    }

    protected MainActivity getMainActivity() {
        return (MainActivity) getActivity();
    }
}
