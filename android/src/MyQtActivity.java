package com.tap2happy.nadoview;
import org.qtproject.qt.android.bindings.QtActivity;
import android.os.Bundle;
// // import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.IOException;

public class MyQtActivity extends QtActivity {
    // @Override
    // public void onCreate() {
    //     super.onCreate();
    //     // Log.i(TAG, "Creating and run MyActivity");
    // }
    public MyQtActivity()
    {
       super(); 
       Log.d("MyQActivity", "MyQtActivity init");
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

         // 获取启动Activity的Intent
         Intent intent = getIntent();
         String action = intent.getAction();

         // 如果是打开文件的Intent，处理文件打开事件
        if (Intent.ACTION_VIEW.equals(action)) {
            Uri fileUri = intent.getData();
            if (fileUri != null ) {
                // 处理epub文件打开事件
                Log.d("MyQtActivity", "Open epub file: " + fileUri.getPath());
       
                // try {
                //     FileInputStream inputStream = new FileInputStream(file);
                //     // byte[] bytes = new byte[(int) file.length()];
                //     // inputStream.read(bytes);
                //     // // String content = new String(bytes);
                //     // Log.d("MyQActivity", bytes.length());
                //     Log.d("MyQActivity","open success;");
                //     inputStream.close();
                // } catch (FileNotFoundException e) {
                //     Log.e("MyQActivity", "File not found: " + e.getMessage());
                // } catch (IOException e) {
                //     Log.e("MyQActivity", "Error reading file: " + e.getMessage());
                // }

            }
        }
    }

  

}
