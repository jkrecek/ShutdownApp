package com.frca.shutdownandroid.network;

import android.util.Log;
import android.util.SparseArray;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.classes.Connection;
import com.frca.shutdownandroid.classes.ProxyConnection;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

public class NetworkTask extends Thread implements Runnable {

    private static final int DISCONNECT_INTERVAL = 15000;

    private String ipAddress;

    private Connection connection;

    private Socket socket;

    private List<Command> sendBuffer = Collections.synchronizedList(new ArrayList<Command>());

    private SparseArray<Command> waitingCommands = new SparseArray<Command>();

    private OnNetworkTaskEnd onEnd;

    private long lastCommandHandled;

    public NetworkTask(String ipAddress, Connection connection, OnNetworkTaskEnd onEnd) {
        this.ipAddress = ipAddress;
        this.connection = connection;
        this.onEnd = onEnd;
        this.lastCommandHandled = System.currentTimeMillis();
    }

    @Override
    public void run() {
        try {
            socket = NetworkThread.createSocket(ipAddress);

            login();

            Packet rcvPacket = null;
            while (socket.isConnected()) {
                synchronized (sendBuffer) {
                    if (!sendBuffer.isEmpty()) {
                        Iterator<Command> it = sendBuffer.iterator();
                        while(it.hasNext()) {
                            Command command = it.next();
                            sendPacket(command.getPacket());
                            waitingCommands.put(command.getPacket().getId(), command);
                            it.remove();
                        }
                    }
                }

                try {
                    rcvPacket = Packet.readPacket(socket.getInputStream());
                    if (rcvPacket != null)
                        readPacket(rcvPacket);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                Helper.simpleSleep(100);

                if (waitingCommands.size() == 0) {
                    if ((lastCommandHandled + DISCONNECT_INTERVAL) < System.currentTimeMillis())
                        break;
                } else {
                    lastCommandHandled = System.currentTimeMillis();
                }
            }
        } catch (final IOException e) {
            respondExceptions(e);
        } finally {
            onEnd.onEnd(this);
            try {
                NetworkThread.closeSocket(socket);
            } catch (IOException e) { /* no problem if already closed */ }
        }
    }

    private void login() throws IOException {
        if (connection instanceof ProxyConnection) {
            ProxyConnection proxy = (ProxyConnection)connection;
            String msg = "type=ANDROID user=" + proxy.getUsername() + " pass=" + proxy.getPasswordHash();
            sendPacket(NetworkThread.createPacket(msg));
        }
    }

    private void sendPacket(Packet packet) throws IOException {
        DataOutputStream output = new DataOutputStream(socket.getOutputStream());

        Log.i("Network", "SND[" + packet.getId() + "]: " + packet.getContent());

        byte[] strBytes = packet.getContent().getBytes();
        byte[] bytes = ByteBuffer
            .allocate(4 + strBytes.length + 1)
            .putInt(packet.getId())
            .put(strBytes)
            .put((byte) 10)
            .array();

        output.write(bytes);
        output.flush();
    }

    private void readPacket(final Packet packet) {
        final Command command = waitingCommands.get(packet.getId(), null);
        if (command == null) {
            Log.i("Network", "RCV_E: " + String.valueOf(packet.getId()) + ", `" + packet.getContent() + "` ... not found");
            return;
        }

        if (packet.getContent().trim().equals("CLOSE")) {
            Log.i("Network", "CLS[" + packet.getId() + "]: Requested close");
        } else {
            Log.i("Network", "RCV[" + packet.getId() + "]: " + packet.getContent());
            Helper.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (command.getMessageReceived() != null)
                        command.getMessageReceived().messageReceived(packet.getContent().trim());
                }
            });
        }
    }

    public void respondExceptions(final IOException e) {
        Helper.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                for (int idx = 0; idx < waitingCommands.size(); ++idx) {
                    Command command = waitingCommands.valueAt(idx);
                    if (command.getExceptionReceived() != null)
                        command.getExceptionReceived().exceptionReceived(e);
                }

                for (Command command : sendBuffer) {
                    if (command.getExceptionReceived() != null)
                        command.getExceptionReceived().exceptionReceived(e);
                }
            }
        });
    }

    public void addCommand(Command command) {
        synchronized (sendBuffer) {
            sendBuffer.add(command);
        }
    }

    public Connection getConnection() {
        return connection;
    }

    public String getIpAddress() {
        return ipAddress;
    }

    public interface OnNetworkTaskEnd {
        void onEnd(NetworkTask task);
    }
}
