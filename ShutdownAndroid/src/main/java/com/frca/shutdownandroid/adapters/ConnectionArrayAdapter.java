package com.frca.shutdownandroid.adapters;

import android.content.Context;

import com.frca.shutdownandroid.classes.Connection;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class ConnectionArrayAdapter extends TwoLineArrayAdapter<Connection> {
    public ConnectionArrayAdapter(Context context, Connection[] connections) {
        super(context, connections);
    }

    @Override
    public String getFirstLine(Connection connection) {
        return connection.getHostname();
    }

    @Override
    public String getSecondLine(Connection connection) {
        return connection.getIp();
    }
}
