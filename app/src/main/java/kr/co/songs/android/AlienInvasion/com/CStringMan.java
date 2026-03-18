package kr.co.songs.android.AlienInvasion.com;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class CStringMan
{
	
	
	public static String getString(Node xnNode) {
		NodeList xnNodeList = xnNode.getChildNodes();
		int nChildDataLen = xnNodeList.getLength();
		StringBuffer bfData = new StringBuffer();
		for(int x = 0; x < nChildDataLen; x++)
		{
			Node xnTempData = xnNodeList.item(x);
			if(xnTempData != null && (xnTempData.getNodeType() == Node.TEXT_NODE || 
					xnTempData.getNodeType() == Node.CDATA_SECTION_NODE))
				bfData.append(xnTempData.getNodeValue());
		}
		//m_sData = bfData.toString();
		return bfData.toString();
	}
}
