package com.frca.shutdownandroid.fragments;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.GridView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.frca.shutdownandroid.Connection;
import com.frca.shutdownandroid.ControlAction;
import com.frca.shutdownandroid.ControlGridAdapter;
import com.frca.shutdownandroid.Helper;
import com.frca.shutdownandroid.HttpTask;
import com.frca.shutdownandroid.LimitedExecutor;
import com.frca.shutdownandroid.NetworkThread;
import com.frca.shutdownandroid.R;

import org.apache.http.Header;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class ControlFragment extends BaseFragment implements AdapterView.OnItemClickListener {

    /*enum DisplayState {
        LOADING,
        DISPLAY
    }*/

    private Connection connection;

    private String currentMessage;

    private LinearLayout loadingLayout;

    private GridView gridView;

    private ControlAction.ConnectionState state;

    private List<Runnable> postponed = new ArrayList<Runnable>();

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.fragment_control, container, false);

        loadingLayout = (LinearLayout) layout.findViewById(R.id.layout_loading);
        gridView = (GridView) layout.findViewById(R.id.grid);

        gridView.setOnItemClickListener(this);

        setState(null, true);

        return layout;
    }

    @Override
    public void onViewCreated (View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        for (Runnable post : postponed)
            post.run();

        postponed.clear();
    }

    public void setState(final ControlAction.ConnectionState state, boolean force) {
        if (force && this.state == state)
            return;

        this.state = state;
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                if (state == null) {
                    loadingLayout.setVisibility(View.VISIBLE);
                    gridView.setVisibility(View.GONE);
                } else {
                    loadingLayout.setVisibility(View.GONE);
                    gridView.setVisibility(View.VISIBLE);
                    gridView.setAdapter(getGridAdapter(state));
                }

            }
        };

        if (getView() == null)
            postponed.add(runnable);
        else
            runnable.run();

    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        ControlAction action = (ControlAction) adapterView.getAdapter().getItem(i);
        action.getCallback().call(this, action, getConnection(), getMainActivity().getThread());
    }

    private ControlGridAdapter getGridAdapter(ControlAction.ConnectionState state) {
        return new ControlGridAdapter(getActivity(), R.layout.grid_control_item, ControlAction.getValuesWithState(state));
    }

    private Fragment getParent() {
        Fragment f = getParentFragment();
        return f != null ? f : null;
    }

    private MainFragment getMainFragment() {
        Fragment f = getParent();
        return f instanceof MainFragment ?  (MainFragment) f : null;
    }

    private Connection getConnection() {
        if (connection == null) {
            MainFragment f = getMainFragment();
            connection =  f != null ? f.getConnection() : null;
        }
        return connection;
    }

    private List<View> getActionViews(LinearLayout layout) {
        List<View> l = new ArrayList<View>();
        addActionViews(layout, l);
        return l;
    }

    private void addActionViews(LinearLayout layout, List<View> list) {
        for (int i = 0; i < layout.getChildCount(); ++i) {
            View view = layout.getChildAt(i);
            if (view.getTag() != null && view.getTag().equals("action"))
                list.add(view);
            else if (view instanceof LinearLayout)
                addActionViews((LinearLayout) view, list);
        }
    }

    public static ControlAction.OnActionClick turnOnControlClick = new ControlAction.OnActionClick() {
        @Override
        public void call(ControlFragment fragment, ControlAction action, Connection connection, NetworkThread thread) {
            Helper.sendWoLMagicPacket(connection.getIp(), connection.getMac());
        }
    };

    public static ControlAction.OnActionClick basicControlClick = new ControlAction.OnActionClick() {

        @Override
        public void call(final ControlFragment fragment, ControlAction action, Connection connection, NetworkThread thread) {
            String message;
            switch (action) {
                case POWER_OFF:      message = "TURN_OFF"; break;
                case RESTART:       message = "RESTART"; break;
                case LOCK:          message = "LOCK"; break;
                case SLEEP:         message = "SLEEP"; break;
                default:            message = null; break;
            }

            if (message != null) {
                thread.setIp(connection.getIp());
                thread.sendMessage(message, new NetworkThread.OnMessageReceived() {
                    @Override
                    public void messageReceived(String message) {
                        if (message.equals("OK")) {
                            Toast.makeText(fragment.getActivity(), "Successful!", Toast.LENGTH_LONG).show();
                        } else
                            Toast.makeText(fragment.getActivity(), "Error!", Toast.LENGTH_LONG).show();
                    }
                });
            } else
                Toast.makeText(fragment.getActivity(), "No response specified for action `" + action.name() + "`", Toast.LENGTH_LONG).show();

        }
    };

    public static ControlAction.OnActionClick torrentControlClick = new ControlAction.OnActionClick() {
        @Override
        public void call(final ControlFragment fragment, final ControlAction action, final Connection connection, final NetworkThread thread) {
            new HttpTask(fragment.getActivity(), "http://www.serialzone.cz/watchlist/", new HttpTask.OnHandled() {
                @Override
                public void call(List<String> list) {
                    if (list == null)
                        return;

                    thread.setIp(connection.getIp());
                    final AlertDialog.Builder builder = new AlertDialog.Builder(fragment.getActivity());
                    builder.setTitle("Data").setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            dialogInterface.dismiss();
                        }
                    });

                    final AlertDialog dialog = builder.create();
                    //currentMessage = "";

                    StringBuilder sb = new StringBuilder();
                    sb.append("TORRENT ");
                    for (String ser : list) {
                        ser = ser.replaceAll("\\.", " ");
                        ser = ser.replaceAll("-", " ");
                        sb.append("\""+ser+"\", ");
                    }

                    thread.sendMessage(sb.toString(), new NetworkThread.OnMessageReceived() {
                        @Override
                        public void messageReceived(String message) {
                            /*if (!currentMessage.equals(""))
                                currentMessage += "\n";

                            currentMessage += message;

                            dialog.setMessage(currentMessage);
                            if (!dialog.isShowing())
                                dialog.show();*/
                        }
                    });
                }
            }).execute();
        }
    };

    public static ControlAction.OnActionClick volumeControlClick = new ControlAction.OnActionClick() {
        @Override
        public void call(final ControlFragment fragment, ControlAction action, Connection connection, final NetworkThread thread) {
            thread.setIp(connection.getIp());
            thread.sendMessage("GET_VOLUME", new NetworkThread.OnMessageReceived() {
                @Override
                public void messageReceived(String message) {
                    float value = Float.valueOf(message);
                    AlertDialog.Builder builder = new AlertDialog.Builder(fragment.getActivity());

                    LayoutInflater inflater = (LayoutInflater) fragment.getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                    final View view = inflater.inflate(R.layout.dialog_seek, null);

                    final TextView nameField = (TextView) view.findViewById(R.id.text1);
                    final TextView valueField = (TextView) view.findViewById(R.id.text2);
                    nameField.setText("Current volume:");
                    SeekBar bar = (SeekBar) view.findViewById(R.id.seek);
                    bar.setMax(1000);
                    final LimitedExecutor runnable = new LimitedExecutor(100);
                    bar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                        @Override
                        public void onProgressChanged(final SeekBar seekBar, final int i, boolean b) {
                            runnable.execute(new Runnable() {
                                @Override
                                public void run() {
                                    valueField.setText(String.valueOf(i/10.f) + "%");
                                    float outValue = seekBar.getProgress()/1000.f;
                                    thread.sendMessage("SET_VOLUME " + String.valueOf(outValue), NetworkThread.doNothingOnMessage);
                                }
                            });
                        }

                        @Override
                        public void onStartTrackingTouch(SeekBar seekBar) {

                        }

                        @Override
                        public void onStopTrackingTouch(SeekBar seekBar) {
                            valueField.setText(String.valueOf(seekBar.getProgress()/10.f) + "%");
                            float outValue = seekBar.getProgress()/1000.f;
                            thread.sendMessage("SET_VOLUME " + String.valueOf(outValue), NetworkThread.doNothingOnMessage);
                        }
                    });
                    bar.setProgress((int) (value * 1000));
                    builder.setTitle("Set Volume")
                        .setView(view)
                        .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                dialogInterface.dismiss();
                            }
                        }).create().show();;
                }
            });
        }
    };
}
