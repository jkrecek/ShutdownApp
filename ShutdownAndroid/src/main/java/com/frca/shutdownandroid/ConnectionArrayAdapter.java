package com.frca.shutdownandroid;

import android.content.Context;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class ConnectionArrayAdapter extends TwoLineArrayAdapter<Connection> {
    public ConnectionArrayAdapter(Context context, Connection[] connections) {
        super(context, connections);
    }

    @Override
    public String getFirstLine(Connection connection) {
        return connection.getIp();
    }

    @Override
    public String getSecondLine(Connection connection) {
        return connection.getMac();
    }
}
