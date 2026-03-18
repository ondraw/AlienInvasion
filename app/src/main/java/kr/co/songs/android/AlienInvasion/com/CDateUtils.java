package kr.co.songs.android.AlienInvasion.com;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class CDateUtils {
	
	public enum DateToStringFormat { yyyyMMddHHmmss,yyyy_MM_dd,HH_mm_ss,HH_mm,yyyy_MM_dd_HH_mm_ss}
	
	/* String getFormat(DateToStringFormat format)
	 * return sFormat 
	 * 날짜 포멧을 yyyy/MM/dd 형태로 변경     05/19   by seo
	 */
	
	protected static String getFormat(DateToStringFormat format)
	{
		String sFormat = "";
		if(format == DateToStringFormat.yyyyMMddHHmmss) sFormat =  "yyyyMMddHHmmss";
		else if(format == DateToStringFormat.yyyy_MM_dd) sFormat =  "yyyy/MM/dd";
		else if(format == DateToStringFormat.HH_mm_ss) sFormat =  "HH:mm:ss";
		else if(format == DateToStringFormat.HH_mm) sFormat =  "HH:mm";
		else if(format == DateToStringFormat.yyyy_MM_dd_HH_mm_ss) sFormat =  "yyyy.MM.dd HH:mm:ss";	
			
		return sFormat;
	}
	
	public static String toDateString(Date date,DateToStringFormat format) {
		
		SimpleDateFormat formatter  = new SimpleDateFormat(getFormat(format));
		return formatter.format(date);
	}
	
	public static long toLong(Date date) 
	{		
		return date.getTime();
	}
	
	public static Date toStringDate(String dateS,DateToStringFormat format)
	{
		SimpleDateFormat formatter = new SimpleDateFormat(getFormat(format));
		try
		{
			return formatter.parse(dateS);
		} 
		catch (ParseException e) 
		{
			e.printStackTrace();
		}
		return null;
	}
}
