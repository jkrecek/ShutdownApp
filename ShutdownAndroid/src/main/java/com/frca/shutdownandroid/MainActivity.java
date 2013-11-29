package com.frca.shutdownandroid;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.os.Build;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;


import org.apache.http.util.ExceptionUtils;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.SocketTimeoutException;

public class MainActivity extends Activity {

    private NetworkThread thread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        LinearLayout layout = (LinearLayout) getLayoutInflater().inflate(R.layout.activity_main, null);
        Display display = getWindowManager().getDefaultDisplay();
        int width = display.getWidth();
        int height = display.getHeight();
        LinearLayout.LayoutParams params;
        //LinearLayout.LayoutParams childParams;
        //int childOrientation;
        if (width > height) {
            layout.setOrientation(LinearLayout.HORIZONTAL);
            //childOrientation = LinearLayout.HORIZONTAL;
            params = new LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.MATCH_PARENT, 1);
            //childParams = new LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.MATCH_PARENT, 1);
        } else {
            layout.setOrientation(LinearLayout.VERTICAL);

            //childOrientation = LinearLayout.VERTICAL;
            params = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 0, 1);
            //childParams = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 0, 1);

        }
        for (int i = 0; i < layout.getChildCount(); ++i) {
            LinearLayout child = (LinearLayout) layout.getChildAt(i);
            child.setLayoutParams(params);
            /*child.setOrientation(childOrientation);
            for (int a = 0; a < child.getChildCount(); ++a) {
                View image = child.getChildAt(a);
                if (image instanceof ImageView) {
                    image.setLayoutParams(childParams);
                    break;
                }
            }*/

            child.setOnClickListener(new View.OnClickListener() {
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
                        Toast.makeText(MainActivity.this, "No action specified for view id `" + String.valueOf(view.getId()) + "`", Toast.LENGTH_LONG).show();
                    else
                        getThread().sendMessage(str);
                }
            });
        }

        layout.setOrientation(width > height ? LinearLayout.HORIZONTAL : LinearLayout.VERTICAL);
        setContentView(layout);
    }

    public NetworkThread getThread() {
        if (thread == null)
            thread = new NetworkThread(messageReceiver, exceptionReceiver);

        return thread;
    }

    private TextView getChildText(LinearLayout parent) {
        for (int a = 0; a < parent.getChildCount(); ++a) {
            View view = parent.getChildAt(a);
            if (view instanceof TextView)
                return (TextView) view;
        }

        return null;
    }

    private NetworkThread.OnMessageReceived messageReceiver = new NetworkThread.OnMessageReceived() {
        @Override
        public void messageReceived(String message) {
            showDialog("Response", message );
        }
    };

    private NetworkThread.OnExceptionReceived exceptionReceiver = new NetworkThread.OnExceptionReceived() {
        @Override
        public void exceptionReceived(Exception e) {
            if (e instanceof SocketTimeoutException) {
                showDialog("Connection error", "App could not connect to host `" + NetworkThread.SERVERIP + "`, port `" + NetworkThread.SERVERPORT + "`" );
                e.printStackTrace();
                return;
            }

            showDialog("Unhandled exception", getStackTrace(e));
        }
    };

    private void showDialog(String title, String message) {
        final AlertDialog.Builder builder = new AlertDialog.Builder(this).setTitle(title).setMessage(message).setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                dialogInterface.dismiss();
            }
        });

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                builder.create().show();
            }
        });

    }

    private String getStackTrace(Throwable t) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        t.printStackTrace(pw);
        return sw.toString();
    }

}
