package com.camera.simplemjpeg;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.net.URI;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.content.Intent;
import android.content.SharedPreferences;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;

public class MjpegActivity extends Activity implements OnClickListener, View.OnTouchListener{
	private static final boolean DEBUG=true;
    private static final String TAG = "MJPEG";

    private MjpegView mv = null;
    String URL;
    
    // for settings (network and resolution)
    private static final int REQUEST_SETTINGS = 0;
    
    private int width = 640;
    private int height = 480;
    
    private int ip_ad1 = 192;
    private int ip_ad2 = 168;
    private int ip_ad3 = 2;
    private int ip_ad4 = 1;
    private int ip_port = 80;
    private String ip_command = "?action=stream";
    public logger logger;
    private boolean suspending = false;

    Button buttonUp;
    Button buttonDown;
    Button buttonRightTurn;
    Button buttonLeftTurn;
    Button buttonConnect;

    private Socket socket;
    private Thread rcvThread;
    private int port = 3002;
    private OutputStream outs;
    private String server = "192.168.1.80";


    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        SharedPreferences preferences = getSharedPreferences("SAVED_VALUES", MODE_PRIVATE);
        width = preferences.getInt("width", width);
        height = preferences.getInt("height", height);
        ip_ad1 = preferences.getInt("ip_ad1", ip_ad1);
        ip_ad2 = preferences.getInt("ip_ad2", ip_ad2);
        ip_ad3 = preferences.getInt("ip_ad3", ip_ad3);
        ip_ad4 = preferences.getInt("ip_ad4", ip_ad4);
        ip_port = preferences.getInt("ip_port", ip_port);
        ip_command = preferences.getString("ip_command", ip_command);
                
        StringBuilder sb = new StringBuilder();
        String s_http = "http://";
        String s_dot = ".";
        String s_colon = ":";
        String s_slash = "/";
        sb.append(s_http);
        sb.append(ip_ad1);
        sb.append(s_dot);
        sb.append(ip_ad2);
        sb.append(s_dot);
        sb.append(ip_ad3);
        sb.append(s_dot);
        sb.append(ip_ad4);
        sb.append(s_colon);
        sb.append(ip_port);
        sb.append(s_slash);
        sb.append(ip_command);
        URL = new String(sb);
        setContentView(R.layout.main);


