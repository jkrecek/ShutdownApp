package com.frca.shutdownandroid.network;

public class Command {
    private Packet packet;

    private NetworkThread.OnMessageReceived messageReceived = null;
    private NetworkThread.OnExceptionReceived exceptionReceived = null;

    public Command(Packet packet, NetworkThread.OnMessageReceived messageReceived, NetworkThread.OnExceptionReceived exceptionReceived) {
        this.packet = packet;
        this.messageReceived = messageReceived;
        this.exceptionReceived = exceptionReceived;
    }

    public Packet getPacket() {
        return packet;
    }

    public NetworkThread.OnMessageReceived getMessageReceived() {
        return messageReceived;
    }

    public NetworkThread.OnExceptionReceived getExceptionReceived() {
        return exceptionReceived;
    }
}
