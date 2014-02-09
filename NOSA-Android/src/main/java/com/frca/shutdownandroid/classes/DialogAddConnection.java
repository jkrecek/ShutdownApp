package com.frca.shutdownandroid.classes;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.frca.shutdownandroid.Helpers.Helper;
import com.frca.shutdownandroid.MainActivity;
import com.frca.shutdownandroid.R;

public abstract class DialogAddConnection {
    public enum AddAction {
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
                    originalDialog.dismiss();
                    Context context = view.getContext();
                    if (context instanceof MainActivity)
                        listener.onClick((MainActivity) context);
                }
            };
        }
    }

    public static AlertDialog displayDialog(Context context) {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
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

    public static boolean checkProxyConnectionInfo(EditText username, EditText password) {
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

    public static interface OnDialogClickListener {
        public void onClick(MainActivity activity);
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
                        if (DialogAddConnection.checkProxyConnectionInfo(userName, password)) {
                            ProxyConnection connection = new ProxyConnection(
                                userName.getText().toString(),
                                Helper.hashPassword(password.getText().toString())
                            );

                            activity.establishInitialProxyConnection(connection);
                        }
                    }
                })
                .setNegativeButton(android.R.string.cancel, MainActivity.dismissListener)
                .create()
                .show();
        }
    };
}
