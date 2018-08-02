package demo.player.ffl.androidplayer;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.SurfaceTexture;
import android.media.AudioTrack;
import android.net.Uri;
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.EditText;

import ffl.player.FFLPlayer;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    TextureView mTextureView;
    FFLPlayer mFFLPlayer;

    String mUrl;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        requestPerssion();

        mTextureView=(TextureView)findViewById(R.id.video);
        mUrl="/sdcard/DCIM/sintel.ts";

        View v=findViewById(R.id.btnPlay);
        v.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                String url=mUrl;
                getPlayer().native_setSurface(new Surface(mTextureView.getSurfaceTexture()));
                getPlayer().native_play(url);
            }
        });

        v=findViewById(R.id.btnStop);
        v.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                getPlayer().native_stop();
            }
        });

        v=findViewById(R.id.btnPause);
        v.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                getPlayer().native_pause(1);
            }
        });

        v=findViewById(R.id.btnResume);
        v.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                getPlayer().native_pause(0);
            }
        });

        v=findViewById(R.id.btnPrepare);
        v.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                getPlayer().native_prepare();
            }
        });

        v=findViewById(R.id.btnSeturl);
        v.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                chooseVideo();

            }
        });
        v=findViewById(R.id.btnSpeed);
        v.setOnClickListener(this);

        v=findViewById(R.id.btnLoop);
        v.setOnClickListener(this);

        v=findViewById(R.id.btnVolume);
        v.setOnClickListener(this);

        v=findViewById(R.id.btnPosition);
        v.setOnClickListener(this);
    }

    private FFLPlayer getPlayer(){
        if(mFFLPlayer==null) {
            mFFLPlayer = new FFLPlayer();

        }
        return mFFLPlayer;
    }
    private void requestPerssion(){
        //判断用户是否已经授权，未授权则向用户申请授权，已授权则直接进行呼叫操作
        //
        if(ContextCompat.checkSelfPermission(
                MainActivity.this,"Manifest.permission.WRITE_EXTERNAL_STORAGE")
                != PackageManager.PERMISSION_GRANTED)
        {
            //注意第二个参数没有双引号
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},1);
        }
    }

    private void chooseVideo(){
        Intent intent=new Intent(Intent.ACTION_PICK);
        intent.setData(MediaStore.Video.Media.EXTERNAL_CONTENT_URI);
        startActivityForResult(intent, 1);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case 1:
                if (resultCode == RESULT_OK) {
                    try {
                        Uri selectedImage = data.getData(); //获取系统返回的照片的Uri
                        String[] filePathColumn = {MediaStore.Images.Media.DATA};
                        Cursor cursor = getContentResolver().query(selectedImage,
                                filePathColumn, null, null, null);//从系统表中查询指定Uri对应的照片
                        cursor.moveToFirst();
                        int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
                        String filePath = cursor.getString(columnIndex);
                        mUrl=filePath;
                       // getPlayer().native_setUrl(filePath);
                        cursor.close();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                break;
        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.btnSpeed:
            {
                EditText ed=(EditText)findViewById(R.id.edit_speed);
                String info=ed.getText().toString();
                int val=Integer.parseInt(info);
                getPlayer().native_setSpeed(val);
            }
            break;
            case R.id.btnLoop:
            {
                EditText ed=(EditText)findViewById(R.id.edit_loop);
                String info=ed.getText().toString();
                int val=Integer.parseInt(info);
                getPlayer().native_setLoop(val);
            }
            break;

            case R.id.btnVolume:
            {
                EditText ed=(EditText)findViewById(R.id.edit_volume);
                String info=ed.getText().toString();
                int val=Integer.parseInt(info);
                getPlayer().native_setVolume(val);
            }
            break;

            case R.id.btnPosition:
            {
                EditText ed=(EditText)findViewById(R.id.edit_position);
                String info=ed.getText().toString();
                int val=Integer.parseInt(info);
                getPlayer().native_seekTo(val);
            }
            break;
        }
    }
}
