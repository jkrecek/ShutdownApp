package com.frca.shutdownandroid.classes;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class NVMap extends HashMap<String, String> {
    public void insertValue(String key, String value) {
        put(key, value);
    }

    public void insertValue(String key, float value) {
        insertValue(key, Float.toString(value));
    }

    public void insertValue(String key, int value)
    {
        insertValue(key, Integer.toString(value));
    }

    public void insertPair(String nameValue, char delim)
    {
        int pos = nameValue.indexOf(delim);
        if (pos == -1)
            return;

        String name = nameValue.substring(0, pos);
        String value = nameValue.substring(pos + 1);
        insertValue(name, value);
    }

    public void appendPairs(String[] vector, char delim)
    {
        for (String line : vector)
            insertPair(line, delim);
    }

    public String getString(String key, String defaultValue)
    {
        String val = get(key);
        if (val != null)
            return val;

        return defaultValue;
    }

    public int getInt(String key, int defaultValue)
    {
        String val = get(key);
        if (val != null)
            return Integer.parseInt(val);

        return defaultValue;
    }

    public float getFloat(String key, float defaultValue)
    {
        String val = get(key);
        if (val != null)
            return Float.parseFloat(val);

        return defaultValue;
    }

    public String toString(char separator)
    {
        StringBuilder sb = new StringBuilder();
        Iterator<Entry<String, String>> itr = entrySet().iterator();
        while (itr.hasNext()) {
            Map.Entry pair = itr.next();
            sb.append(pair.getKey());
            sb.append("=");
            sb.append(pair.getValue());
            if (itr.hasNext())
                sb.append(separator);
        }

        return sb.toString();
    }
}
