package kr.co.songs.android.AlienInvasion;

import kr.co.songs.android.AlienInvasion.minkml.CRMRecordSet;
import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;



public class BestScoreAdapter extends BaseAdapter 
{

	protected CRMRecordSet mRec;
	
	public BestScoreAdapter()
	{
		mRec = null;
	}
	
	public void setRecordSet(CRMRecordSet Rec)
	{
		mRec = Rec;
	}
	
	public CRMRecordSet getRecordSet() { return mRec;}
	
	@Override
	public int getCount() {
		if(mRec == null) return 0;
		return mRec.getCount();
	}

	@Override
	public Object getItem(int arg0) {
		if(mRec == null) return null;
		return mRec.MovePosition(arg0);
	}

	@Override
	public long getItemId(int arg0) {
		if(mRec == null) return 0;
		return arg0;
	}

	@Override
	public View getView(int nPos, View convertView, ViewGroup arg2) 
	{
		 View v = convertView;


         if (v == null)
         {
             LayoutInflater vi = (LayoutInflater) AlienInvasion.gMainActivity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
             v = vi.inflate(R.layout.bestscorecell, null);
         }
         try
         {
        	 mRec.MovePosition(nPos);
	       //device_id,rownum,user_id,total_score,rank,country,gold,message
	         TextView myTxt = (TextView) v.findViewById(R.id.txtName);
	         
	         String sName = mRec.getData(2);
	         if(sName.compareTo("***") == 0 || sName.length() == 0)
	        	 sName =  mRec.getData(5);
	         myTxt.setText(sName);
	         
	         myTxt = (TextView) v.findViewById(R.id.txtNumber);
	         myTxt.setText(mRec.getData(1));
	         
	         myTxt = (TextView) v.findViewById(R.id.txtMessage);
	         myTxt.setText(mRec.getData(7));
	         
	         myTxt = (TextView) v.findViewById(R.id.txtScore);
	         myTxt.setText(mRec.getData(3));
	         if(SongGLLib.GetUniqueID().compareTo(mRec.getData(0)) == 0)
	        	 v.setBackgroundColor(Color.rgb(255, 255, 0));	
	         else
	        	 v.setBackgroundColor(Color.TRANSPARENT);
	         
	         ImageView img = (ImageView)v.findViewById(R.id.imgRank);
	         
	          
	         
	         if(mRec.getData(4).length() > 0)
	         {
	        	 int[] arrid = new int[]{
		        		 R.drawable.rank_01,R.drawable.rank_02,R.drawable.rank_03,R.drawable.rank_04,R.drawable.rank_05,
		        		 R.drawable.rank_06,R.drawable.rank_07,R.drawable.rank_08,R.drawable.rank_09,R.drawable.rank_10,
		        		 R.drawable.rank_11,R.drawable.rank_12,R.drawable.rank_13,R.drawable.rank_14,R.drawable.rank_15,
		        		 R.drawable.rank_16,R.drawable.rank_17,R.drawable.rank_18,R.drawable.rank_19
		        		 };
	        	 
	        	 int in = Integer.parseInt(mRec.getData(4));
	        	 if(in > 0)
	        		 img.setImageResource(arrid[in - 1]);
	        	 else
	        		 img.setImageBitmap(null);
	         }
	         else
	        	 img.setImageBitmap(null);
	         
	         
	         ImageView img2 = (ImageView)v.findViewById(R.id.imgDayGrade);
	         
	         int nDayGrade = 0;
	         nDayGrade = Integer.parseInt(mRec.getData(8));
	         if(nDayGrade == 0)
	        	 img2.setImageResource(R.drawable.noarrow);
	         else if(nDayGrade > 0)
	        	 img2.setImageResource(R.drawable.uparrow);
	         else // < 0
	        	 img2.setImageResource(R.drawable.downarrow);
	         
	         myTxt = (TextView) v.findViewById(R.id.txtDayGrade);
	         myTxt.setText(mRec.getData(8));
         }
         catch(Exception ex)
         {
        	 Log.e("JAVA","getView " + ex.getLocalizedMessage());
         }
      
		return v;
	}
}
