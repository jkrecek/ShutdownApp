package com.frca.shutdownandroid.network;

import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.classes.Command;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class NetworkTask extends Thread implements Runnable {

    private Command command;

    private String ipAddress;

    private Socket socket;

    public NetworkTask(String ipAddress, Command command) {
        this.ipAddress = ipAddress;
        this.command = command;
    }

    @Override
    public void run() {
        try {
            socket = NetworkThread.createSocket(ipAddress);

            DataOutputStream output = new DataOutputStream(socket.getOutputStream());

            Log.i(getClass().getSimpleName(), "Send: " + command.getCommand());
            output.writeBytes(command.getCommand() + '\n');
            output.flush();

            /*try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            output.writeBytes("TEST0" + '\n');
            output.flush();*/

            BufferedReader input;
            while (true) {
                input = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                if (input.ready()) {
                    String line;
                    while ((line = input.readLine()) != null) {
                        if (line.equals("CLOSE"))
                            return;

                        final String response = getReceivedString(line);
                        if (response != null) {
                            Log.i(getClass().getSimpleName(), "Read: " + response);
                            if (command.getMessageReceived() != null) {
                                Helper.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        command.getMessageReceived().messageReceived(response);
                                    }
                                });
                            }
                        }
                    }
                }

                try {
                    sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        } catch (final IOException e) {
            if (command.getExceptionReceived() != null) {
                Helper.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    command.getExceptionReceived().exceptionReceived(e);
                }
            });
            }
        } finally {
            try {
                NetworkThread.closeSocket(socket);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private String getReceivedString(String string) {
        if (TextUtils.isEmpty(string))
            return null;

        string = string.trim();
        if (TextUtils.isEmpty(string))
            return null;

        string = string.replaceAll("\\\\n", "\n");
        return string;
    }
}
