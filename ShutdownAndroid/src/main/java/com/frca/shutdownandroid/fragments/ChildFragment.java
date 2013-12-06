package com.frca.shutdownandroid.fragments;

import android.app.Fragment;

import com.frca.shutdownandroid.Connection;
import com.frca.shutdownandroid.MainActivity;

public abstract class ChildFragment extends Fragment {
    protected Connection connection;

    protected ChildFragment(Connection connection) {
        this.connection = connection;
    }

    protected MainActivity getMainActivity() {
        return (MainActivity) getActivity();
    }
}
