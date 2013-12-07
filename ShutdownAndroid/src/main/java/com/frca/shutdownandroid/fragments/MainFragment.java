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

import com.frca.shutdownandroid.classes.Connection;
import com.frca.shutdownandroid.classes.ControlAction;
import com.frca.shutdownandroid.R;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class MainFragment extends BaseFragment {

    private static final String EXTRA_IP = "ip";

    private Connection connection;

    private TextView text_connetion;
    private LinearLayout layout_state;
    private ImageView image_state;
    private TextView text_state;
    private ProgressBar progress;

    private Fragment currentChildFragment;


    public static final MainFragment newInstance(String ip) {
        MainFragment f = new MainFragment();
        Bundle bdl = new Bundle();
        bdl.putString(EXTRA_IP, ip);
        f.setArguments(bdl);
        return f;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        String ip = getArguments().getString(EXTRA_IP);
        connection = getMainActivity().getConnectionForIp(ip);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        LinearLayout layoutView = (LinearLayout) inflater.inflate(R.layout.fragment_main, container, false);
        text_connetion = (TextView) layoutView.findViewById(R.id.text_connection);
        layout_state = (LinearLayout) layoutView.findViewById(R.id.layout_state);
        image_state = (ImageView) layoutView.findViewById(R.id.image_state);
        text_state = (TextView) layoutView.findViewById(R.id.text_state);
        progress = (ProgressBar) layoutView.findViewById(R.id.progress);

        //String header = String.format();
        text_connetion.setText(getString(R.string.connection_header, connection.getIp()));

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

    @Override
    public void onViewCreated (View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);


    }

    private void refresh() {
        progress.setVisibility(View.VISIBLE);
        layout_state.setVisibility(View.GONE);
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

    /*private void refreshChild(boolean success) {
        if (getActivity() == null)
            return;

        if (currentChildFragment != null) {
            if (success ? currentChildFragment instanceof ControlFragment : currentChildFragment instanceof OfflineFragment) {
                return;
            }
        }

        currentChildFragment = success ? new ControlFragment() : new OfflineFragment(connection);
        getChildFragmentManager().beginTransaction().replace(R.id.child_container, currentChildFragment).commit();
    }*/

    public Connection getConnection() {
        return connection;
    }
}
