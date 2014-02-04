package com.frca.shutdownandroid.classes;

/**
 * Created by KillerFrca on 31.1.14.
 */
public class ProxyConnection extends Connection{

    private String username;
    private String passwordHash;
    private String localIp = "";

    public ProxyConnection(String username, String passwordHash) {
        super(Connection.ConnectionType.PROXY);
        this.username = username;
        this.passwordHash = passwordHash;
    }

    public String getUsername() {
        return username;
    }

    @Override
    public String getStringIdentifier() {
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
