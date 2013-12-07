package com.frca.shutdownandroid.classes;

import com.frca.shutdownandroid.network.NetworkThread;

public class Connection {
    private String ip;
    private String mac;

    private long lastCheckedTime = System.currentTimeMillis();

    public Connection(String ip, String mac) {
        this.ip = ip;
        this.mac = mac;
    }

    public String getIp() {
        return ip;
    }

    public String getMac() {
        return mac;
    }

    public void setIp(String ip) {
        this.ip = ip;
    }

    public void setMac(String mac) {
        this.mac = mac;
    }

    public void isOnline(NetworkThread thread, PingResult result) {
        if (System.currentTimeMillis() - lastCheckedTime < 30000)
            result.result(true);

        thread.setIp(getIp());
        thread.pingConnection(this, result);
    }

    public interface PingResult {
        public void result(boolean success);
    }
}