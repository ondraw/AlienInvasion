package kr.co.songs.android.AlienInvasion.minkml;



public class CMinkParam 
{
	protected long		 	nLength; //
	protected String		 	sValue; 
	protected String 			sMode; //in,out,result
	protected String 			sType; //int,String	
	protected String 			sName; //
	
	public CMinkParam()
	{
		nLength = 0;
		sValue = null;
		sMode = null;
		sType = null;
		sName = null;
		
	}

	public CMinkParam(String sValue)
	{
		setValue(sValue);
	}
	
	public CMinkParam(String sValue,String sMode,String sType)
	{
		setValue(sValue,sMode,sType);
	}
	
	public CMinkParam(String sName,String sValue,String sMode,String sType)
	{
		setValue(sName,sValue,sMode,sType);
	}
	
	/**
	 * Param의 값을 넣는다.
	 * @param sValue
	 * sType = "string"
	 * sMode = "in","result"
	 * sName = ""
	 */
	public void setValue(String sValue)
	{
		setValue("",sValue,"in","table");
	}
	
	/**
	 * Param의 값을 넣는다.
	 * @param sValue 
	 * @param sMode = in,out,ref,result
	 * @param sType = string,int,table
	 */
	public void setValue(String sValue,String sMode,String sType)
	{
		setValue("",sValue,sMode,sType);
	}
	
	/**
	 * Param의 값을 넣는다.
	 * @param sName
	 * @param sValue
	 * @param sMode = in,out,ref,result
	 * @param sType = string,int,table
	 */
	public void setValue(String sName,String sValue,String sMode,String sType)
	{
		if(sValue == null || sMode == null) return;	
		
		this.sName = sName;
		this.sMode = sMode;
		this.sValue = sValue;	
		this.sType = sType;
		nLength = sValue.getBytes().length;
	}
	
	public String getType() { return sType;}
	public String getName() { return sName;}
	public String getValue() { return sValue;}
	public String getMode() { return sMode;}
	public long   getLength() { return nLength;}
	

}
