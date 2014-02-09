package com.frca.shutdownandroid.network;

import android.text.TextUtils;

import com.frca.shutdownandroid.Helpers.Helper;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;

public class Packet {

    private int id;
    private String content;

    Packet(int id, String content) {
        this.id = id;
        this.content = content;
    }

    public static Packet readPacket(InputStream stream) throws IOException {
        if (stream.available() <= 0)
            return null;

        DataInputStream inputStream = new DataInputStream(stream);
        int packetInt = inputStream.readInt();
        byte buffer[] = new byte[1024];
        byte packetByte;
        int pos = 0;
        do {
            try {
                packetByte = inputStream.readByte();
            } catch (IOException e) {
                Helper.simpleSleep(10);
                continue;
            }

            if (packetByte == '\n')
                break;
            buffer[++pos] = packetByte;
        } while (pos < 1024);

        return new Packet(packetInt, getReceivedString(new String(buffer)));
    }

    private static String getReceivedString(String string) {
        if (TextUtils.isEmpty(string))
            return null;

        string = string.trim();
        if (TextUtils.isEmpty(string))
            return null;

        string = string.replaceAll("\\\\n", "\n");
        return string;
    }

    public int getId() {
        return id;
    }

    public String getContent() {
        return content;
    }

}
