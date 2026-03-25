package com.android.googleservice;

import android.app.Activity;
import android.content.Intent;
import com.google.android.gms.common.api.GoogleApiClient;

public class GameHelper {

    public interface GameHelperListener {
        void onSignInFailed();
        void onSignInSucceeded();
    }

    public static final int RC_WAITING_ROOM = 9004;
    public static final int RC_LEADERBOARD = 9005;
    public static final int CLIENT_NONE = 0x00;
    public static final int CLIENT_GAMES = 0x01;
    public static final int CLIENT_PLUS = 0x02;
    public static final int CLIENT_SNAPSHOT = 0x08;
    public static final int CLIENT_ALL = CLIENT_GAMES | CLIENT_PLUS | CLIENT_SNAPSHOT;

    public GameHelper(Activity activity, int clientsToUse) {
    }

    public void setMaxAutoSignInAttempts(int max) {
    }

    public void enableDebugLog(boolean enabled) {
    }

    public void setup(GameHelperListener listener) {
    }

    public GoogleApiClient getApiClient() {
        return null;
    }

    public boolean isSignedIn() {
        return false;
    }

    public boolean hasSignInError() {
        return false;
    }

    public void onStart(Activity activity) {
    }

    public void onStop() {
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    }

    public void beginUserInitiatedSignIn() {
    }

    public void signOut() {
    }
}
