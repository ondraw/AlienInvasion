/**
* - Project : Mobile Office(Mink) Solution
*
* - Copyright 2009 by Solution Development Team, thinkM, Inc., All rights reserved.
*
* - This software is proprietary information
* of thinkM, Inc. You shall not
* disclose such Confidential Information and shall use it
* only in accordance with the terms of the license agreement
* you entered into with thinkM.
*/

package kr.co.songs.android.AlienInvasion.enc;

/**
 * Represents an exception that can occur using the EncryptionTool
 * @version $Id: EncryptionException.java,v 1.1 2010/02/01 00:22:54 songhh Exp $
 */
public class EncryptionException extends java.lang.Exception {

    /**
	 * 
	 */
	private static final long serialVersionUID = 3259587865027308067L;

	/**
     * Creates a new instance of <code>EncryptionException</code> without detail message.
     */
    public EncryptionException() {
    }

    /**
     * Constructs an instance of <code>EncryptionException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public EncryptionException(String msg) {
        super(msg);
    }

    /**
     * Constructs an instance of <code>EncryptionException</code> with the specified cause.
     * @param cause the cause.
     */
    public EncryptionException(Throwable cause) {
        super(cause);
    }

    /**
     * Constructs an instance of <code>EncryptionException</code> with the specified details.
     * @param msg the detail message.
     * @param cause the cause.
     */
    public EncryptionException(String msg, Throwable cause) {
        super(msg, cause);
    }

}
