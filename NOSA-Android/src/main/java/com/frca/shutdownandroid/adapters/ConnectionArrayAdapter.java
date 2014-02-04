package com.frca.shutdownandroid.adapters;

import android.content.Context;
import android.text.TextUtils;

import com.frca.shutdownandroid.classes.Connection;
import com.frca.shutdownandroid.classes.DirectConnection;
import com.frca.shutdownandroid.classes.ProxyConnection;

import org.w3c.dom.Text;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class ConnectionArrayAdapter extends TwoLineArrayAdapter<Connection> {
    public ConnectionArrayAdapter(Context context, Connection[] connections) {
        super(context, connections);
    }

    @Override
    public String getFirstLine(Connection connection) {
        if (!TextUtils.isEmpty(connection.getHostname()))
            return connection.getHostname();
        else
            return connection.getStringIdentifier();
    }

    @Override
    public String getSecondLine(Connection connection) {
        if (!TextUtils.isEmpty(connection.getHostname()))
            return connection.getStringIdentifier();
        else
            return "";
    }
}
