package com.frca.shutdownandroid.classes;

import com.frca.shutdownandroid.network.NetworkThread;

/**
 * Created by KillerFrca on 7.12.13.
 */
public class Command {
    private String command;

    private NetworkThread.OnMessageReceived messageReceived = null;
    private NetworkThread.OnExceptionReceived exceptionReceived = null;

    public Command(String command, NetworkThread.OnMessageReceived messageReceived, NetworkThread.OnExceptionReceived exceptionReceived) {
        this.command = command;
        this.messageReceived = messageReceived;
        this.exceptionReceived = exceptionReceived;
    }

    public String getCommand() {
        return command;
    }

    public NetworkThread.OnMessageReceived getMessageReceived() {
        return messageReceived;
    }

    public NetworkThread.OnExceptionReceived getExceptionReceived() {
        return exceptionReceived;
    }
}