        mv = (MjpegView) findViewById(R.id.mv);  
        if(mv != null){
        	mv.setResolution(width, height);
        }
        new DoRead().execute(URL);

    }

    public void onResume() {
    	if(DEBUG) Log.d(TAG,"onResume()");
        super.onResume();
        if(mv!=null){
        	if(suspending){
        		new DoRead().execute(URL);
        		suspending = false;
        	}
        }

        buttonUp        = (Button)this.findViewById(R.id.buttonup);
        buttonLeftTurn  = (Button)this.findViewById(R.id.buttonleft);
        buttonRightTurn = (Button)this.findViewById(R.id.buttonright);
        buttonDown      = (Button)this.findViewById(R.id.buttondown);
        buttonConnect   = (Button)this.findViewById(R.id.buttonConnect);

        buttonConnect.setOnClickListener(this);
        buttonUp.setOnTouchListener(this);
        buttonLeftTurn.setOnTouchListener(this);
        buttonRightTurn.setOnTouchListener(this);
        buttonDown.setOnTouchListener(this);
    }

    @Override
    public void onClick(View arg0)
    {
        if(arg0 == buttonConnect) {

            try {
                if (socket != null) {
                    socket.close();
                    socket = null;
                }

                socket = new Socket(server, port);
                outs = socket.getOutputStream();

            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public boolean onTouch(View arg0, MotionEvent event)
    {
        String sndOpkey = "CMD";
        int action = event.getAction();
        int id = arg0.getId();
        if(action == MotionEvent.ACTION_DOWN)
        {
            if (id == R.id.buttonup) {
                Log.d("TAG", "OnTouch : ACTION_DOWN(UP)");
                if(arg0 == buttonUp)	    sndOpkey = "Go";
            }
            if (id == R.id.buttondown) {
                Log.d("TAG", "OnTouch : ACTION_DOWN(Down)");
                if(arg0 == buttonDown)	    sndOpkey = "Back";
            }
            if (id == R.id.buttonleft) {
                Log.d("TAG", "OnTouch : ACTION_DOWN(Left)");
                if(arg0 == buttonLeftTurn)	    sndOpkey = "Left";
            }
            if (id == R.id.buttonright) {
                Log.d("TAG", "OnTouch : ACTION_DOWN(Right)");
                if(arg0 == buttonRightTurn)	    sndOpkey = "Right";
            }
        }
        if(action == MotionEvent.ACTION_UP){
            if(id == R.id.buttonup) {
                Log.d("TAG", "OnTouch : ACTION_UP(UP)");
                if(arg0 == buttonUp)	    sndOpkey = "Stop_Go";
            }
            if (id == R.id.buttondown) {
                Log.d("TAG", "OnTouch : ACTION_UP(Down)");
                if(arg0 == buttonDown)	    sndOpkey = "Stop_Back";
            }
            if (id == R.id.buttonleft) {
                Log.d("TAG", "OnTouch : ACTION_UP(Left)");
                if(arg0 == buttonLeftTurn)	    sndOpkey = "Stop_Left";
            }
            if (id == R.id.buttonright) {
                Log.d("TAG", "OnTouch : ACTION_UP(Right)");
                if(arg0 == buttonRightTurn)	    sndOpkey = "Stop_Right";
            }
        }
        try{
            outs.write(sndOpkey.getBytes("UTF-8"));
            outs.flush();
        } catch (IOException e){
            e.printStackTrace();
        }
        return false;
    }

    public void onStart() {
    	if(DEBUG) Log.d(TAG,"onStart()");
        super.onStart();
    }
    public void onPause() {
    	if(DEBUG) Log.d(TAG,"onPause()");
        super.onPause();
        if(mv!=null){
        	if(mv.isStreaming()){
		        mv.stopPlayback();
		        suspending = true;
        	}
        }
    }
    public void onStop() {
    	if(DEBUG) Log.d(TAG,"onStop()");
        super.onStop();
    }

    public void onDestroy() {
    	if(DEBUG) Log.d(TAG,"onDestroy()");
    	
    	if(mv!=null){
    		mv.freeCameraMemory();
    	}
    	
        super.onDestroy();
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	MenuInflater inflater = getMenuInflater();
    	inflater.inflate(R.layout.option_menu, menu);
    	return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	switch (item.getItemId()) {
    		case R.id.settings:
    			Intent settings_intent = new Intent(MjpegActivity.this, SettingsActivity.class);
    			settings_intent.putExtra("width", width);
    			settings_intent.putExtra("height", height);
    			settings_intent.putExtra("ip_ad1", ip_ad1);
    			settings_intent.putExtra("ip_ad2", ip_ad2);
    			settings_intent.putExtra("ip_ad3", ip_ad3);
    			settings_intent.putExtra("ip_ad4", ip_ad4);
    			settings_intent.putExtra("ip_port", ip_port);
    			settings_intent.putExtra("ip_command", ip_command);
    			startActivityForResult(settings_intent, REQUEST_SETTINGS);
    			return true;
    	}
    	return false;
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    	switch (requestCode) {
    		case REQUEST_SETTINGS:
    			if (resultCode == Activity.RESULT_OK) {
    				width = data.getIntExtra("width", width);
    				height = data.getIntExtra("height", height);
    				ip_ad1 = data.getIntExtra("ip_ad1", ip_ad1);
    				ip_ad2 = data.getIntExtra("ip_ad2", ip_ad2);
    				ip_ad3 = data.getIntExtra("ip_ad3", ip_ad3);
    				ip_ad4 = data.getIntExtra("ip_ad4", ip_ad4);
    				ip_port = data.getIntExtra("ip_port", ip_port);
    				ip_command = data.getStringExtra("ip_command");

    				if(mv!=null){
    					mv.setResolution(width, height);
    				}
    				SharedPreferences preferences = getSharedPreferences("SAVED_VALUES", MODE_PRIVATE);
    				SharedPreferences.Editor editor = preferences.edit();
    				editor.putInt("width", width);
    				editor.putInt("height", height);
    				editor.putInt("ip_ad1", ip_ad1);
    				editor.putInt("ip_ad2", ip_ad2);
    				editor.putInt("ip_ad3", ip_ad3);
    				editor.putInt("ip_ad4", ip_ad4);
    				editor.putInt("ip_port", ip_port);
    				editor.putString("ip_command", ip_command);

    				editor.commit();

    				new RestartApp().execute();
    			}
    			break;
    	}
    }

    
    public class DoRead extends AsyncTask<String, Void, MjpegInputStream> {
        protected MjpegInputStream doInBackground(String... url) {
            //TODO: if camera has authentication deal with it and don't just not work
            HttpResponse res = null;
            DefaultHttpClient httpclient = new DefaultHttpClient(); 
            HttpParams httpParams = httpclient.getParams();
            HttpConnectionParams.setConnectionTimeout(httpParams, 5*1000);
            Log.d(TAG, "1. Sending http request");
            try {
                res = httpclient.execute(new HttpGet(URI.create(url[0])));
                Log.d(TAG, "2. Request finished, status = " + res.getStatusLine().getStatusCode());
                if(res.getStatusLine().getStatusCode()==401){
                    //You must turn off camera User Access Control before this will work
                    return null;
                }
                return new MjpegInputStream(res.getEntity().getContent());  
            } catch (ClientProtocolException e) {
                e.printStackTrace();
                Log.d(TAG, "Request failed-ClientProtocolException", e);
                //Error connecting to camera
            } catch (IOException e) {
                e.printStackTrace();
                Log.d(TAG, "Request failed-IOException", e);
                //Error connecting to camera
            }
            return null;
        }

        protected void onPostExecute(MjpegInputStream result) {
            mv.setSource(result);
            if(result!=null) result.setSkip(1);
            mv.setDisplayMode(MjpegView.SIZE_BEST_FIT);
            mv.showFps(false);
        }
    }
    
    public class RestartApp extends AsyncTask<Void, Void, Void> {
        protected Void doInBackground(Void... v) {
			MjpegActivity.this.finish();
            return null;
        }

        protected void onPostExecute(Void v) {
        	startActivity((new Intent(MjpegActivity.this, MjpegActivity.class)));
        }
    }
}
