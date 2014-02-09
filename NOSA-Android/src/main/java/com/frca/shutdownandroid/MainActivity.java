package com.frca.shutdownandroid;

import android.app.ActionBar;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.app.ActionBarDrawerToggle;
import android.support.v4.widget.DrawerLayout;
import android.text.TextUtils;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.adapters.NavigationDrawerAdapter;
import com.frca.shutdownandroid.classes.Connection;
import com.frca.shutdownandroid.classes.ConnectionList;
import com.frca.shutdownandroid.classes.DialogAddConnection;
import com.frca.shutdownandroid.classes.ProxyConnection;
import com.frca.shutdownandroid.fragments.MainFragment;
import com.frca.shutdownandroid.network.NetworkThread;

public class MainActivity extends Activity {

    private static final String SELECTED_CONNECTION = "selected_connection";

    private NetworkThread thread;

    private ConnectionList connections;

    private MainFragment currentFragment;

    private DrawerLayout mDrawerLayout;

    private ListView mDrawerList;

    private ActionBarDrawerToggle mDrawerToggle;

    private boolean running = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        running = true;

        setContentView(R.layout.activity_main);

        mDrawerList = (ListView) findViewById(R.id.left_drawer);

        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);

        connections = ConnectionList.loadFromPrefs(getPreferences(this));

        if (connections.isEmpty())
            getActionBar().setTitle("Add connection");
        else {
            reloadAdapter(false);

            int generatedId = savedInstanceState != null ? savedInstanceState.getInt(SELECTED_CONNECTION) : getPreferences(this).getInt(SELECTED_CONNECTION, 0);

            Connection connection = getConnection(generatedId);
            if (connection != null)
                displayConnectionFragment(connection);
        }

        setDrawer();

        final ActionBar bar = getActionBar();
        if (bar != null) {
            bar.setDisplayHomeAsUpEnabled(true);
            bar.setHomeButtonEnabled(true);
        }

        checkConnectionInfo();
    }

    protected void setDrawer() {
        mDrawerList.setOnItemClickListener(new ListView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int position, long id) {

                Connection connection = (Connection) adapterView.getAdapter().getItem(position);
                displayConnectionFragment(connection);
                mDrawerList.setItemChecked(position, true);
                mDrawerLayout.closeDrawer(mDrawerList);
            }
        });

        mDrawerToggle = new ActionBarDrawerToggle(this, mDrawerLayout, R.drawable.ic_drawer, R.string.drawer_open, R.string.drawer_close) {

            public void onDrawerClosed(View view) {
                ActionBar actionBar = getActionBar();
                if (actionBar != null) {
                    if (currentFragment != null && currentFragment.getConnection() != null)
                        actionBar.setTitle(currentFragment.getConnection().getStringIdentifier());
                }
            }

            public void onDrawerOpened(View drawerView) {
                ActionBar actionBar = getActionBar();
                if (actionBar != null)
                    actionBar.setTitle("Select connection");
            }
        };

        mDrawerLayout.setDrawerListener(mDrawerToggle);
    }

    @Override
    protected void onResume() {
        super.onResume();

        running = true;
        Fragment fragment = getFragmentManager().findFragmentById(R.id.container);
        if (fragment == null || !fragment.equals(currentFragment))
            setFragment(currentFragment);
    }

    @Override
    protected void onPause() {
        super.onPause();

        running = false;
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        if (currentFragment != null && currentFragment.getConnection() != null)
            outState.putInt(SELECTED_CONNECTION, currentFragment.getConnection().getGeneratedId());
    }


    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        if (mDrawerToggle != null)
            mDrawerToggle.syncState();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (mDrawerToggle != null)
            mDrawerToggle.onConfigurationChanged(newConfig);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        // If the nav drawer is open, hide action items related to the content view
        if (mDrawerLayout != null) {
            boolean drawerOpen = mDrawerLayout.isDrawerOpen(mDrawerList);
            //menu.findItem(R.id.action_websearch).setVisible(!drawerOpen);
        }

        return super.onPrepareOptionsMenu(menu);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (mDrawerToggle.onOptionsItemSelected(item))
            return true;

        switch (item.getItemId()) {
            case R.id.action_add: {
                DialogAddConnection.displayDialog(this);
                return true;
            }

            case R.id.action_settings: {
                startActivity(new Intent(this, SettingsActivity.class));
                return true;
            }
        }

        return super.onOptionsItemSelected(item);
    }

    private void reloadAdapter(boolean isUpdate) {
        Connection[] connectionsArray = connections.toArray();
        mDrawerList.setAdapter(new NavigationDrawerAdapter(this, connectionsArray));

        if (isUpdate) {
            connections.saveToPrefs();
        }
    }

    public NetworkThread getThread() {
        if (thread == null)
            thread = new NetworkThread(this);

        return thread;
    }

    public void displayConnectionFragment(Connection connection) {
        getPreferences(this).edit().putInt(SELECTED_CONNECTION, connection.getGeneratedId()).commit();
        currentFragment = MainFragment.newInstance(connection.getGeneratedId());
        getThread().setConnection(connection);

        ActionBar actionBar = getActionBar();
        if (actionBar != null)
            actionBar.setTitle(connection.getStringIdentifier());

        if (running)
            setFragment(currentFragment);
    }

    private void setFragment(MainFragment fragment) {
        if (fragment == null)
            return;

        Fragment currentFragment = getFragmentManager().findFragmentById(R.id.container);
        FragmentTransaction trans = getFragmentManager().beginTransaction().replace(R.id.container, fragment);
        if (currentFragment != null && currentFragment instanceof MainFragment)
            trans.addToBackStack(null);

        trans.commit();
    }

    @Override
    public void onBackPressed() {
        finish();
    }

    public void establishInitialProxyConnection(final ProxyConnection connection) {
        NetworkThread nt = getThread();
        nt.sendMessage(connection,
            "STATUS",
            new NetworkThread.OnMessageReceived() {
                @Override
                public void messageReceived(final String response) {
                    if (response.equals("ONLINE")) {
                        connections.add(connection);
                        reloadAdapter(true);
                        displayConnectionFragment(connection);
                    } else if (response.equals("OFFLINE")) {
                        Helper.showDialog(MainActivity.this, "Failure", "No computer is connected on proxy server with such login details.");
                    } else {
                        Helper.showDialog(MainActivity.this, "Failure", "Unexpected response.");
                        Log.e("Proxy connection", "Unexpected response: " + response);
                    }
                }
            }, new NetworkThread.OnExceptionReceived() {
                @Override
                public void exceptionReceived(Exception e) {
                    Helper.showDialog(MainActivity.this, "Error", "We apologize, proxy server seems to be offline.");
                }
            }
        );
    }

    private void checkConnectionInfo() {
        for(int i = 0; i < connections.size(); ++i) {
            Connection connection = (Connection) connections.valueAt(i);
            if (TextUtils.isEmpty(connection.getHostname())) {
                connection.loadInfo(getThread(), getReloadAdapterRunnable());
            }
        }
    }

    private Runnable reloadAdapterRun;

    private Runnable getReloadAdapterRunnable() {
        if (reloadAdapterRun == null) {
            reloadAdapterRun = new Runnable() {
                @Override
                public void run() {
                    reloadAdapter(true);
                }
            };
        }

        return reloadAdapterRun;
    }

    public static SharedPreferences getPreferences(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context);
    }

    public Connection getConnection(int generatedId) {
        return (Connection) connections.get(generatedId);
    }

    public static DialogInterface.OnClickListener dismissListener = new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialogInterface, int i) {
            dialogInterface.dismiss();
        }
    };
}
