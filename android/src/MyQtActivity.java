package com.tap2happy.nadoview;
import org.qtproject.qt.android.bindings.QtActivity;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.Manifest;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class MyQtActivity extends QtActivity {
    // @Override
    // public void onCreate() {
    //     super.onCreate();
    //     // Log.i(TAG, "Creating and run MyActivity");
    // }
    private static String[] PERMISSIONS_STORAGE={
            "android.permission.WRITE_EXTERNAL_STORAGE",
            "android.permission.READ_EXTERNAL_STORAGE"};

    public MyQtActivity()
    {
       super(); 
       Log.d("MyQActivity", "MyQtActivity init");

    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        Context ctx = getApplicationContext();
        for (String permission : PERMISSIONS_STORAGE) {
            int per = ContextCompat.checkSelfPermission(ctx, PERMISSIONS_STORAGE[0]);
            if (PackageManager.PERMISSION_GRANTED != per) {
                Log.w("XXXX", "required permission not granted . permission = " + permission);
                ActivityCompat.requestPermissions(this,PERMISSIONS_STORAGE,PERMISSIONS_STORAGE.length);
            }
        }
    }



}
