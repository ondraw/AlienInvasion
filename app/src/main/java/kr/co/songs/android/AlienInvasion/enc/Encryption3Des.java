package kr.co.songs.android.AlienInvasion.enc;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.KeySpec;

import javax.crypto.Cipher;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.DESedeKeySpec;

public class Encryption3Des {
	
	private static final String ALGORITHM = "DESEde";
   
	//3DES
    private static final byte[]  BYTE_KEY = {  
    	(byte) 0xBC,(byte) 0xDB,(byte) 0xC8,(byte) 0xA3 ,
    	(byte) 0xC7,(byte) 0xD0,(byte) 0xBE,(byte) 0xE7 ,
    	(byte) 0xC0,(byte) 0xAF,(byte) 0xB9,(byte) 0xCE ,
    	(byte) 0xB9,(byte) 0xDA,(byte) 0xC7,(byte) 0xFC ,			
    	(byte) 0xBC,(byte) 0xF6,(byte) 0xC0,(byte) 0xAF ,
    	(byte) 0xBB,(byte) 0xF3,(byte) 0xC3,(byte) 0xB5	};
    
    private static KeySpec keySpec;
    private static Cipher cipher;
    private static SecretKey key;
    private static SecretKeyFactory keyFactory;   
    static {
    	
    	try {
    		
    		keySpec = new DESedeKeySpec(BYTE_KEY);
    		keyFactory = SecretKeyFactory.getInstance(ALGORITHM);
    		cipher = Cipher.getInstance("DESEde/ECB/PKCS5Padding");
    		key = keyFactory.generateSecret(keySpec);
			
    	} catch (InvalidKeyException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
    	} catch (NoSuchAlgorithmException e) {
    		// TODO Auto-generated catch block
    		e.printStackTrace();
    	} catch (NoSuchPaddingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InvalidKeySpecException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
    }
	
	public static byte[] encrypt(byte [] plainBytes) throws Exception{
		
		byte[] cipherBytes = null;
        
		 //
        cipher.init(Cipher.ENCRYPT_MODE, key);
        
        // 
        cipherBytes = cipher.doFinal(plainBytes);

        return cipherBytes;

	}
	
	public static byte[] decrypt(byte[] encryptedBytes) throws Exception{
			
		byte[] plainBytes = null;
	            
		//
		cipher.init(Cipher.DECRYPT_MODE, key);
		
		//
		plainBytes  = cipher.doFinal(encryptedBytes);	

		return plainBytes;
		
	}	
}
