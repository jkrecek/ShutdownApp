package com.frca.shutdownandroid.network.http;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.net.http.AndroidHttpClient;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;

import com.frca.shutdownandroid.MainActivity;
import com.frca.shutdownandroid.R;

import org.apache.http.Header;
import org.apache.http.HttpEntity;
import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.HttpRequest;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpRequestBase;
import org.apache.http.client.params.HttpClientParams;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;
import org.jsoup.Jsoup;
import org.jsoup.helper.StringUtil;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by KillerFrca on 6.12.13.
 */
public class HttpTask extends AsyncTask<Void, Void, List<String>> {

    public final static String USER_AGENT = "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36";

    /*public static final String KEY_USERNAME = "utorrent_username";

    public static final String KEY_PASSWORD = "utorrent_password";*/

    private static final String URL_LOGIN = "http://www.serialzone.cz/prihlasovani/";

    private Context context;

    private String baseUrl;

    private String username;

    private String password;

    private List<String> cookies = new ArrayList<String>();

    private OnHandled callback;

    private AndroidHttpClient httpClient = AndroidHttpClient.newInstance(USER_AGENT);

    public HttpTask(Context context, String baseUrl, OnHandled callback) {
        this.context = context;
        this.baseUrl = baseUrl;
        this.callback = callback;

        HttpClientParams.setRedirecting(httpClient.getParams(), false);
    }

    @Override
    protected List<String> doInBackground(Void... voids) {
        try {
            setCredentialCookies();

            Elements elements = getShowsElements();

            return parseShows(elements);

        } catch (ClientProtocolException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            httpClient.close();
        }

        return null;
    }

    private HttpResponse execute(HttpRequestBase requestBase) throws IOException {
        dumpRequest(requestBase);
        HttpResponse response = httpClient.execute(requestBase);
        dumpResponse(response);
        return response;
    }

    private void setCredentialCookies() throws IOException {
        getCredentials();

        HttpResponse response = execute(getPost(URL_LOGIN));

        if (response.getStatusLine().getStatusCode() != 302) {
            MainActivity.getPreferences(context)
                .edit()
                .remove(context.getString(R.string.edit_text_utorrent_username))
                .remove(context.getString(R.string.edit_text_utorrent_password))
                .commit();

            response.getEntity().consumeContent();
            setCredentialCookies();
            return;
        }

        Header[] headers = response.getHeaders("Set-Cookie");
        cookies.clear();
        int counter = 0;
        for (Header header : headers)
        {
            if (++counter > 2) {
                String cookie = header.getValue();
                cookie = cookie.substring(0, cookie.indexOf(";"));
                cookies.add(cookie);
            }
        }

        response.getEntity().consumeContent();
    }

    private Elements getShowsElements() throws IOException {
        HttpResponse response = execute(getGet(baseUrl));
        HttpEntity entity = response.getEntity();
        String text = EntityUtils.toString(entity);;
        entity.consumeContent();

        Document doc = Jsoup.parse(text);
        return doc.body().select("div#disc-big-main-topic > *");
    }

    private List<String> parseShows(Elements elements) {
        if (elements == null)
            return null;

        List<String> series = new ArrayList<String>();
        for (Element element : elements) {
            if (element.hasClass("cleaner"))
                break;

            if (element.hasClass("watchlist")) {
                Element info = element.select("li.watchinfo span:first-of-type span").first();
                if (info.hasClass("past") || info.text().equals("dnes")) {
                    String name = element.select("li.watchname > a").text();
                    name = name.replaceAll("(.*), The", "The $1");
                    series.add(name);
                }
            }
        }

        return series;
    }

    @Override
    protected void onPostExecute(List<String> strings) {
        if (strings == null)
            Log.i(getClass().getSimpleName(), "Strings are null");

        callback.call(strings);
    }

    private HttpGet getGet(String url) {
        HttpGet request = new HttpGet(url);
        request.setHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
        request.setHeader("Host", "www.serialzone.cz");
        request.setHeader("Accept-Language", "cs-CZ,en;q=0.8");
        request.setHeader("Connection", "keep-alive");
        request.setHeader("User-Agent", USER_AGENT);
        if (!cookies.isEmpty())
            request.setHeader("Cookie", StringUtil.join(cookies, "; "));

        return request;
    }

