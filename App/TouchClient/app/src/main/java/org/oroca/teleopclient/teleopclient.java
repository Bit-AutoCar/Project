package org.oroca.teleopclient;

import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.TextView;
import android.widget.Button;
import android.widget.EditText;

import java.net.Socket;
import java.io.IOException;
import java.io.OutputStream;

public class teleopclient extends Activity implements OnClickListener, View.OnTouchListener {

	private EditText editTextIPAddress;
	private TextView textViewStatus;
	private Button buttonConnect;
	private Button buttonClose;
	private Button buttonUp;
	private Button buttonLeftTurn;
	private Button buttonRightTurn;
	private Button buttonDown;
	private Button buttonStop;
	private InputMethodManager imm;
	private String server = "192.168.1.31";
	private int port = 3000;
	private Socket socket;
	private OutputStream outs;
	private Thread rcvThread;
	public logger logger;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
			
		editTextIPAddress = (EditText)this.findViewById(R.id.editTextIPAddress);
		editTextIPAddress.setText(server);
		imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
				
		textViewStatus = (TextView)this.findViewById(R.id.textViewStatus);
		textViewStatus.setText("TeleOp Client");
		
		logger = new logger(textViewStatus);
		
		buttonConnect   = (Button)this.findViewById(R.id.buttonConnect);
		buttonClose     = (Button)this.findViewById(R.id.buttonClose);
		buttonUp        = (Button)this.findViewById(R.id.buttonUp);
		buttonLeftTurn  = (Button)this.findViewById(R.id.buttonLeftTurn);
		buttonRightTurn = (Button)this.findViewById(R.id.buttonRightTurn);
		buttonDown      = (Button)this.findViewById(R.id.buttonDown);
		buttonStop      = (Button)this.findViewById(R.id.buttonStop);
		
		buttonConnect.setOnClickListener(this);
		buttonClose.setOnClickListener(this);

		buttonUp.setOnTouchListener(this);
		buttonLeftTurn.setOnTouchListener(this);
		buttonRightTurn.setOnTouchListener(this);
		buttonDown.setOnTouchListener(this);

		buttonStop.setOnClickListener(this);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public void onClick(View arg0) {
		if(arg0 == buttonConnect)
		{
			imm.hideSoftInputFromWindow(editTextIPAddress.getWindowToken(), 0);
			
			try{
				if(socket!=null)
				{
					socket.close();
					socket = null;
				}
				
				server = editTextIPAddress.getText().toString();
				socket = new Socket(server, port);
				outs = socket.getOutputStream();

				rcvThread = new Thread(new rcvthread(logger, socket));
    		    rcvThread.start();
				logger.log(server);
				//logger.log("Connected");
			} catch (IOException e){
				logger.log("Fail to connect");
				e.printStackTrace();
			}
		}
		
		if(arg0 == buttonClose)
		{
			imm.hideSoftInputFromWindow(editTextIPAddress.getWindowToken(), 0);
			
			if(socket!=null)
			{
				exitFromRunLoop();
				try{
					socket.close();
					socket = null;
					logger.log("Closed!");
					rcvThread = null;
				} catch (IOException e){
					logger.log("Fail to close");
					e.printStackTrace();
				}
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
			if (id == R.id.buttonUp) {
				Log.d("TAG", "OnTouch : ACTION_DOWN(UP)");
				if(arg0 == buttonUp)	    sndOpkey = "Go";
			}
			if (id == R.id.buttonDown) {
				Log.d("TAG", "OnTouch : ACTION_DOWN(Down)");
				if(arg0 == buttonDown)	    sndOpkey = "Back";
			}
			if (id == R.id.buttonLeftTurn) {
				Log.d("TAG", "OnTouch : ACTION_DOWN(Left)");
				if(arg0 == buttonLeftTurn)	    sndOpkey = "Left";
			}
			if (id == R.id.buttonRightTurn) {
				Log.d("TAG", "OnTouch : ACTION_DOWN(Right)");
				if(arg0 == buttonRightTurn)	    sndOpkey = "Right";
			}
		}
		if(action == MotionEvent.ACTION_UP){
			if(id == R.id.buttonUp) {
				Log.d("TAG", "OnTouch : ACTION_UP(UP)");
				if(arg0 == buttonUp)	    sndOpkey = "Stop_Go";
			}
			if (id == R.id.buttonDown) {
				Log.d("TAG", "OnTouch : ACTION_UP(Down)");
				if(arg0 == buttonDown)	    sndOpkey = "Stop_Back";
			}
			if (id == R.id.buttonLeftTurn) {
				Log.d("TAG", "OnTouch : ACTION_UP(Left)");
				if(arg0 == buttonLeftTurn)	    sndOpkey = "Stop_Left";
			}
			if (id == R.id.buttonRightTurn) {
				Log.d("TAG", "OnTouch : ACTION_UP(Right)");
				if(arg0 == buttonRightTurn)	    sndOpkey = "Stop_Right";
			}
		}
		try{
			outs.write(sndOpkey.getBytes("UTF-8"));
			outs.flush();
		} catch (IOException e){
			logger.log("Fail to send");
			e.printStackTrace();
		}
		return false;
	}
    void exitFromRunLoop(){
    	try {
    		String sndOpkey = "[close]";
    		outs.write(sndOpkey.getBytes("UTF-8"));
    		outs.flush();
    	} catch (IOException e) {
			logger.log("Fail to send");
			e.printStackTrace();
    	}
    }
}
