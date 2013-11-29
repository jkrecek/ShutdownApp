package com.frca.shutdownandroid;

import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

/**
 * Created by KillerFrca on 28.11.13.
 */
public class NetworkThread extends Thread implements Runnable  {

    private String serverMessage;
    public static final String SERVERIP = "10.0.0.44";
    public static final int SERVERPORT = 3691;
    private static final int TIMEOUT = 5000;
    private OnMessageReceived mMessageListener = null;
    private OnExceptionReceived mExceptionReceived = null;
    private Socket socket;
    private BufferedReader input;
    private DataOutputStream output;

    private long lastTimeSocketUsed;

    private List<String> commands = Collections.synchronizedList(new ArrayList<String>());

    public NetworkThread(OnMessageReceived messageListener, OnExceptionReceived exceptionReceived) {
        mMessageListener = messageListener;
        mExceptionReceived = exceptionReceived;

    }

    public void sendMessage(String message){
        synchronized (commands) {
            commands.add(message);
        }

        if (!isAlive())
            start();
    }

    @Override
    public void run() {
        while(true) {
            try {
                if (socket != null) {
                    input = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                    if (input.ready()) {
                        String line;
                        while ((line = input.readLine()) != null) {
                            Log.i(getClass().getSimpleName(), "Read: " + line);
                            mMessageListener.messageReceived(line);
                        }

                        destroySocket();
                        lastTimeSocketUsed = System.currentTimeMillis();
                    }
                }

                synchronized (commands) {
                    if (!commands.isEmpty()) {
                        if (socket == null)
                            createSocket();

                        output = new DataOutputStream(socket.getOutputStream());

                        for (Iterator<String> itr = commands.iterator(); itr.hasNext(); ) {
                            String command = itr.next();
                            Log.i(getClass().getSimpleName(), "Send: " + command);
                            output.writeBytes(command + '\n');
                            output.flush();
                            itr.remove();
                        }

                        lastTimeSocketUsed = System.currentTimeMillis();
                    }
                }

                if ((System.currentTimeMillis() - lastTimeSocketUsed) > 60000)
                    destroySocket();

                sleep(500);

            } catch (Exception e) {
                mExceptionReceived.exceptionReceived(e);
                commands.clear();

                try {
                    destroySocket();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }

            }
        }

    }

    private void createSocket() throws IOException {
        Log.i(getClass().getName(), "Sock: Connecting");

        long cur = System.currentTimeMillis();
        socket = new Socket();
        socket.connect(new InetSocketAddress(SERVERIP, SERVERPORT), TIMEOUT);
        socket.setSoTimeout(TIMEOUT);

        Log.i(getClass().getName(), "M: Connected in " + String.valueOf(System.currentTimeMillis() - cur) + " milis.");
        lastTimeSocketUsed = System.currentTimeMillis();
    }

    private void destroySocket() throws IOException {
        if (socket != null) {
            socket.close();
            socket = null;
            Log.i(getClass().getSimpleName(), "Sock: Destroyed");
        }

    }

    public interface OnMessageReceived {
        public void messageReceived(String message);
    }

    public interface OnExceptionReceived {
        public void exceptionReceived(Exception e);
    }
}
