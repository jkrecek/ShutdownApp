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

import com.frca.shutdownandroid.Connection;
import com.frca.shutdownandroid.MainActivity;
import com.frca.shutdownandroid.R;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class MainFragment extends Fragment {

    private Connection connection;

    private TextView text_connetion;
    private LinearLayout layout_state;
    private ImageView image_state;
    private TextView text_state;
    private ProgressBar progress;

    private Fragment currentChildFragment;

    public MainFragment(Connection connection) {
        this.connection = connection;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        /*Display display = getActivity().getWindowManager().getDefaultDisplay();
        int width = display.getWidth();
        int height = display.getHeight();
        LinearLayout.LayoutParams params;
        //LinearLayout.LayoutParams childParams;
        //int childOrientation;
        if (width > height) {
            layoutView.setOrientation(LinearLayout.HORIZONTAL);
            //childOrientation = LinearLayout.HORIZONTAL;
            params = new LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.MATCH_PARENT, 1);
            //childParams = new LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.MATCH_PARENT, 1);
        } else {
            layoutView.setOrientation(LinearLayout.VERTICAL);

            //childOrientation = LinearLayout.VERTICAL;
            params = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 0, 1);
            //childParams = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 0, 1);

        }*/
        /*for (int i = 0; i < layoutView.getChildCount(); ++i) {
            LinearLayout child = (LinearLayout) layoutView.getChildAt(i);*/
            //child.setLayoutParams(params);
            /*child.setOrientation(childOrientation);
            for (int a = 0; a < child.getChildCount(); ++a) {
                View image = child.getChildAt(a);
                if (image instanceof ImageView) {
                    image.setLayoutParams(childParams);
                    break;
                }
            }*/

            /*child.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    String str = null;
                    switch (view.getId()) {
                        case R.id.turn_off: str = "TURN_OFF"; break;
                        case R.id.restart:  str = "RESTART"; break;
                        case R.id.lock:     str = "LOCK"; break;
                        case R.id.sleep:    str = "SLEEP"; break;
                    }

                    if (str == null)
                        Toast.makeText(getActivity(), "No action specified for view id `" + String.valueOf(view.getId()) + "`", Toast.LENGTH_LONG).show();
                    else {
                        NetworkThread thread = ((MainActivity)getActivity()).getThread();
                        thread.setIp(connection.getIp());
                        thread.sendMessage(str);
                    }
                }
            });*/
        //}

        //layoutView.setOrientation(width > height ? LinearLayout.HORIZONTAL : LinearLayout.VERTICAL);

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

        refresh();

        return layoutView;
    }

    @Override
    public void onViewCreated (View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        currentChildFragment = new LoadingFragment();
        getChildFragmentManager().beginTransaction().replace(R.id.child_container, currentChildFragment).commit();
    }

    private void refresh() {
        progress.setVisibility(View.VISIBLE);
        layout_state.setVisibility(View.GONE);
        connection.isOnline(getMainActivity().getThread(), new Connection.PingResult() {
            @Override
            public void result(boolean success) {
                image_state.setImageResource(success ? android.R.drawable.presence_online : android.R.drawable.presence_offline);
                text_state.setText(success ? R.string.online : R.string.offline);

                refreshChild(success);

                progress.setVisibility(View.GONE);
                layout_state.setVisibility(View.VISIBLE);
            }
        });
    }

    private void refreshChild(boolean success) {
        if (getActivity() == null)
            return;

        if (currentChildFragment != null) {
            if (success ? currentChildFragment instanceof OnlineFragment : currentChildFragment instanceof OfflineFragment) {
                return;
            }
        }

        currentChildFragment = success ? new OnlineFragment(connection) : new OfflineFragment(connection);
        getChildFragmentManager().beginTransaction().replace(R.id.child_container, currentChildFragment).commit();
    }

    public MainActivity getMainActivity() {
        return (MainActivity) getActivity();
    }

}
