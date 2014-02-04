package com.frca.shutdownandroid.classes;

import com.frca.shutdownandroid.network.NetworkThread;

import java.io.Serializable;

public class Connection implements Serializable {

    public enum ConnectionType {
        DIRECT(DirectConnection.class),
        PROXY(ProxyConnection.class);

        private Class<? extends Connection> conClass;
        private ConnectionType(Class<? extends Connection> conClass) {
             this.conClass = conClass;
        }

        public static ConnectionType fromString(String string) {
            for (ConnectionType type : values())
                if (type.toString().equals(string))
                    return type;

            return null;
        }

        public Class<? extends Connection> getConClass() {
            return conClass;
        }

        public Class<? extends Connection> getConArrayClass() {
            try {
                return (Class<? extends Connection>) Class.forName("[L" + conClass.getName());
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
            return null;
        }
    };

    private int generatedId;
    private static int idBuffer = 0;
    protected String mac = "";
    protected String hostname = "";

    private long lastCheckedTime;

    private ConnectionType type;

    protected Connection(ConnectionType type) {
        generatedId = idBuffer++;
        this.type = type;
        this.lastCheckedTime = 0;
    }

    public int getGeneratedId() {
        return generatedId;
    }

    public String getMac() {
        return mac;
    }

    public void setMac(String mac) {
        this.mac = mac;
    }

    public String getHostname() {
        return hostname;
    }

    public void setHostname(String hostname) {
        this.hostname = hostname;
    }

    public ConnectionType getType() {
        return type;
    }

    public static void setIdBuffer(int idBuffer) {
        Connection.idBuffer = idBuffer;
    }

    public void isOnline(NetworkThread thread, PingResult result) {
        if (System.currentTimeMillis() - lastCheckedTime < 30000)
            result.result(true);
        else
            thread.pingConnection(this, result);
    }

    public String getStringIdentifier() { return  null; }

    public interface PingResult {
        public void result(boolean success);
    }
}