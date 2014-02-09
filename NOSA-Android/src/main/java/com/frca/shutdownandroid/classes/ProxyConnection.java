package com.frca.shutdownandroid.classes;

import com.frca.shutdownandroid.network.NetworkThread;

public class ProxyConnection extends Connection{

    private String username;
    private String passwordHash;
    private String localIp = "";

    public ProxyConnection(String username, String passwordHash) {
        super(Connection.ConnectionType.PROXY);
        this.username = username;
        this.passwordHash = passwordHash;
    }

    @Override
    public void loadInfo(NetworkThread thread, final Runnable onUpdatedInfo) {
        thread.sendMessage(this, "GET_INFO", new NetworkThread.OnMessageReceived() {
            @Override
            public void messageReceived(String message) {
                if (message.startsWith("ERROR"))
                    return;

                NVMap map = new NVMap();
                String[] args = message.split(" ");
                map.appendPairs(args, '=');

                String value;
                value = map.getString("host", null);
                if (value != null)
                    hostname = value;

                value = map.getString("mac", null);
                if (value != null)
                    mac = value;

                value = map.getString("ip", null);
                if (value != null)
                    localIp = value;

                onUpdatedInfo.run();
            }
        });
    }

    @Override
    public String getStringIdentifier() {
        return username;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getPasswordHash() {
        return passwordHash;
    }

    public void setPasswordHash(String passwordHash) {
        this.passwordHash = passwordHash;
    }

    public String getLocalIp() {
        return localIp;
    }

    public void setLocalIp(String localIp) {
        this.localIp = localIp;
    }
}
