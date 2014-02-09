package com.frca.shutdownandroid.classes;

import com.frca.shutdownandroid.network.NetworkThread;

public class DirectConnection extends Connection {

    private String ip;

    public DirectConnection(String ip, String mac, String hostname) {
        super(ConnectionType.DIRECT);
        this.ip = ip;
        this.mac = mac;
        this.hostname = hostname;
    }

    @Override
    public void loadInfo(NetworkThread thread, ConnectionList list) {

    }

    @Override
    public String getStringIdentifier() {
        return ip;
    }

    public String getIp() {
        return ip;
    }

    public void setIp(String ip) {
        this.ip = ip;
    }
}
