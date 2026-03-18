package kr.co.songs.android.AlienInvasion.minkml;

public class CColInfo extends CMinkParam {	
	
	public CColInfo(String sName, String sType, String sLength) 
	{
		super();
		if(sLength.length() > 0) nLength = Integer.parseInt(sLength);
		this.sType = sType;
		this.sName = sName;
		
	}
}
