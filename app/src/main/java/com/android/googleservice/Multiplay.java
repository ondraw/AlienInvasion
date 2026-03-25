package com.android.googleservice;

import android.util.Log;
import kr.co.songs.android.AlienInvasion.AlienInvasion;

public class Multiplay {

    static final String TAG = "Multiplay";
    private final AlienInvasion mActivity;
    private boolean mDebugLog = false;

    public Multiplay(AlienInvasion activity) {
        mActivity = activity;
    }

    public void enableDebugLog(boolean enabled) {
        mDebugLog = enabled;
    }

    public void FindPlyer(long nResetFindTime, long nMatchFilter, long nMatchGroup) {
        if (mDebugLog) {
            Log.d(TAG, "Multiplayer is disabled. FindPlyer request ignored.");
        }
    }

    public void LogOut() {
        if (mDebugLog) {
            Log.d(TAG, "Multiplayer is disabled. LogOut request ignored.");
        }
    }

    public void SendData(byte[] btData, boolean bReiable) {
        if (mDebugLog) {
            Log.d(TAG, "Multiplayer is disabled. SendData request ignored.");
        }
    }
}
