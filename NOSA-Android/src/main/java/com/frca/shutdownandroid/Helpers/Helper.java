package com.frca.shutdownandroid.Helpers;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.frca.shutdownandroid.MainActivity;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;

/**
 * Created by KillerFrca on 7.12.13.
 */
public abstract class Helper {

    public static void sendWoLMagicPacket(final String broadcastIp, final String macAddress) {
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
                    final String output = getStringFromBytes(bytes);
                    Log.i("WAKE_UP", output);
                }
                catch (Exception e) {
                    Log.e("WAKE_UP", "Failed to send Wake-on-LAN packet: " + e);

                }
            }
        }).start();
    }

    public static byte[] getMacBytes(String macStr) throws IllegalArgumentException {
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

    public static String getStringFromBytes(byte[] bytes) {
        StringBuilder sb = new StringBuilder(bytes.length * 3);
        for (byte b : bytes) {
            String hex = Integer.toHexString(0xFF & b);
            if (hex.length() == 1)
                hex = '0' + hex;

            sb.append(hex + ' ');
        }

        return sb.toString();
    }


    public static void showDialog(Context context, String title, String message) {
        final AlertDialog.Builder builder = new AlertDialog.Builder(context)
            .setTitle(title)
            .setMessage(message)
            .setPositiveButton(android.R.string.ok, MainActivity.dismissListener);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                builder.create().show();
            }
        });
    }

    public static void runOnUiThread(Runnable runnable) {
        new Handler(Looper.getMainLooper()).post(runnable);
    }

    public static String getStackTrace(Throwable t) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        t.printStackTrace(pw);
        return sw.toString();
    }

    public static void simpleSleep(long time) {
        try {
            Thread.sleep(time);
        } catch (InterruptedException ie) {
            ie.printStackTrace();
        }
    }

    public static String hashPassword(String flatPassword) {
        MessageDigest mda = null;
        try {
            mda = MessageDigest.getInstance("SHA-512", "BC");
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (NoSuchProviderException e) {
            e.printStackTrace();
        }
        byte [] digest = mda.digest(flatPassword.getBytes());
        return new String(digest);
    }
}
