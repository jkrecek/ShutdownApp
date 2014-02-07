package com.frca.shutdownandroid;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.adapters.ConnectionArrayAdapter;
import com.frca.shutdownandroid.classes.Connection;
import com.frca.shutdownandroid.classes.ConnectionList;
import com.frca.shutdownandroid.classes.ProxyConnection;
import com.frca.shutdownandroid.fragments.MainFragment;
import com.frca.shutdownandroid.network.NetworkThread;

public class MainActivity extends Activity implements ActionBar.OnNavigationListener {
    private static final String NAVIGATION_IDX = "navi_idx";

    private NetworkThread thread;

    private ConnectionList connections;

    private MainFragment currentFragment;

    private boolean running = false;

    private enum AddAction {
        DIRECT(R.drawable.network_connection_w, R.string.add_direct, addDirectListener),
        PROXY(R.drawable.proxy_server_add, R.string.add_proxy, addProxyListener);

        private int iconRes;
        private int textRes;
        private OnDialogClickListener listener;
        private AddAction(int iconRes, int textRes, OnDialogClickListener listener) {
            this.iconRes = iconRes;
            this.textRes = textRes;
            this.listener = listener;
        }

        public int getIconRes() {
            return iconRes;
        }

        public int getTextRes() {
            return textRes;
        }

        public View.OnClickListener getListener() {
            return new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    AlertDialog originalDialog = (AlertDialog) ((View)view.getParent()).getTag();
                    originalDialog.dismiss();;
                    Context context = view.getContext();
                    if (context instanceof MainActivity)
                        listener.onClick((MainActivity) context);
                }
            };
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        running = true;

        setContentView(R.layout.activity_main);

        connections = ConnectionList.loadFromPrefs(getPreferences(this));

        if (connections.isEmpty())
            getActionBar().setTitle("Add connection");
        else {
            reloadAdapter(false);

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
                showRequestCredentialsDialog();
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
        final ActionBar actionBar = getActionBar();
        if (actionBar == null)
            return;

        Connection[] conns = connections.toArray();
        ConnectionArrayAdapter adapter = new ConnectionArrayAdapter(this, conns);
        actionBar.setListNavigationCallbacks(adapter, this);
        actionBar.setDisplayOptions(ActionBar.DISPLAY_SHOW_HOME);
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_LIST);

        if (isUpdate) {
            connections.saveToPrefs(getPreferences(this));
            actionBar.setSelectedNavigationItem(conns.length -1);
        }
    }

    private AlertDialog showRequestCredentialsDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        LayoutInflater inflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        final LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.dialog_multiline, null);

        for (AddAction type : AddAction.values()) {
            final View view = inflater.inflate(R.layout.dialog_line_item, null);
            ((ImageView)view.findViewById(R.id.icon)).setImageResource(type.getIconRes());
            ((TextView)view.findViewById(R.id.text)).setText(type.getTextRes());
            view.setOnClickListener(type.getListener());
            layout.addView(view);
        }

        builder.setTitle("Select connection type")
            .setView(layout)
            .setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    dialogInterface.dismiss();
                }
            });

        AlertDialog d = builder.create();
        d.show();
        layout.setTag(d);
        return d;

    }

    /*private void displayEditText() {
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
    }*/

    private EditText getActionBarEditText() {
        View view = getActionBar().getCustomView();
        if (view != null) {
            view = view.findViewById(R.id.edit);
            if (view != null && view instanceof EditText)
                return (EditText) view;
        }

        return null;
    }

    /*private void onEditTextConfirm(EditText editText) {
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

                            reloadAdapter(true);
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
    }*/

    public NetworkThread getThread() {
        if (thread == null)
            thread = new NetworkThread(this);

        return thread;
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
        getPreferences(this).edit().putInt(NAVIGATION_IDX, i).commit();
        //Fragment fragment = getFragmentManager().findFragmentById(R.id.container); ??? TODO
        Connection connection = (Connection) connections.valueAt(i);
        currentFragment = MainFragment.newInstance(connection.getGeneratedId());
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

    public boolean checkProxyConnectionInfo(EditText username, EditText password) {
        String strUser = username.getText().toString();
        String strPass = password.getText().toString();

        if (TextUtils.isEmpty(strUser)) {
            username.setError("You must enter valid username");
            return false;
        }

        if (strUser.length() < 4) {
            username.setError("Your username must be at least 5 characters long");
            return false;
        }

        if (TextUtils.isEmpty(strPass)) {
            password.setError("You must enter valid password");
            return false;
        }

        if (strPass.length() < 5) {
            password.setError("Your password must be at least 5 characters long");
            return false;
        }
        return true;
    }

    public void establishInitialProxyConnection(final ProxyConnection connection) {
        NetworkThread nt = getThread();
        nt.sendMessage(connection, "STATUS", new NetworkThread.OnMessageReceived() {
                @Override
                public void messageReceived(final String response) {
                    if (response.equals("ONLINE")) {
                        connections.add(connection);
                        reloadAdapter(true);
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

    public static SharedPreferences getPreferences(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context);
        //return context.getSharedPreferences(MainActivity.class.getSimpleName(), Context.MODE_PRIVATE);
    }

    public Connection getConnection(int generatedId) {
        return (Connection) connections.get(generatedId);
    }

    public Fragment getCurrentFragment() {
        return getFragmentManager().findFragmentById(R.id.container);
    }

    private static OnDialogClickListener addDirectListener = new OnDialogClickListener() {
        @Override
        public void onClick(MainActivity activity) {

        }
    };

    private static OnDialogClickListener addProxyListener = new OnDialogClickListener() {
        @Override
        public void onClick(final MainActivity activity) {
            LayoutInflater inflater = (LayoutInflater) activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            final LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.dialog_username_password, null);
            new AlertDialog.Builder(activity)
                .setTitle("Set proxy connection")
                .setView(layout)
                .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        EditText userName = (EditText)layout.findViewById(R.id.edit1);
                        EditText password = (EditText)layout.findViewById(R.id.edit2);
                        if (activity.checkProxyConnectionInfo(userName, password)) {
                            ProxyConnection connection = new ProxyConnection(
                                userName.getText().toString(),
                                Helper.hashPassword(password.getText().toString())
                            );

                            activity.establishInitialProxyConnection(connection);
                        }
                    }
                })
                .setNegativeButton(android.R.string.cancel, dismissListener)
                .create()
                .show();
        }
    };

    public static DialogInterface.OnClickListener dismissListener = new DialogInterface.OnClickListener() {
        @Override
        public void onClick(DialogInterface dialogInterface, int i) {
            dialogInterface.dismiss();
        }
    };

    public static interface OnDialogClickListener {
        public void onClick(MainActivity activity);
    }
}
