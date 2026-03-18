/*
 *  CMesh.cpp
 *  SongGL
 *
 *  Created by 호학 송 on 10. 11. 17..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <string.h>
#include <stdlib.h>
#include "CSGLMesh.h"

CSGLMesh::CSGLMesh(CListNode<SGLResource> *pResource)
{
	mplstMesh = pResource;
}

CSGLMesh::~CSGLMesh()
{
	if(mplstMesh != NULL)
	{
		int nSize = mplstMesh->Size();
		for(int i = 0; i < nSize; i++)
		{
			SGLResource *pResource = (*mplstMesh)[i];
			if(pResource->pVertex)
			{
				delete[] pResource->pVertex;
				pResource->pVertex = NULL;
			}
            
            if(pResource->pWorldVertex)
            {
                delete[] pResource->pWorldVertex;
				pResource->pWorldVertex = NULL;
            }
			
			if(pResource->pNormal)
			{
				delete[] pResource->pNormal;
				pResource->pNormal = NULL;
			}
			
			if(pResource->pshIndeces)
			{
				delete[] pResource->pshIndeces;
				pResource->pshIndeces = NULL;
			}
			
			if(pResource->Texture.pCoord)
			{
				delete[] pResource->Texture.pCoord;
				pResource->Texture.pCoord = NULL;
			}
			
			if(pResource->Texture.pImageBuffer)
			{
				//delete[] pResource->Texture.pImageBuffer;
                free(pResource->Texture.pImageBuffer);
				pResource->Texture.pImageBuffer = NULL;
			}
			
			if(pResource->Texture.glTextureID != 0)
			{
				glDeleteTextures(1, &pResource->Texture.glTextureID);
				pResource->Texture.glTextureID = 0;
			}
			
			
			delete pResource;
		}
		mplstMesh->Clear();
		
		delete mplstMesh;
	}
}