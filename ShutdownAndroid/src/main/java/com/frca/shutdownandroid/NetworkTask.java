package com.frca.shutdownandroid;

import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.Iterator;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class NetworkTask extends Thread implements Runnable {

    private NetworkThread.Command command;

    private String ipAddress;
    private Socket socket;

    public static NetworkTask start(String ipAddress, NetworkThread.Command command) {
        NetworkTask task = new NetworkTask();
        task.ipAddress = ipAddress;
        task.command = command;
        task.start();
        return task;
    }

    @Override
    public void run() {
        try {
            socket = NetworkThread.createSocket(ipAddress);

            DataOutputStream output = new DataOutputStream(socket.getOutputStream());

            Log.i(getClass().getSimpleName(), "Send: " + command.getCommand());
            output.writeBytes(command.getCommand() + '\n');
            output.flush();

            BufferedReader input;
            while (true) {
                input = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                if (input.ready()) {
                    String line;
                    while ((line = input.readLine()) != null) {
                        final String response = line.trim();
                        if (!TextUtils.isEmpty(line)) {
                            Log.i(getClass().getSimpleName(), "Read: " + line);
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    command.getMessageReceived().messageReceived(response);
                                }
                            });
                        }
                    }
                    break;
                }

                try {
                    sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        } catch (final IOException e) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    command.getExceptionReceived().exceptionReceived(e);
                }
            });
        } finally {
            try {
                NetworkThread.destroySocket(socket);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void runOnUiThread(Runnable run) {
        new Handler(Looper.getMainLooper()).post(run);
    }
}
