package com.frca.shutdownandroid.network;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.MainActivity;
import com.frca.shutdownandroid.classes.Connection;
import com.frca.shutdownandroid.classes.DirectConnection;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;

/**
 * Created by KillerFrca on 28.11.13.
 */
public class NetworkThread /*extends Thread implements Runnable */ {

    public static final String KEY_PROXY_HOST = "proxy_host";
    public static final int SERVER_PORT = 3691;
    public static final int CONNECT_TIMEOUT = 2000;
    public static final int STREAM_TIMEOUT = 15000;

    public static final Pattern MAC_ADDRESS = Pattern.compile("^([0-9A-F]{2}[:-]){5}([0-9A-F]{2})$");

    private Connection connection;
    private Context context;
    private static int currentPacketId = 0;

    private Map<String, NetworkTask> activeTask = Collections.synchronizedMap(new HashMap<String, NetworkTask>());

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
        Command command = new Command(createPacket(message), messageReceived, exceptionReceived);
        run(getConnectIp(connection), command);
    }

    public void sendMessage(Connection connection, String message) {
        sendMessage(connection, message, defaultMessageReceiver);
    }

    public void sendMessage(Connection connection, String message, OnMessageReceived messageReceived) {
        sendMessage(connection, message, messageReceived, defaultExceptionReceiver);
    }

    public void sendMessage(Connection connection, String message, OnMessageReceived messageReceived, OnExceptionReceived exceptionReceived) {
        Command command = new Command(createPacket(message), messageReceived, exceptionReceived);
        run(getConnectIp(connection), command);
    }

    public NetworkTask run(String ipAddress, Command command) {
        NetworkTask task;
        if (activeTask.containsKey(ipAddress))
            task = activeTask.get(ipAddress);
        else {
            task = new NetworkTask(ipAddress, onTaskEndInstance);
            activeTask.put(ipAddress, task);
        }

        task.addCommand(command);
        if (!task.isAlive())
            task.start();

        return task;
    }


    public static Socket createSocket(String ipAddress) throws IOException {
        Log.i("Network", "Sock: Connecting to `" + ipAddress + "`");

        long cur = System.currentTimeMillis();
        Socket socket = new Socket();
        if (TextUtils.isEmpty(ipAddress))
            throw new IOException("No Ip Address!");

        socket.connect(new InetSocketAddress(ipAddress, NetworkThread.SERVER_PORT), NetworkThread.CONNECT_TIMEOUT);
        socket.setSoTimeout(NetworkThread.STREAM_TIMEOUT);

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

    private NetworkTask.OnNetworkTaskEnd onTaskEndInstance = new NetworkTask.OnNetworkTaskEnd() {
        @Override
        public void onEnd(NetworkTask task) {
            activeTask.remove(task.getIpAddress());
        }
    };

    public static Packet createPacket(String message) {
        return new Packet(++currentPacketId, message);
    }

    public void pingConnection(Connection connection, final Connection.PingResult resultCallback) {
        sendMessage(connection, "STATUS", new OnMessageReceived() {
            @Override
            public void messageReceived(String response) {
                // TODO
                /*boolean success = true;

                if (!NetworkThread.MAC_ADDRESS.matcher(responseMAC).matches())
                    success = false;*/

                resultCallback.result(response.equals("ONLINE"));
            }
        }, new OnExceptionReceived() {
            @Override
            public void exceptionReceived(Exception e) {
                resultCallback.result(false);
            }
        });
    }

    private String getConnectIp(Connection connection) {
        if (connection.getType() == Connection.ConnectionType.DIRECT)
            return ((DirectConnection)connection).getIp();
        else {
            return MainActivity.getPreferences(context).getString(KEY_PROXY_HOST, "54.194.222.199");
        }
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
                Helper.showDialog(context, "Connection error", "App could not connect to host `" + getConnection().getStringIdentifier() + "`, port `" + NetworkThread.SERVER_PORT + "`" );
                e.printStackTrace();
                return;
            }

            Helper.showDialog(context, "Unhandled exception", Helper.getStackTrace(e));
        }
    };

}
