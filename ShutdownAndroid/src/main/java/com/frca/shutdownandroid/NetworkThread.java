package com.frca.shutdownandroid;

import android.text.TextUtils;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.regex.Pattern;

/**
 * Created by KillerFrca on 28.11.13.
 */
public class NetworkThread /*extends Thread implements Runnable */ {

    public static final int SERVER_PORT = 3691;
    public static final int CONNECT_TIMEOUT = 2000;
    public static final int STREAM_TIMEOUT = 15000;

    public static final Pattern MAC_ADDRESS = Pattern.compile("^([0-9A-F]{2}[:-]){5}([0-9A-F]{2})$");

    private OnMessageReceived defaultMessageListener;
    private OnExceptionReceived defaultExceptionReceived;
    private Socket socket;
    private BufferedReader input;
    private DataOutputStream output;

    private String ip;

    public NetworkThread(OnMessageReceived messageListener, OnExceptionReceived exceptionReceived) {
        defaultMessageListener = messageListener;
        defaultExceptionReceived = exceptionReceived;
    }

    public Command sendMessage(String message) {
        return sendMessage(message, defaultMessageListener);
    }

    public Command sendMessage(String message, OnMessageReceived messageReceived) {
        return sendMessage(message, messageReceived, defaultExceptionReceived);
    }

    public Command sendMessage(String message, OnMessageReceived messageReceived, OnExceptionReceived exceptionReceived) {
        Command command = new Command(message, messageReceived, exceptionReceived);
        NetworkTask.start(ip, command);
        return command;
    }

    public void setIp(String ip) {
        this.ip = ip;
    }

    public String getIp() {
        return ip;
    }

    public static Socket createSocket(String ipAddress) throws IOException {
        Log.i("Network", "Sock: Connecting");

        long cur = System.currentTimeMillis();
        Socket socket = new Socket();
        if (TextUtils.isEmpty(ipAddress))
            throw new IOException("No Ip Address!");

        socket.connect(new InetSocketAddress(ipAddress, NetworkThread.SERVER_PORT), NetworkThread.CONNECT_TIMEOUT);
        socket.setSoTimeout(NetworkThread.STREAM_TIMEOUT);

        Log.i("Network", "M: Connected in " + String.valueOf(System.currentTimeMillis() - cur) + " milis.");
        return socket;
    }

    public static Socket destroySocket(Socket socket) throws IOException {
        if (socket != null) {
            socket.close();
            socket = null;
            Log.i("Network", "Sock: Destroyed");
        }

        return socket;
    }

    public void pingConnection(Connection connection, final Connection.PingResult resultCallback) {
        sendMessage("GET_MAC " + connection.getIp(), new OnMessageReceived() {
            @Override
            public void messageReceived(String responseMAC) {
                boolean success = true;
                if (!NetworkThread.MAC_ADDRESS.matcher(responseMAC).matches())
                    success = false;

                resultCallback.result(success);
            }
        }, new OnExceptionReceived() {
            @Override
            public void exceptionReceived(Exception e) {
                resultCallback.result(false);
            }
        });
    }

    public interface OnMessageReceived {
        public void messageReceived(String message);
    }

    public interface OnExceptionReceived {
        public void exceptionReceived(Exception e);
    }

    public static class Command {
        private String command;

        private OnMessageReceived messageReceived = null;
        private OnExceptionReceived exceptionReceived = null;

        private Command(String command, OnMessageReceived messageReceived, OnExceptionReceived exceptionReceived) {
            this.command = command;
            this.messageReceived = messageReceived;
            this.exceptionReceived = exceptionReceived;
        }

        public String getCommand() {
            return command;
        }

        public OnMessageReceived getMessageReceived() {
            return messageReceived;
        }

        public OnExceptionReceived getExceptionReceived() {
            return exceptionReceived;
        }
    }
}
