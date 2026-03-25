package com.android.googleservice;

import android.app.Activity;
import android.app.AlertDialog;
import android.util.Log;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesResponseListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;
import com.android.billingclient.api.ProductDetailsResponseListener;
import com.android.billingclient.api.PurchasesUpdatedListener;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import kr.co.songs.android.AlienInvasion.AlienInvasion;
import kr.co.songs.android.AlienInvasion.SongGLLib;

public class IAPMan {
    public static final int RC_REQUEST = 10002;
    private static final String TAG = "SongsJavaIAP";

    private final Activity mActivity;
    private BillingClient mBillingClient;
    private boolean mbWaiting = false;
    private final Map<String, Integer> mPermanent = new HashMap<String, Integer>();
    private ArrayList<String> mQueryLists = null;
    private Runnable mPendingAction = null;

    public IAPMan(Activity activity) {
        mActivity = activity;
    }

    public void InitIAP(String base64EncodedPublicKey) {
        mBillingClient = BillingClient.newBuilder(mActivity)
                .enablePendingPurchases()
                .setListener(mPurchasesUpdatedListener)
                .build();
        startConnection(null);
    }

    public void Buy(String productId, boolean countable) {
        if (mbWaiting) {
            return;
        }
        mbWaiting = true;
        mPermanent.clear();
        if (!countable) {
            addPermanentProducts(productId);
        }
        QueryProducts(productId);
    }

    public void addPermanentProducts(String productId) {
        mPermanent.put(productId, 0);
    }

    public void addSubscribeProducts(String productId) {
        mPermanent.put(productId, 1);
    }

    public void setQueryProducts(ArrayList<String> listProducts) {
        mQueryLists = listProducts;
    }

    public void QueryProducts(final String productId) {
        runWhenReady(new Runnable() {
            @Override
            public void run() {
                mQueryLists = new ArrayList<String>();
                mQueryLists.add(productId);

                QueryProductDetailsParams.Product product =
                        QueryProductDetailsParams.Product.newBuilder()
                                .setProductId(productId)
                                .setProductType(getProductType(productId))
                                .build();

                QueryProductDetailsParams params = QueryProductDetailsParams.newBuilder()
                        .setProductList(java.util.Collections.singletonList(product))
                        .build();

                mBillingClient.queryProductDetailsAsync(params, mProductDetailsListener);
            }
        });
    }

    public void Resore() {
        queryExistingPurchases(BillingClient.ProductType.INAPP);
        queryExistingPurchases(BillingClient.ProductType.SUBS);
    }

    public void onDestroy() {
        Log.d(TAG, "Destroying billing client.");
        if (mBillingClient != null) {
            mBillingClient.endConnection();
            mBillingClient = null;
        }
    }