    private HttpPost getPost(String url) {
        HttpPost post = new HttpPost(url);
        post.setHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
        post.setHeader("Host", "www.serialzone.cz");
        post.setHeader("Accept-Language", "cs-CZ,en;q=0.8");
        post.setHeader("Connection", "keep-alive");
        post.setHeader("User-Agent", USER_AGENT);

        List<NameValuePair> urlParameters = new ArrayList<NameValuePair>();
        urlParameters.add(new BasicNameValuePair("login_name", username));
        urlParameters.add(new BasicNameValuePair("login_pass", password));
        urlParameters.add(new BasicNameValuePair("login_auto", "ano"));

        try {
            post.setEntity(new UrlEncodedFormEntity(urlParameters, "utf-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        return post;
    }

    public static void dumpRequest(HttpRequest request) {
        StringBuilder sb = new StringBuilder();
        sb.append("Dumping http request\n");
        sb.append(request.getRequestLine().getMethod() + " " + request.getRequestLine().getUri() + " " + request.getRequestLine().getProtocolVersion().toString() + "\n");

        for (Header header : request.getAllHeaders())
            sb.append(header.getName() + ": " + header.getValue() + "\n");

        if (request instanceof HttpEntityEnclosingRequest) {
            HttpEntity entity = ((HttpEntityEnclosingRequest) request).getEntity();
            if (entity.getContentEncoding() != null)
                sb.append(entity.getContentEncoding().getName() + ": " + entity.getContentEncoding().getValue() + "\n");
            if (entity.getContentLength() != 0L)
                sb.append("Content-Length: " + String.valueOf(entity.getContentLength()) + "\n");
            if (entity.getContentType() != null)
                sb.append(entity.getContentType().getName() + ": " + entity.getContentType().getValue() + "\n");

            try {
                sb.append("Content: " + EntityUtils.toString(entity) + "\n");
            } catch (IOException e) {
                Log.e("ERROR_DUMPING", "Error:" + e.getMessage());
                e.printStackTrace();
            }
        }

        Log.d("REQUEST_DUMP", sb.toString());
    }

    public static void dumpResponse(HttpResponse response) {
        StringBuilder sb = new StringBuilder();
        sb.append("Dumping http response\n");
        sb.append("Status code is: " + response.getStatusLine().getStatusCode() + "\n");
        for (Header header : response.getAllHeaders())
            sb.append(header.getName()  + ": " + header.getValue() + "\n");


        Log.d("RESPONSE_DUMP", sb.toString());
    }

    private void getCredentials() {
        username = MainActivity.getPreferences(context).getString(context.getString(R.string.edit_text_utorrent_username), null);
        password = MainActivity.getPreferences(context).getString(context.getString(R.string.edit_text_utorrent_password), null);

        if (username == null || password == null) {
            new Handler(Looper.getMainLooper()).post(new Runnable() {
                @Override
                public void run() {
                    showRequestCredentialsDialog();
                }
            });

            while (username == null || password == null) {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private AlertDialog showRequestCredentialsDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        final View view = inflater.inflate(R.layout.dialog_username_password, null);

        builder.setTitle("Serialzone.cz")
            .setView(view)
            .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    EditText usernameView = (EditText) view.findViewById(R.id.edit1);
                    EditText passwordView = (EditText) view.findViewById(R.id.edit2);

                    if (TextUtils.isEmpty(usernameView.getText())) {
                        usernameView.setError("You must input username.");
                        return;
                    }

                    if (TextUtils.isEmpty(passwordView.getText())) {
                        passwordView.setError("You must input password.");
                        return;
                    }

                    username = usernameView.getText().toString();
                    password = passwordView.getText().toString();

                    MainActivity.getPreferences(context)
                        .edit()
                        .putString(context.getString(R.string.edit_text_utorrent_username), username)
                        .putString(context.getString(R.string.edit_text_utorrent_password), password)
                        .commit();
                }
            }).setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    username = "-1";
                    password = "-1";
                    dialogInterface.dismiss();
                }
            });

        AlertDialog d = builder.create();
        d.show();
        return d;

    }

    public interface OnHandled {
        public void call(List<String> list);
    }
}
