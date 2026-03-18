package com.android.googleservice;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import kr.co.songs.android.AlienInvasion.AlienInvasion;
import kr.co.songs.android.AlienInvasion.SongGLLib;
import kr.co.songs.android.AlienInvasion.iap.util.IabHelper;
import kr.co.songs.android.AlienInvasion.iap.util.IabResult;
import kr.co.songs.android.AlienInvasion.iap.util.Inventory;
import kr.co.songs.android.AlienInvasion.iap.util.Purchase;
import kr.co.songs.android.AlienInvasion.iap.util.SkuDetails;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class IAPMan 
{
	ArrayList<String> mQueryLists = null;
	public static final int RC_REQUEST = 10002;
	private static final String TAG = "SongsJavaIAP";
	IabHelper mHelper;
    Activity mActivity;
    boolean   mbWaiting = false;//연속해서 요청하지 않게 한다.
    //0 : 영구
    //1 : 구
    Map<String,Integer> mPermenent = new HashMap<String,Integer>();
    public IAPMan(Activity actvity) 
    {
    	mActivity = actvity;
    }
    
    public void InitIAP(String base64EncodedPublicKey)
    {
    	mHelper = new IabHelper(mActivity, base64EncodedPublicKey);
    	mHelper.enableDebugLogging(true);
    	//초기화를 하면서 상품구매내역을 쿼리한다.
    	mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() 
    	{
            public void onIabSetupFinished(IabResult result) 
            {
                if (!result.isSuccess()) 
                {
                    // Oh noes, there was a problem.
                    complain("Problem setting up in-app billing: " + result);
                    return;
                }

                // Have we been disposed of in the meantime? If so, quit.
                if (mHelper == null) return;
            }
        });
    }
    
    public void Buy(String sProductID,boolean bCountable)
	{	
    	if(mbWaiting == true) return; //이미요청중에 있다.
    	mbWaiting = true;
		mPermenent.clear();
		if(bCountable == false)
			addPermanentProducts(sProductID);
		QueryProducts(sProductID);
	}
    
    public void addPermanentProducts(String sProdcutID)
    {
    	mPermenent.put(sProdcutID, 0);
    }
    
   public void addSubscribeProducts(String sProdcutID)
   {
	   mPermenent.put(sProdcutID, 1);
   }
    
    
    public void setQueryProducts(ArrayList<String> listProducts)
    {
    	mQueryLists = listProducts;
    }
    
    public Handler mIAP = new Handler() {
		public void handleMessage(Message msg) {
			int nID = msg.getData().getInt("id");
			if (nID == 0) //프러덕트 쿠리를 날린다.
			{
				String sID = msg.getData().getString("sku");
				mQueryLists = new ArrayList<String>();
		    	mQueryLists.add(sID);
		        mHelper.queryInventoryAsync(true, mQueryLists, mGotInventoryListener);
			}
		}
    };
   
    public void QueryProducts(String sID)
    {
    	Message msg = mIAP.obtainMessage();
		Bundle b = new Bundle();
		b.putInt("id", 0);
		b.putString("sku", sID);
		msg.setData(b);
		mIAP.sendMessage(msg);
    }
    
    protected void Buy(String sID)
    {
    	if(mHelper != null)
    	{
    		String payload = "";
    		Integer nType = mPermenent.get(sID);
    		if(nType != null && nType == 1) //구독.
    			mHelper.launchPurchaseFlow(mActivity, sID, IabHelper.ITEM_TYPE_SUBS,RC_REQUEST, mPurchaseFinishedListener,payload);
    		else
    			mHelper.launchPurchaseFlow(mActivity, sID, RC_REQUEST, mPurchaseFinishedListener,payload);
    	}
    }
    
    
    
    public void Resore()
    {
    	//구매내역을 가져온다. IabHelper.QueryInventoryFinishedListener
    	mHelper.queryInventoryAsync(mGotInventoryListener);
    }
    
 // 구매내역을 쿼리한후에...
    IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener() {
        public void onQueryInventoryFinished(IabResult result, Inventory inventory) 
        {   
        	try
        	{
        		if (mHelper == null) return;
        		// Is it a failure?
        		if (result.isFailure()) {
        			complain("Failed to query inventory: " + result);
        			mbWaiting = false;
        			return;
        		}

        		//Org Source
        		String[] rowData = null;
        		
        		int nCnt = mQueryLists.size();
        		for(int i = 0; i < nCnt; i++)
        		{
        			String sProdID = mQueryLists.get(i);
        			SkuDetails detail = inventory.getSkuDetails(sProdID);
        			if(detail == null)
        			{
        				Log.e(TAG, "getSkuDetails null Error: " + sProdID);
        				continue;
        			}
        			
        			rowData = new String[5];

        			rowData[0] = detail.getSku();
        			rowData[1] = detail.getTitle();
        			rowData[2] = detail.getDescription();
        			rowData[3] = detail.getType();
        			rowData[4] = detail.getPrice();


        			//SongGLLib.sglAddProductsList(rowData);
        			Purchase purchase = inventory.getPurchase(sProdID);
        			if(purchase != null)
        			{
        				Integer nType = mPermenent.get(sProdID);
        				if(nType == null) //소모성인데 상품은 샀지만 적용이 안되었다면 다시 클리어 해준다.
        				{	
        					mHelper.consumeAsync(purchase, mConsumeFinishedListener);
        				}
        			}
        		}

        		if(rowData != null)
        		{
        			Buy(rowData[0]);
        		}
        	}
        	catch(Exception e)
        	{
        		Log.e(TAG, " onQueryInventoryFinished " + e.getLocalizedMessage());
        	}
        	finally
        	{
        		mbWaiting = false; //요청을푼다.
        	}
        }
    };
    
    
    public void onDestroy() {
        // very important:
        Log.d(TAG, "Destroying helper.");
        if (mHelper != null) {
            mHelper.dispose();
            mHelper = null;
        }
    }
    
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + data);
        try
        {
	        // Pass on the activity result to the helper for handling
	        if(mHelper != null)
	        	mHelper.handleActivityResult(requestCode, resultCode, data);
        }
        catch(Exception e)
        {
        	Log.e(TAG, "onActivityResult " + e.getLocalizedMessage());
        }
    }
    
    void ResultPurchase(String itemId,boolean bSuccess,String sMessage)
    {
    	// 정보파일을 보낸다.
    	if(bSuccess)
    	{
			if (SongGLLib.WriteCacheProduct(itemId)) {
				AlienInvasion.gGLView.SendMessage(
						SongGLLib.SGL_BUY_PRODUCT_GOLDTTYPE_FOR_ANDROID, 0,
						0);
			}
    	}
    	else
    	{
    		Log.e("JavaSongs", "상품 요청 에러입니다 (ID=" +itemId +")" + sMessage);
    	}
    }
    
 // Callback for when a purchase is finished
    IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
        public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
            Log.d(TAG, "Purchase finished: " + result + ", purchase: " + purchase);

            // if we were disposed of in the meantime, quit.
            if (mHelper == null) return;

            if (result.isFailure()) {
                //complain("Error purchasing: " + result);
            	//실패.
            	if(purchase != null)
            	{
            		ResultPurchase(purchase.getSku(),false, result.getMessage());
            	}
            	else
            	{
            		ResultPurchase("", false, result.getMessage());
            	}
                return;
            }
            
        	Integer nType = mPermenent.get(purchase.getSku());
    		if(nType == null) //소모성인데 상품은 샀지만 적용이 안되었다면 다시 클리어 해준다.
    		{	
    			mHelper.consumeAsync(purchase, mConsumeFinishedListener);
    		}
    		else if(nType == 0) //영구
    		{
    			ResultPurchase(purchase.getSku(), true, "");
    		}
    		else if(nType == 1) //구독
    		{
    			ResultPurchase(purchase.getSku(), true, "");
    		}
        }
    };
    
    
    
 // Called when consumption is complete
    IabHelper.OnConsumeFinishedListener mConsumeFinishedListener = new IabHelper.OnConsumeFinishedListener() 
    {
        public void onConsumeFinished(Purchase purchase, IabResult result) {
            Log.d(TAG, "Consumption finished. Purchase: " + purchase + ", result: " + result);

            // if we were disposed of in the meantime, quit.
            if (mHelper == null) return;

            // We know this is the "gas" sku because it's the only one we consume,
            // so we don't check which sku was consumed. If you have more than one
            // sku, you probably should check...
            if (result.isSuccess()) {
                //성공.
            	ResultPurchase(purchase.getSku(), true, "");
            }
            else 
            {
                //실패.
            	if(purchase != null)
            		ResultPurchase(purchase.getSku(), false, result.getMessage());
            	else
            		ResultPurchase("", false, result.getMessage());
            }
        }
    };
    
    void complain(String message) {
        Log.e(TAG, "**** TrivialDrive Error: " + message);
        alert("Error: " + message);
    }
    
    void alert(String message) {
        AlertDialog.Builder bld = new AlertDialog.Builder(mActivity);
        bld.setMessage(message);
        bld.setNeutralButton("OK", null);
        Log.d(TAG, "Showing alert dialog:" + message);
        bld.create().show();
    }
}
