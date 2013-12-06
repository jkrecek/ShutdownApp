package com.frca.shutdownandroid.fragments;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.frca.shutdownandroid.Connection;
import com.frca.shutdownandroid.HttpTask;
import com.frca.shutdownandroid.NetworkThread;
import com.frca.shutdownandroid.R;

import java.util.List;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class OnlineFragment extends ChildFragment {

    private String currentMessage;
    public OnlineFragment(Connection connection) {
        super(connection);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.fragment_online, container, false);
        for (int i = 0; i < layout.getChildCount(); ++i) {
            LinearLayout child = (LinearLayout) layout.getChildAt(i);
            if (child == null)
                continue;

            for (int y = 0; y < child.getChildCount(); ++y) {
                View item = child.getChildAt(y);
                if (item == null)
                    continue;

                item.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        final NetworkThread thread = getMainActivity().getThread();
                        String str = getResponseForView(view.getId());

                        if (str != null) {
                            thread.setIp(connection.getIp());
                            thread.sendMessage(str, new NetworkThread.OnMessageReceived() {
                                @Override
                                public void messageReceived(String message) {
                                    if (message.equals("OK")) {
                                        Toast.makeText(getActivity(), "Successful!", Toast.LENGTH_LONG).show();
                                    } else
                                        Toast.makeText(getActivity(), "Error!", Toast.LENGTH_LONG).show();
                                }
                            });
                            return;
                        }

                        if (view.getId() == R.id.torrent) {
                            handleTorrentRequest(thread);
                            return;
                        }

                        Toast.makeText(getActivity(), "No action specified for view id `" + String.valueOf(view.getId()) + "`", Toast.LENGTH_LONG).show();
                    }
                });
            }
        }

        return layout;
    }

    protected String getResponseForView(int resId) {
        switch (resId) {
            case R.id.turn_off: return "TURN_OFF";
            case R.id.restart:  return "RESTART";
            case R.id.lock:     return "LOCK";
            case R.id.sleep:    return "SLEEP";
        }

        return null;
    }

    private void handleTorrentRequest(final NetworkThread thread) {
        new HttpTask(getActivity(), "http://www.serialzone.cz/watchlist/", new HttpTask.OnHandled() {
            @Override
            public void call(List<String> list) {
                if (list == null)
                    return;
                thread.setIp(connection.getIp());
                final AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
                builder.setTitle("Data").setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        dialogInterface.dismiss();
                    }
                });

                final AlertDialog dialog = builder.create();
                currentMessage = "";

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
                        if (!currentMessage.equals(""))
                            currentMessage += "\n";

                        currentMessage += message;

                        dialog.setMessage(currentMessage);
                        if (!dialog.isShowing())
                            dialog.show();
                    }
                });
            }
        }).execute();
    }
}
