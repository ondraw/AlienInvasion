package kr.co.songs.android.AlienInvasion.com;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import kr.co.songs.android.AlienInvasion.minkml.CMinkException;

public abstract class ASyncMLObj extends Object {
	protected void Parse(Node xnNode) throws CMinkException {};
	protected abstract StringBuffer ToXmlString() throws CMinkException;
	

	protected Node getFirstChild(Node xnNode)
	{
		NodeList lstChild = xnNode.getChildNodes();
		int nCnt = lstChild.getLength();
		for(int i = 0; i < nCnt ; i++)
		{
			Node xnChild = lstChild.item(i);
			if(xnChild.getNodeType() == Node.ELEMENT_NODE)
				return xnChild;
		}		
		return null;
	}
	
	
	protected Node getLastChild(Node xnNode)
	{
		NodeList lstChild = xnNode.getChildNodes();
		int nCnt = lstChild.getLength();
		for(int i = nCnt - 1; i >= 0 ; i--)
		{
			Node xnChild = lstChild.item(i);
			if(xnChild.getNodeType() == Node.ELEMENT_NODE)
				return xnChild;
		}		
		return null;
	}
}
