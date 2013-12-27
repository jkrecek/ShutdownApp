package com.frca.shutdownandroid;

import android.app.ActionBar;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Patterns;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.adapters.ConnectionArrayAdapter;
import com.frca.shutdownandroid.classes.Connection;
import com.frca.shutdownandroid.fragments.MainFragment;
import com.frca.shutdownandroid.network.NetworkThread;

import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class MainActivity extends Activity implements ActionBar.OnNavigationListener {

    private static final String KEY_CONNECTIONS = "connections";

    private static final String NAVIGATION_IDX = "navi_idx";

    private NetworkThread thread;

    private List<Connection> connections = new ArrayList<Connection>();

    private MainFragment currentFragment;

    private boolean running = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        running = true;

        setContentView(R.layout.activity_main);

        loadConnections();

        if (connections.isEmpty())
            getActionBar().setTitle("Add connection");
        else {
            displayAdapter(false);

            int idx = savedInstanceState != null ? savedInstanceState.getInt(NAVIGATION_IDX) : 0;
            if (idx == 0)
                idx = getPreferences(this).getInt(NAVIGATION_IDX, 0);

            getActionBar().setSelectedNavigationItem(idx);
        }
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

        outState.putInt(NAVIGATION_IDX, getActionBar().getSelectedNavigationIndex());
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_add: {
                EditText editText = getActionBarEditText();
                if (editText == null)
                    displayEditText();
                else
                    onEditTextConfirm(editText);
                return true;
            }
        }

        return super.onOptionsItemSelected(item);
    }

    private void displayAdapter(boolean isUpdate) {
        final ActionBar actionBar = getActionBar();
        actionBar.setCustomView(null);
        Connection[] conns = connections.toArray(new Connection[connections.size()]);
        ConnectionArrayAdapter adapter = new ConnectionArrayAdapter(this, conns);
        actionBar.setListNavigationCallbacks(adapter, this);
        actionBar.setDisplayOptions(ActionBar.DISPLAY_SHOW_HOME);
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_LIST);

        if (isUpdate)
            saveConnections();
    }

    private void displayEditText() {
        final ActionBar actionBar = getActionBar();

        actionBar.setCustomView(R.layout.edit_text);
        final EditText edit = (EditText) actionBar.getCustomView().findViewById(R.id.edit);
        edit.setHint("Ip Address (e.g. 192.168.0.30)");
        edit.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                onEditTextConfirm((EditText) v);
                return false;
            }
        });

        edit.requestFocus();
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
                imm.showSoftInput(edit, InputMethodManager.SHOW_IMPLICIT);
            }
        }, 200);


        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_STANDARD);
        actionBar.setDisplayOptions(ActionBar.DISPLAY_SHOW_HOME | ActionBar.DISPLAY_SHOW_CUSTOM);
    }

    private EditText getActionBarEditText() {
        View view = getActionBar().getCustomView();
        if (view != null) {
            view = view.findViewById(R.id.edit);
            if (view != null && view instanceof EditText)
                return (EditText) view;
        }

        return null;
    }

    private void onEditTextConfirm(EditText editText) {
        final String inputIp = editText.getText().toString();
        if (TextUtils.isEmpty(inputIp)) {
            editText.setError("Enter new IP Address");
            return;
        }

        if (inputIp.length() < 7 && inputIp.length() > 15) {
            editText.setError("Wrong length");
            return;
        }

        int count = 0;
        for (int i=0; i < inputIp.length(); i++)
            if (inputIp.charAt(i) == '.')
                ++count;

        if (count != 3) {
            editText.setError("Wrong format");
            return;
        }

        if (!Patterns.IP_ADDRESS.matcher(inputIp).matches()) {
            editText.setError("Wrong format");
            return;
        }

        for (Connection con : connections) {
            if (con.getIp() == inputIp) {
                editText.setError("Connection with such IP already exists.");
                return;
            }
        }

        InputMethodManager im = (InputMethodManager)getSystemService(INPUT_METHOD_SERVICE);
        im.hideSoftInputFromWindow(editText.getWindowToken(), 0);

        NetworkThread nt = getThread();
        nt.sendMessage(inputIp, "GET_MAC " + inputIp, new NetworkThread.OnMessageReceived() {
                @Override
                public void messageReceived(final String responseMAC) {
                    if (responseMAC.length() != 17) {
                        Helper.showDialog(MainActivity.this, "Error", "Unexpected length of response");
                        return;
                    }

                    if (!NetworkThread.MAC_ADDRESS.matcher(responseMAC).matches()) {
                        Helper.showDialog(MainActivity.this, "Error", "Unexpected format of response");
                        return;
                    }

                    new AsyncTask<Void, Void, String>() {
                        @Override
                        protected String doInBackground(Void... voids) {
                            return new InetSocketAddress(inputIp, NetworkThread.SERVER_PORT).getHostName();
                        }

                        @Override
                        protected void onPostExecute(String s) {
                            Connection connection = null;
                            for (Connection con : connections) {
                                if (con.getIp() == inputIp) {
                                    connection = con;
                                    break;
                                }
                            }
                            String hostname = s.substring(0, s.indexOf("."));
                            if (connection != null) {
                                connection.setMac(responseMAC);
                                connection.setHostname(hostname);
                            } else {
                                connections.add(new Connection(inputIp, responseMAC, hostname));
                            }

                            displayAdapter(true);
                        }
                    }.execute();
                }
            }, new NetworkThread.OnExceptionReceived() {
                @Override
                public void exceptionReceived(Exception e) {
                    Helper.showDialog(MainActivity.this, "Error", "Couldn't connect to to '" + inputIp + "'.");
                }
            }
        );
    }

    public NetworkThread getThread() {
        if (thread == null)
            thread = new NetworkThread(this);

        return thread;
    }

    private void loadConnections() {
        connections.clear();
        Set<String> conn_set = getPreferences(MODE_PRIVATE).getStringSet(KEY_CONNECTIONS, null);
        if (conn_set == null)
            return;

        for (String s : conn_set) {
            String parts[] = s.split("\n");
            connections.add(new Connection(parts[0], parts[1], parts[2]));
        }
    }

    private void saveConnections() {
        SharedPreferences prefs = getPreferences(MODE_PRIVATE);
        Set<String> conn_set = new HashSet<String>();
        for (Connection con : connections)
            conn_set.add(con.getIp() + "\n" + con.getMac() + "\n" + con.getHostname());

        prefs.edit().putStringSet(KEY_CONNECTIONS, conn_set).commit();
    }

    private TextView getChildText(LinearLayout parent) {
        for (int a = 0; a < parent.getChildCount(); ++a) {
            View view = parent.getChildAt(a);
            if (view instanceof TextView)
                return (TextView) view;
        }

        return null;
    }

    @Override
    public boolean onNavigationItemSelected(int i, long l) {
        MainActivity.getPreferences(this).edit().putInt(NAVIGATION_IDX, i).commit();
        Fragment fragment = getFragmentManager().findFragmentById(R.id.container);
        Connection connection = connections.get(i);
        currentFragment = MainFragment.newInstance(connection.getIp());
        getThread().setConnection(connection);

        if (running)
            setFragment(currentFragment);


        return false;
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

    public static SharedPreferences getPreferences(Context context) {
        return context.getSharedPreferences(context.getApplicationInfo().name, Context.MODE_PRIVATE);
    }

    public Connection getConnectionForIp(String ip) {
        if (ip == null)
            return null;

        for (Connection connection : connections)
            if (connection.getIp().equals(ip))
                return connection;

        return null;
    }

    public Fragment getCurrentFragment() {
        return getFragmentManager().findFragmentById(R.id.container);
    }
}
