package com.frca.shutdownandroid.network;

import android.content.Context;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.classes.Command;
import com.frca.shutdownandroid.classes.Connection;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.regex.Pattern;

/**
 * Created by KillerFrca on 28.11.13.
 */
public class NetworkThread /*extends Thread implements Runnable */ {

    public static final int SERVER_PORT = 3691;
    public static final int CONNECT_TIMEOUT = 2000;
    public static final int STREAM_TIMEOUT = 15000;

    public static final Pattern MAC_ADDRESS = Pattern.compile("^([0-9A-F]{2}[:-]){5}([0-9A-F]{2})$");

    private Connection connection;
    private Context context;

    public NetworkThread(Context context) {
        this.context = context;
    }

    public void sendMessage(String message) {
        sendMessage(message, defaultMessageReceiver);
    }

    public void sendMessage(String message, OnMessageReceived messageReceived) {
        sendMessage(message, messageReceived, defaultExceptionReceiver);
    }

    public void sendMessage(String message, OnMessageReceived messageReceived, OnExceptionReceived exceptionReceived) {
        Command command = new Command(message, messageReceived, exceptionReceived);
        run(connection.getIp(), command);
    }

    public void sendMessage(String ipAddress, String message) {
        sendMessage(ipAddress, message, defaultMessageReceiver);
    }

    public void sendMessage(String ipAddress, String message, OnMessageReceived messageReceived) {
        sendMessage(ipAddress, message, messageReceived, defaultExceptionReceiver);
    }

    public void sendMessage(String ipAddress, String message, OnMessageReceived messageReceived, OnExceptionReceived exceptionReceived) {
        Command command = new Command(message, messageReceived, exceptionReceived);
        run(ipAddress, command);
    }

    public static NetworkTask run(String ipAddress, Command command) {
        NetworkTask task = new NetworkTask(ipAddress, command);
        task.start();
        return task;
    }


    public static Socket createSocket(String ipAddress) throws IOException {
        Log.i("Network", "Sock: Connecting");

        long cur = System.currentTimeMillis();
        Socket socket = new Socket();
        if (TextUtils.isEmpty(ipAddress))
            throw new IOException("No Ip Address!");

        socket.connect(new InetSocketAddress(ipAddress, NetworkThread.SERVER_PORT), NetworkThread.CONNECT_TIMEOUT);
        socket.setSoTimeout(NetworkThread.STREAM_TIMEOUT);
        final DataOutputStream output = new DataOutputStream(socket.getOutputStream());
        output.writeBytes("type=ANDROID user=frca pass=superdupr" + '\n');
        output.flush();

        Log.i("Network", "M: Connected in " + String.valueOf(System.currentTimeMillis() - cur) + " milis.");
        return socket;
    }

    public static Socket closeSocket(Socket socket) throws IOException {
        if (socket != null) {
            final DataOutputStream output = new DataOutputStream(socket.getOutputStream());
            output.writeBytes("CLOSE" + '\n');
            output.flush();

            socket.close();
            socket = null;
            Log.i("Network", "Sock: Closed");
        }

        return socket;
    }

    public void pingConnection(Connection connection, final Connection.PingResult resultCallback) {
        sendMessage(connection.getIp(), "GET_MAC " + connection.getIp(), new OnMessageReceived() {
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

    public void setConnection(Connection connection) {
        this.connection = connection;
    }

    public Connection getConnection() {
        return connection;
    }

    public interface OnMessageReceived {
        public void messageReceived(String message);
    }

    public interface OnExceptionReceived {
        public void exceptionReceived(Exception e);
    }

    public static OnMessageReceived doNothingOnMessage = new OnMessageReceived() { @Override public void messageReceived(String message) { } };
    public static OnExceptionReceived doNothingOnException = new OnExceptionReceived() { @Override public void exceptionReceived(Exception e) { } };

    private OnMessageReceived defaultMessageReceiver = new OnMessageReceived() {
        @Override
        public void messageReceived(String message) {
            Helper.showDialog(context, "Response", message);
        }
    };

    private OnExceptionReceived defaultExceptionReceiver = new OnExceptionReceived() {
        @Override
        public void exceptionReceived(Exception e) {
            if (e instanceof SocketTimeoutException) {
                Helper.showDialog(context, "Connection error", "App could not connect to host `" + getConnection().getIp() + "`, port `" + NetworkThread.SERVER_PORT + "`" );
                e.printStackTrace();
                return;
            }

            Helper.showDialog(context, "Unhandled exception", Helper.getStackTrace(e));
        }
    };
}
