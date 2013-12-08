package com.frca.shutdownandroid.classes;

import com.frca.shutdownandroid.network.NetworkThread;

public class Connection {

    private String ip;
    private String mac;
    private String hostname;

    private long lastCheckedTime;

    public Connection(String ip, String mac, String hostname) {
        this.ip = ip;
        this.mac = mac;
        this.hostname = hostname;
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

    public String getHostname() {
        return hostname;
    }

    public void setHostname(String hostname) {
        this.hostname = hostname;
    }

    public void isOnline(NetworkThread thread, PingResult result) {
        if (System.currentTimeMillis() - lastCheckedTime < 30000)
            result.result(true);
        else
            thread.pingConnection(this, result);
    }

    public interface PingResult {
        public void result(boolean success);
    }
}