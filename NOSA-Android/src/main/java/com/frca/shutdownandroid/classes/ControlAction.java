package com.frca.shutdownandroid.classes;

import com.frca.shutdownandroid.R;
import com.frca.shutdownandroid.fragments.ControlFragment;
import com.frca.shutdownandroid.fragments.MainFragment;
import com.frca.shutdownandroid.network.NetworkThread;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by KillerFrca on 7.12.13.
 */
public enum ControlAction {
    POWER_ON(R.drawable.restart, "Turn on", ControlFragment.turnOnControlClick, ConnectionState.OFFLINE),
    POWER_OFF(R.drawable.turn_off, "Shutdown", ControlFragment.basicControlClick, ConnectionState.ONLINE),
    RESTART(R.drawable.restart, "Restart", ControlFragment.basicControlClick, ConnectionState.ONLINE),
    LOCK(R.drawable.log_off, "Lock", ControlFragment.basicControlClick, ConnectionState.ONLINE),
    SLEEP(R.drawable.sleep, "Sleep", ControlFragment.basicControlClick, ConnectionState.ONLINE),
    TORRENT(R.drawable.torrent, "Torrent", ControlFragment.torrentControlClick, ConnectionState.ONLINE),
    VOLUME(R.drawable.volume, "Volume", ControlFragment.volumeControlClick, ConnectionState.ONLINE),
    PACKET(R.drawable.terminal, "Packet", ControlFragment.packetControlClick, ConnectionState.ONLINE);

    public enum ConnectionState {
        ONLINE,
        OFFLINE
    }

    private int drawable;
    private String text;
    private OnActionClick callback;
    private ConnectionState state;

    private ControlAction(int drawable, String text, OnActionClick callback, ConnectionState state) {
        this.drawable = drawable;
        this.text = text;
        this.callback = callback;
        this.state = state;
    }

    public static List<ControlAction> getValuesWithState(ConnectionState state) {
        List<ControlAction> list = new ArrayList<ControlAction>();
        for (ControlAction action : ControlAction.values())
            if (state == action.state)
                list.add(action);

        return list;
    }

    public int getDrawable() {
        return drawable;
    }

    public String getText() {
        return text;
    }

    public OnActionClick getCallback() {
        return callback;
    }

    public ConnectionState getState() {
        return state;
    }

    public interface OnActionClick {
        void call(final MainFragment fragment, final ControlAction action, final NetworkThread thread);
    }
}
