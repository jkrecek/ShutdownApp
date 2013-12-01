package com.frca.shutdownandroid.fragments;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.frca.shutdownandroid.Connection;
import com.frca.shutdownandroid.MainActivity;
import com.frca.shutdownandroid.NetworkThread;
import com.frca.shutdownandroid.R;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class OfflineFragment extends ChildFragment {

    public OfflineFragment(Connection connection) {
        super(connection);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.fragment_offline, container, false);
        for (int i = 0; i < layout.getChildCount(); ++i) {
            LinearLayout child = (LinearLayout) layout.getChildAt(i);
            if (child == null)
                continue;

            child.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    switch (view.getId()) {
                        case R.id.turn_on:
                            wakeUp(connection.getIp(), connection.getMac());
                            return;
                    }

                    Toast.makeText(getActivity(), "No action specified for view id `" + String.valueOf(view.getId()) + "`", Toast.LENGTH_LONG).show();
                }
            });
        }

        return layout;
    }

    private void wakeUp(final String broadcastIp, final String macAddress)
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    byte[] macBytes = getMacBytes(macAddress);
                    byte[] bytes = new byte[6 + 16 * macBytes.length];
                    for (int i = 0; i < 6; i++) {
                        bytes[i] = (byte) 0xff;
                    }
                    for (int i = 6; i < bytes.length; i += macBytes.length) {
                        System.arraycopy(macBytes, 0, bytes, i, macBytes.length);
                    }

                    InetAddress address = InetAddress.getByName(broadcastIp);
                    DatagramPacket packet = new DatagramPacket(bytes, bytes.length, address, 9);
                    DatagramSocket socket = new DatagramSocket();
                    socket.send(packet);
                    socket.close();

                    packet = new DatagramPacket(bytes, bytes.length, address, 7);
                    socket = new DatagramSocket();
                    socket.send(packet);
                    socket.close();

                    Log.e("WAKE_UP", "Wake-on-LAN packet sent.");
                    final String output = getString(bytes);
                    Log.i("WAKE_UP", output);
                    new Handler(Looper.getMainLooper()).post(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(MainActivity.context, output, Toast.LENGTH_LONG).show();
                        }
                    });
                }
                catch (Exception e) {
                    Log.e("WAKE_UP", "Failed to send Wake-on-LAN packet: " + e);
                    Toast.makeText(getActivity(), "Failed to send Wake-on-LAN packet: " + e, Toast.LENGTH_LONG).show();
                }
            }
        }).start();
    }

    private static byte[] getMacBytes(String macStr) throws IllegalArgumentException {
        byte[] bytes = new byte[6];
        String[] hex = macStr.split("(\\:|\\-)");
        if (hex.length != 6) {
            throw new IllegalArgumentException("Invalid MAC address.");
        }
        try {
            for (int i = 0; i < 6; i++) {
                bytes[i] = (byte) Integer.parseInt(hex[i], 16);
            }
        }
        catch (NumberFormatException e) {
            throw new IllegalArgumentException("Invalid hex digit in MAC address.");
        }
        return bytes;
    }

    private static String getString(byte[] bytes) {
        StringBuilder sb = new StringBuilder(bytes.length * 3);
        for (byte b : bytes) {
            String hex = Integer.toHexString(0xFF & b);
            if (hex.length() == 1)
                hex = '0' + hex;

            sb.append(hex + ' ');
        }

        return sb.toString();
    }
}
