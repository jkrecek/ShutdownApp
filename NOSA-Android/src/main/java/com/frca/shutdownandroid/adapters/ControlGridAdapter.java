package com.frca.shutdownandroid.adapters;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.frca.shutdownandroid.R;
import com.frca.shutdownandroid.classes.ControlAction;

import java.util.List;

public class ControlGridAdapter extends ArrayAdapter<ControlAction> {

    private int resourceId;

    public ControlGridAdapter(Context context, int resourceId, ControlAction[] actions) {
        super(context, resourceId, actions);
        this.resourceId = resourceId;
    }

    public ControlGridAdapter(Context context, int resourceId, List<ControlAction> list) {
        super(context, resourceId, list);
        this.resourceId = resourceId;
    }

    @Override
    public View getView( int position, View convertView, ViewGroup parent) {
        LayoutInflater inflater = (LayoutInflater)getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        View listItemView = convertView;
        if (listItemView == null) {
            listItemView = inflater.inflate(resourceId, parent, false);
        }

        ControlAction action = getItem(position);
        ImageView imageView = (ImageView)listItemView.findViewById(R.id.image);
        TextView textView = (TextView)listItemView.findViewById(R.id.text);

        imageView.setImageResource(action.getDrawable());
        textView.setText(action.getText());

        return listItemView;
    }
}
