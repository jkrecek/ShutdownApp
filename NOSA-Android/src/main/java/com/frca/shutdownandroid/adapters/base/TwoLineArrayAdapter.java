package com.frca.shutdownandroid.adapters.base;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public abstract class TwoLineArrayAdapter<T> extends ArrayAdapter<T> {
    private int mListItemLayoutResId;

    public TwoLineArrayAdapter(Context context, T[] ts) {
        this(context, android.R.layout.two_line_list_item, ts);
    }

    public TwoLineArrayAdapter(Context context, int listItemLayoutResourceId, T[] ts) {
        super(context, listItemLayoutResourceId, ts);
        mListItemLayoutResId = listItemLayoutResourceId;
    }

    @Override
    public View getView( int position, View convertView, ViewGroup parent) {
        LayoutInflater inflater = (LayoutInflater)getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        View listItemView = convertView;
        if (listItemView == null) {
            listItemView = inflater.inflate(mListItemLayoutResId, parent, false);
        }

        TextView firstView = (TextView)listItemView.findViewById(android.R.id.text1);
        TextView secondView = (TextView)listItemView.findViewById(android.R.id.text2);

        T t = getItem(position);
        firstView.setText(getFirstLine(t));
        secondView.setText(getSecondLine(t));

        return listItemView;
    }

    @Override
    public View getDropDownView(int position, View convertView, ViewGroup parent) {
        return getView(position, convertView, parent);
    }

    public abstract String getFirstLine(T t);

    public abstract String getSecondLine(T t);
}