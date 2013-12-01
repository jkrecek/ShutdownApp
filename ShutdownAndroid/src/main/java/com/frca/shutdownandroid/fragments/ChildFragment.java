package com.frca.shutdownandroid.fragments;

import android.app.Fragment;
import android.os.Bundle;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.frca.shutdownandroid.Connection;
import com.frca.shutdownandroid.MainActivity;
import com.frca.shutdownandroid.NetworkThread;

public abstract class ChildFragment extends Fragment {
    protected Connection connection;

    protected ChildFragment(Connection connection) {
        this.connection = connection;
    }

    protected MainActivity getMainActivity() {
        return (MainActivity) getActivity();
    }
}
