package com.frca.shutdownandroid.fragments;

import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.frca.shutdownandroid.R;
import com.frca.shutdownandroid.classes.Connection;
import com.frca.shutdownandroid.classes.ControlAction;

public class MainFragment extends BaseFragment {

    private static final String EXTRA_ID = "id";

    private Connection connection;

    private TextView text_connetion;
    private LinearLayout layout_state;
    private ImageView image_state;
    private TextView text_state;
    private ProgressBar progress;

    private Fragment currentChildFragment;


    public static final MainFragment newInstance(int id) {
        MainFragment f = new MainFragment();
        Bundle bdl = new Bundle();
        bdl.putInt(EXTRA_ID, id);
        f.setArguments(bdl);
        return f;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        int id = getArguments().getInt(EXTRA_ID);
        connection = getMainActivity().getConnection(id);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        LinearLayout layoutView = (LinearLayout) inflater.inflate(R.layout.fragment_main, container, false);
        if (layoutView == null)
            return null;

        text_connetion = (TextView) layoutView.findViewById(R.id.text_connection);
        layout_state = (LinearLayout) layoutView.findViewById(R.id.layout_state);
        image_state = (ImageView) layoutView.findViewById(R.id.image_state);
        text_state = (TextView) layoutView.findViewById(R.id.text_state);
        progress = (ProgressBar) layoutView.findViewById(R.id.progress);

        int resourceId = connection.getType() == Connection.ConnectionType.DIRECT ? R.string.connection_header_direct : R.string.connection_header_proxy;
        text_connetion.setText(getString(resourceId, connection.getStringIdentifier()));

        ImageButton button_refresh = (ImageButton) layoutView.findViewById(R.id.button_refresh);
        button_refresh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                refresh();
            }
        });

        currentChildFragment = new ControlFragment();

        getChildFragmentManager().beginTransaction().replace(R.id.child_container, currentChildFragment).commit();

        refresh();

        return layoutView;
    }

    private void refresh() {
        progress.setVisibility(View.VISIBLE);
        layout_state.setVisibility(View.GONE);
        ((ControlFragment)currentChildFragment).setState(null, false);
        connection.isOnline(getMainActivity().getThread(), new Connection.PingResult() {
            @Override
            public void result(boolean success) {
                image_state.setImageResource(success ? android.R.drawable.presence_online : android.R.drawable.presence_offline);
                text_state.setText(success ? R.string.online : R.string.offline);

                ((ControlFragment)currentChildFragment).setState(success ? ControlAction.ConnectionState.ONLINE : ControlAction.ConnectionState.OFFLINE, false);

                progress.setVisibility(View.GONE);
                layout_state.setVisibility(View.VISIBLE);
            }
        });
    }

    public Connection getConnection() {
        return connection;
    }
}
