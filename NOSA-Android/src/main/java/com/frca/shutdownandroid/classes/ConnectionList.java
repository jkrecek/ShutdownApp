package com.frca.shutdownandroid.classes;

import android.content.SharedPreferences;
import android.util.Log;
import android.util.SparseArray;

import java.lang.reflect.Type;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;

public class ConnectionList<T extends Connection> extends SparseArray<T> {

    public static final String KEY_CONNECTIONS = "connection";

    private static Gson gsonDeserializationInstance;

    public ConnectionList() {
        super();
    }

    private static Gson getGsonDeserializationInstance() {
        if (gsonDeserializationInstance == null) {
            GsonBuilder gsonBilder = new GsonBuilder();
            gsonBilder.registerTypeAdapter(Connection.class, new JsonDeserializer<Connection>() {

                @Override
                public Connection deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext jsonDeserializationContext) throws JsonParseException {
                    final JsonObject member = (JsonObject) jsonElement;
                    final JsonElement typeElem = member.get("type");
                    type = Connection.ConnectionType.fromString(typeElem.getAsString()).getConClass();

                    return jsonDeserializationContext.deserialize(jsonElement, type);
                }
            });
            gsonBilder.setPrettyPrinting();
            gsonDeserializationInstance = gsonBilder.create();
        }

        return gsonDeserializationInstance;
    }

    public static ConnectionList loadFromPrefs(SharedPreferences preferences) {
        ConnectionList list = new ConnectionList();
        Gson gson = getGsonDeserializationInstance();
        String values = preferences.getString(KEY_CONNECTIONS, null);
        if (values != null) {
            Connection[] array = gson.fromJson(values, Connection[].class);
            list = new ConnectionList(array);
            Connection lastConnection = array[array.length - 1];
            Connection.setIdBuffer(lastConnection.getGeneratedId() + 1);
        }

        return list;
    }

    public ConnectionList(T[] array) {
        for(int i = 0; i < array.length; ++i)
            add(array[i]);
    }

    public void add(T connection) {
        put(connection.getGeneratedId(), connection);
    }

    public void saveToPrefs(SharedPreferences preferences) {
        Gson gson = new Gson();
        String str = gson.toJson(toArray());
        preferences.edit().putString(KEY_CONNECTIONS, gson.toJson(toArray())).commit();
    }

    public Connection[] toArray() {
        Connection array[] = new Connection[size()];
        for(int i = 0; i < size(); ++i) {
            array[i] = valueAt(i);
        }

        return array;
    }

    public boolean isEmpty() {
        return size() == 0;
    }
}
