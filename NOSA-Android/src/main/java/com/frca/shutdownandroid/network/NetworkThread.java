package com.frca.shutdownandroid.network;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.MainActivity;
import com.frca.shutdownandroid.R;
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

    public static final String LOCALHOST = "localhost";
    public static final Pattern HOST_ADDRESS = Pattern.compile("^(?:([\\w\\-])*\\.)?([\\w\\-]+)\\.([a-zA-Z]{2,4})$");
    public static final Pattern MAC_ADDRESS = Pattern.compile("^([0-9A-F]{2}[:-]){5}([0-9A-F]{2})$");
    public static final Pattern IP_ADDRESS = Pattern.compile("^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." + "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
            "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." + "([01]?\\d\\d?|2[0-4]\\d|25[0-5])$");

    private Connection connection;
    private Context context;
    private static int currentPacketId = 0;

    private Map<Integer, NetworkTask> activeTask = Collections.synchronizedMap(new HashMap<Integer, NetworkTask>());

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
        run(getConnectIp(connection), connection, command);
    }

    public void sendMessage(Connection connection, String message) {
        sendMessage(connection, message, defaultMessageReceiver);
    }

    public void sendMessage(Connection connection, String message, OnMessageReceived messageReceived) {
        sendMessage(connection, message, messageReceived, defaultExceptionReceiver);
    }

    public void sendMessage(Connection connection, String message, OnMessageReceived messageReceived, OnExceptionReceived exceptionReceived) {
        Command command = new Command(createPacket(message), messageReceived, exceptionReceived);
        run(getConnectIp(connection), connection, command);
    }

    public NetworkTask run(String ipAddress, Connection connection, Command command) {
        NetworkTask task;
        if (activeTask.containsKey(connection.getGeneratedId()))
            task = activeTask.get(connection.getGeneratedId());
        else {
            task = new NetworkTask(ipAddress, connection, onTaskEndInstance);
            activeTask.put(connection.getGeneratedId(), task);
        }

        task.addCommand(command);
        if (!task.isAlive() && !task.isInterrupted())
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
            Log.e("blabla", "test");
            Log.e("size", Integer.toString(activeTask.size()));
            activeTask.remove(task.getConnection().getGeneratedId());
            Log.e("size", Integer.toString(activeTask.size()));
        }
    };

    public static Packet createPacket(String message) {
        return new Packet(++currentPacketId, message);
    }

    public void pingConnection(Connection connection, final Connection.PingResult resultCallback) {
        sendMessage(connection, "STATUS", new OnMessageReceived() {
            @Override
            public void messageReceived(String response) {
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
            return MainActivity.getPreferences(context).getString(
                context.getString(R.string.edit_text_host_key),
                context.getString(R.string.default_host)
            );
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