    public void onActivityResult(int requestCode, int resultCode, android.content.Intent data) {
        Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + data + ")");
    }

    private void startConnection(final Runnable onConnected) {
        if (mBillingClient == null) {
            return;
        }
        if (mBillingClient.isReady()) {
            if (onConnected != null) {
                onConnected.run();
            }
            return;
        }

        mPendingAction = onConnected;
        mBillingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(BillingResult billingResult) {
                if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    Runnable pending = mPendingAction;
                    mPendingAction = null;
                    if (pending != null) {
                        pending.run();
                    }
                } else {
                    mbWaiting = false;
                    complain("Problem setting up in-app billing: " + billingResult.getDebugMessage());
                }
            }

            @Override
            public void onBillingServiceDisconnected() {
                Log.w(TAG, "Billing service disconnected.");
            }
        });
    }

    private void runWhenReady(Runnable action) {
        if (mBillingClient != null && mBillingClient.isReady()) {
            action.run();
        } else {
            startConnection(action);
        }
    }

    private String getProductType(String productId) {
        Integer type = mPermanent.get(productId);
        return type != null && type == 1
                ? BillingClient.ProductType.SUBS
                : BillingClient.ProductType.INAPP;
    }

    private final ProductDetailsResponseListener mProductDetailsListener = new ProductDetailsResponseListener() {
        @Override
        public void onProductDetailsResponse(BillingResult billingResult, List<ProductDetails> productDetailsList) {
            if (billingResult.getResponseCode() != BillingClient.BillingResponseCode.OK) {
                mbWaiting = false;
                complain("Failed to query products: " + billingResult.getDebugMessage());
                return;
            }

            if (productDetailsList == null || productDetailsList.isEmpty()) {
                mbWaiting = false;
                complain("Product not found in Play Billing.");
                return;
            }

            launchPurchase(productDetailsList.get(0));
        }
    };

    private void launchPurchase(ProductDetails productDetails) {
        BillingFlowParams.ProductDetailsParams.Builder detailsParamsBuilder =
                BillingFlowParams.ProductDetailsParams.newBuilder()
                        .setProductDetails(productDetails);

        if (BillingClient.ProductType.SUBS.equals(productDetails.getProductType())
                && productDetails.getSubscriptionOfferDetails() != null
                && !productDetails.getSubscriptionOfferDetails().isEmpty()) {
            detailsParamsBuilder.setOfferToken(
                    productDetails.getSubscriptionOfferDetails().get(0).getOfferToken());
        }

        BillingFlowParams billingFlowParams = BillingFlowParams.newBuilder()
                .setProductDetailsParamsList(
                        java.util.Collections.singletonList(detailsParamsBuilder.build()))
                .build();

        BillingResult billingResult = mBillingClient.launchBillingFlow(mActivity, billingFlowParams);
        if (billingResult.getResponseCode() != BillingClient.BillingResponseCode.OK) {
            mbWaiting = false;
            ResultPurchase("", false, billingResult.getDebugMessage());
        }
    }

    private final PurchasesUpdatedListener mPurchasesUpdatedListener = new PurchasesUpdatedListener() {
        @Override
        public void onPurchasesUpdated(BillingResult billingResult, List<Purchase> purchases) {
            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK && purchases != null) {
                handlePurchases(purchases);
                return;
            }

            mbWaiting = false;
            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.USER_CANCELED) {
                ResultPurchase("", false, "User canceled.");
                return;
            }

            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED
                    && mQueryLists != null && !mQueryLists.isEmpty()) {
                queryExistingPurchases(getProductType(mQueryLists.get(0)));
                return;
            }

            ResultPurchase("", false, billingResult.getDebugMessage());
        }
    };

    private void queryExistingPurchases(final String productType) {
        runWhenReady(new Runnable() {
            @Override
            public void run() {
                QueryPurchasesParams params = QueryPurchasesParams.newBuilder()
                        .setProductType(productType)
                        .build();
                mBillingClient.queryPurchasesAsync(params, new PurchasesResponseListener() {
                    @Override
                    public void onQueryPurchasesResponse(BillingResult billingResult, List<Purchase> purchases) {
                        if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK && purchases != null) {
                            handlePurchases(purchases);
                        } else {
                            mbWaiting = false;
                            complain("Failed to query purchases: " + billingResult.getDebugMessage());
                        }
                    }
                });
            }
        });
    }

    private void handlePurchases(List<Purchase> purchases) {
        for (Purchase purchase : purchases) {
            if (purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) {
                continue;
            }

            List<String> products = purchase.getProducts();
            if (products == null || products.isEmpty()) {
                continue;
            }

            for (String productId : products) {
                Integer type = mPermanent.get(productId);
                if (type == null) {
                    consumePurchase(productId, purchase);
                } else {
                    acknowledgePurchase(productId, purchase);
                }
            }
        }
    }

    private void consumePurchase(final String productId, Purchase purchase) {
        String purchaseToken = purchase.getPurchaseToken();
        if (purchaseToken == null || purchaseToken.length() == 0) {
            mbWaiting = false;
            ResultPurchase(productId, false, "Missing purchase token.");
            return;
        }

        ConsumeParams params = ConsumeParams.newBuilder()
                .setPurchaseToken(purchaseToken)
                .build();

        mBillingClient.consumeAsync(params, new ConsumeResponseListener() {
            @Override
            public void onConsumeResponse(BillingResult billingResult, String purchaseToken) {
                mbWaiting = false;
                if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    ResultPurchase(productId, true, "");
                } else {
                    ResultPurchase(productId, false, billingResult.getDebugMessage());
                }
            }
        });
    }

    private void acknowledgePurchase(final String productId, Purchase purchase) {
        if (purchase.isAcknowledged()) {
            mbWaiting = false;
            ResultPurchase(productId, true, "");
            return;
        }

        AcknowledgePurchaseParams params = AcknowledgePurchaseParams.newBuilder()
                .setPurchaseToken(purchase.getPurchaseToken())
                .build();

        mBillingClient.acknowledgePurchase(params, new com.android.billingclient.api.AcknowledgePurchaseResponseListener() {
            @Override
            public void onAcknowledgePurchaseResponse(BillingResult billingResult) {
                mbWaiting = false;
                if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    ResultPurchase(productId, true, "");
                } else {
                    ResultPurchase(productId, false, billingResult.getDebugMessage());
                }
            }
        });
    }

    void ResultPurchase(String itemId, boolean success, String message) {
        if (success) {
            if (SongGLLib.WriteCacheProduct(itemId)) {
                AlienInvasion.gGLView.SendMessage(
                        SongGLLib.SGL_BUY_PRODUCT_GOLDTTYPE_FOR_ANDROID, 0, 0);
            }
        } else {
            Log.e("JavaSongs", "상품 요청 에러입니다 (ID=" + itemId + ")" + message);
        }
    }

    void complain(String message) {
        Log.e(TAG, "Billing error: " + message);
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
