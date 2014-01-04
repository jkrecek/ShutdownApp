package com.frca.shutdownandroid.network;

import android.text.TextUtils;
import android.util.Log;
import android.util.SparseArray;

import com.frca.shutdownandroid.Helpers.Helper;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class NetworkTask extends Thread implements Runnable {

    private String ipAddress;

    private Socket socket;

    private List<Command> sendBuffer = Collections.synchronizedList(new ArrayList<Command>());

    private SparseArray<Command> waitingCommands = new SparseArray<Command>();

    private OnNetworkTaskEnd onEnd;

    public NetworkTask(String ipAddress, OnNetworkTaskEnd onEnd) {
        this.ipAddress = ipAddress;
        this.onEnd = onEnd;
    }

    @Override
    public void run() {
        try {
            socket = NetworkThread.createSocket(ipAddress);

            sendPacket(NetworkThread.createPacket("type=ANDROID user=frca pass=superdupr"));

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
        /*output.writeInt(packet.getId());
        output.writeBytes(packet.getContent());
        output.write('\n');*/
        output.write(bytes);
        output.flush();
    }

    private void readPacket(final Packet packet) {
        final Command command = waitingCommands.get(packet.getId(), null);
        if (command == null) {
            Log.i("Network", "RCV_E: " + String.valueOf(packet.getId()) + ", `" + packet.getContent() + "` ... not found");
            return;
        }

        Log.i("Network", "RCV[" + packet.getId() + "]: " + packet.getContent());
        Helper.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (command.getMessageReceived() != null)
                    command.getMessageReceived().messageReceived(packet.getContent());
            }
        });
    }

    public void respondExceptions(final IOException e) {
        Helper.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Command command;
                for (int idx = 0; idx < waitingCommands.size(); ++ idx) {
                    command = waitingCommands.valueAt(idx);
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

    public String getIpAddress() {
        return ipAddress;
    }

    public interface OnNetworkTaskEnd {
        void onEnd(NetworkTask task);
    }
}
