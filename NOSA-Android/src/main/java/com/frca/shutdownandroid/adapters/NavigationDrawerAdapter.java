package com.frca.shutdownandroid.adapters;

import android.content.Context;

import com.frca.shutdownandroid.R;
import com.frca.shutdownandroid.adapters.base.IconTwoLineArrayAdapter;
import com.frca.shutdownandroid.classes.Connection;

public class NavigationDrawerAdapter extends IconTwoLineArrayAdapter<Connection> {

    public NavigationDrawerAdapter(Context context, Connection[] connections) {
        super(context, connections);
    }

    @Override
    public int getIconDrawable(Connection connection) {
        if (connection.getType() == Connection.ConnectionType.DIRECT)
            return R.drawable.network_connection_w;
        else
            return R.drawable.proxy_server_add;
    }

    @Override
    public String getFirstLine(Connection connection) {
        return connection.getStringIdentifier();
    }

    @Override
    public String getSecondLine(Connection connection) {
        return connection.getHostname();
    }
}