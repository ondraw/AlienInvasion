//
//  CMS3DModel.cpp
//  SongGL
//
//  Created by Song Hohak on 11. 11. 24..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#include <iostream>
#include "CMS3DModel.h"
#include "CModelBound.h"
#include "CPicking.h"

CMS3DModel::Groupf::Groupf() :
vertices(0),
normals(0),
texCoord(0)
{
}

CMS3DModel::Groupf::~Groupf()
{
	if (vertices)
	{
		delete[] vertices;
	}
    
	if (normals)
	{
		delete[] normals;
	}
    
	if (texCoord)
	{
		delete[] texCoord;
	}
	
	
}

CMS3DModel::CMS3DModel() :
triangles(0),
groups(0),
joints(0),
numberAnimations(0),
animations(0),
worldVertices(0),
running(GL_FALSE),
currentAnimation(-1),
nextAnimation(-1),
elapsedTime(0),
useQuaternion(GL_TRUE),
dynamicBoundingSphere(0)
{
	
//	pfinalmulMaxtix = NULL;
	resetf();
}

CMS3DModel::~CMS3DModel()
{
	resetf();
//	if(pfinalmulMaxtix)
//	{
//		delete[] pfinalmulMaxtix;
//		pfinalmulMaxtix = 0;
//	}
}

SGLResult CMS3DModel::initf(CModelData& currentModelCore)
{
	GLint i;
    
	resetf();
    
	modelCore = &currentModelCore;
	
	worldVertices = new GLfloat[3*modelCore->numberVertices];
	
	if (!worldVertices)
	{
		resetf();
		
		return SGL_OUT_OF_MEMORY_ERROR;
	}
	
	triangles = new Trianglef[modelCore->numberTriangles];
	
	if (!triangles)
	{
		resetf();
		
		return SGL_OUT_OF_MEMORY_ERROR;
	}
    
	groups = new Groupf[modelCore->numberGroups];
	
	if (!groups)
	{
		resetf();
		
		return SGL_OUT_OF_MEMORY_ERROR;
	}
    
	for (i = 0; i < modelCore->numberGroups; i++)
	{
		groups[i].vertices = new GLfloat[9*modelCore->groups[i].numberTriangles];
		groups[i].normals = new GLfloat[9*modelCore->groups[i].numberTriangles];
        
		if (groups[i].vertices == 0 || groups[i].normals == 0)
		{
			resetf();
            
			return SGL_OUT_OF_MEMORY_ERROR;
		}
        
		if (modelCore->materials[modelCore->groups[i].materialIndex].textureAvailable)
		{
			groups[i].texCoord = new GLfloat[6*modelCore->groups[i].numberTriangles];
            
			if (groups[i].texCoord == 0)
			{
				resetf();
                
				return SGL_OUT_OF_MEMORY_ERROR;
			}
		}
	}
    
	joints = new Jointf[modelCore->numberJoints];
    
	if (!joints)
	{
		resetf();
		
		return SGL_OUT_OF_MEMORY_ERROR;
	}
    
    //Added By Song 2011.04.07
    sglLoadIdentityf(finalBeforeMatrix);
	
	return SGL_NO_ERROR;	
}

//Added By Song 2012.03.06 모델을 초기화, 위치설정할때 update()를 해야 finalMatrix가 초기화된다. 
//미사일을 발사하자마자 적이 터지는 현상은 생성되자마자 충돌검사를 할때 문제가 생긴다.
void CMS3DModel::InitFinalMatrix()
{
// Deleted by Song 2012.05.10 충돌검사에서 center를 가져올때 model의 position을 가져온다.
    
//    sglLoadIdentityf(finalMatrix);
//    
//    //
//    // This is the position and orientation of the model in the real world
//    //	
//    sglTranslateMatrixf(finalMatrix, position[0], position[1], position[2]);
//    if (useQuaternion)
//    {
//        GLfloat	rotationMatrix[16];
//        GLfloat	quaternion[4];
//        
//        sglAngleToQuaternionf(quaternion, orientation[0], orientation[1], orientation[2]);
//        sglQuaternionToMatrixf(rotationMatrix, quaternion);
//        
//        sglMultMatrixf(finalMatrix, finalMatrix, rotationMatrix);
//    }
//    else
//    {
//        sglRotateRzRyRxMatrixf(finalMatrix, orientation[0], orientation[1], orientation[2]);
//    }
//    
//    sglScaleMatrixf(finalMatrix, scale[0], scale[1], scale[2]);
}

//Added By Song 2011.07.07 finally매트릭스를 계산한다.
bool CMS3DModel::calFinallyMatrix()
{
    
    if(mbeforePosition[0] == position[0] && mbeforePosition[1] == position[1] && mbeforePosition[2] == position[2] &&
       mbeforeOreination[0] == orientation[0] && mbeforeOreination[1] == orientation[1] && mbeforeOreination[2] == orientation[2]) {
        
        if(pfinalmulMaxtix == NULL) 
        {
            if(mbMoreOpre)
                return true;
            else
                return false;
        }
        else if(pfinalmulMaxtix &&
                pfinalmulMaxtix[0] == mbeforeMatrix[0]   && pfinalmulMaxtix[1]   == mbeforeMatrix[1] && pfinalmulMaxtix[2]   == mbeforeMatrix[2] &&
                pfinalmulMaxtix[4] == mbeforeMatrix[4]   && pfinalmulMaxtix[5]   == mbeforeMatrix[5] && pfinalmulMaxtix[6]   == mbeforeMatrix[6] &&
                pfinalmulMaxtix[8] == mbeforeMatrix[8]   && pfinalmulMaxtix[9]   == mbeforeMatrix[9] && pfinalmulMaxtix[10]  == mbeforeMatrix[10] &&
                pfinalmulMaxtix[12] == mbeforeMatrix[12] && pfinalmulMaxtix[13]  == mbeforeMatrix[13]&& pfinalmulMaxtix[14]  == mbeforeMatrix[14] )
        {
            if(mbMoreOpre)
                return true;
            else
                return false;
        }
    }
    
    sglLoadIdentityf(finalMatrix);
    
    //
    // This is the position and orientation of the model in the real world
    //	
    sglTranslateMatrixf(finalMatrix, position[0], position[1], position[2]);
    if (useQuaternion)
    {
        GLfloat	rotationMatrix[16];
        GLfloat	quaternion[4];
        
        sglAngleToQuaternionf(quaternion, orientation[0], orientation[1], orientation[2]);
        sglQuaternionToMatrixf(rotationMatrix, quaternion);
        
        sglMultMatrixf(finalMatrix, finalMatrix, rotationMatrix);
    }
    else
    {
        sglRotateRzRyRxMatrixf(finalMatrix, orientation[0], orientation[1], orientation[2]);
    }
    
    sglScaleMatrixf(finalMatrix, scale[0], scale[1], scale[2]);
    
    //Added By Song 2011.02.21 탱크의 머리부분을 프로그램적으로 회전할때 또는 이동할때 등등을
    //사용한다.
    if(pfinalmulMaxtix)
    {
        sglMultMatrixf(finalMatrix, finalMatrix, pfinalmulMaxtix);
        
        memcpy(mbeforeMatrix, pfinalmulMaxtix, sizeof(mbeforeMatrix));
    }
    
    memcpy(mbeforePosition, position, sizeof(mbeforePosition));
    memcpy(mbeforeOreination, orientation, sizeof(mbeforePosition));
    
    mbMoreOpre = true;
    return true;  
}

void CMS3DModel::GetCenter(GLfloat *fOutCenter)
{ 
    if(dynamicBoundingSphere == 0) return;
    return dynamicBoundingSphere->getCenterf(fOutCenter);
}


GLvoid CMS3DModel::updatef(GLuint time)
{
	GLushort i;
    
	if (!modelCore)
		return;
    
	//
	// Assume that the animation is not finished
	//
	running = GL_TRUE;
	
	if (nextAnimation != -1 && currentAnimation == -1)
	{
		currentAnimation = nextAnimation;
		
		nextAnimation = -1;
	}
	
	//
	// Handle the animation
	//
	if (currentAnimation != -1)
	{
		if (elapsedTime < animations[currentAnimation].start)
		{
			elapsedTime = animations[currentAnimation].start;
		}
		
		elapsedTime += time;
		
		//
		// If the elapsed time is larger than the current duration
		//
		if (elapsedTime > animations[currentAnimation].end)
		{
			//
			// if loop, then begin from start
			//
			if (animations[currentAnimation].loop)
			{
				if (animations[currentAnimation].end > 0)
				{
					elapsedTime = animations[currentAnimation].start + elapsedTime % animations[currentAnimation].end;
				}
				
				if (elapsedTime > animations[currentAnimation].end)
					elapsedTime = animations[currentAnimation].end;				
			}
			else
			{
				//
				// if there is another animation, continue with this one
				//
				if (nextAnimation != -1)
				{	
					if (animations[currentAnimation].end > 0)
					{
						elapsedTime = animations[nextAnimation].start + elapsedTime % animations[currentAnimation].end;
					}
					
					currentAnimation = nextAnimation;					
					nextAnimation = -1;
					
					if (elapsedTime > animations[currentAnimation].end)
					{
						if (animations[currentAnimation].end > 0)
						{
							elapsedTime = animations[currentAnimation].start + elapsedTime % animations[currentAnimation].end;
						}
						
						if (elapsedTime > animations[currentAnimation].end)
						{
							elapsedTime = animations[currentAnimation].end;
                            
							running = GL_FALSE;
						}
					}
				}
				else
				{
					elapsedTime = animations[currentAnimation].end;
					
					running = GL_FALSE;
				}
			}
		}
		
	}
	else
	{
		elapsedTime = 0;
		
		running = GL_FALSE;
	}
    
    //Modified By Song 2011.07.07 위치와 방향이 동일하면 false
    if(calFinallyMatrix() == false && running == false) 
        return ;
    
    //calFinallyMatrix CoreThread에서 실행되면 BeginUpdate에서는 아래의 로직을 타야한다. 그래야 화면에 그려지는데 
    //CoreBeginUpdate에서 위치를 연산해버리면 BeginUpdate에서는 밑의 연산을 타지 않기때문에 화면에 그려질 데이터가 없단다.
    //밑의 로직을 타기때문에 MoreOpre가 필요하지 않음.
    mbMoreOpre = false;
  
	
	for (i = 0; i < modelCore->numberJoints; i++)
	{
		GLushort k;
        
		GLshort	currentFrame;
		
		sglLoadIdentityf(joints[i].finalMatrix);
        
		if (modelCore->joints[i].fatherJoint != -1)
		{
			//
			// 5. Consider the father matrix
			//
			sglMultMatrixf(joints[i].finalMatrix, joints[modelCore->joints[i].fatherJoint].finalMatrix, joints[i].finalMatrix);
		}
		else
		{
			sglMultMatrixf(joints[i].finalMatrix, finalMatrix, joints[i].finalMatrix);
		}
        
		//
		// 4. Bring back the Matrix to the joint
		//		
		sglMultMatrixf(joints[i].finalMatrix, joints[i].finalMatrix, modelCore->joints[i].jointMatrix);
		
		currentFrame = -1;
		for (k = 0; k < modelCore->joints[i].numberKeyframeTranslations; k++)
		{
			if (elapsedTime < modelCore->joints[i].keyframeTranslations[k].time)
			{
				break;
			}
			currentFrame++;
		}
		
		if (currentFrame >= 0)
		{
			GLfloat	localTranslation[3] = {0.0f, 0.0f, 0.0f};
			
			localTranslation[0] = modelCore->joints[i].keyframeTranslations[currentFrame].translation[0];
			localTranslation[1] = modelCore->joints[i].keyframeTranslations[currentFrame].translation[1];
			localTranslation[2] = modelCore->joints[i].keyframeTranslations[currentFrame].translation[2];
			
			if (currentFrame+1 < modelCore->joints[i].numberKeyframeTranslations && modelCore->joints[i].keyframeTranslations[currentFrame+1].time <= animations[currentAnimation].end)
			{
				GLuint	delta = modelCore->joints[i].keyframeTranslations[currentFrame+1].time - modelCore->joints[i].keyframeTranslations[currentFrame].time;
				GLuint	factor = elapsedTime - modelCore->joints[i].keyframeTranslations[currentFrame].time;
				
				localTranslation[0] += (modelCore->joints[i].keyframeTranslations[currentFrame+1].translation[0] - modelCore->joints[i].keyframeTranslations[currentFrame].translation[0])*((GLfloat)factor/delta);
				localTranslation[1] += (modelCore->joints[i].keyframeTranslations[currentFrame+1].translation[1] - modelCore->joints[i].keyframeTranslations[currentFrame].translation[1])*((GLfloat)factor/delta);
				localTranslation[2] += (modelCore->joints[i].keyframeTranslations[currentFrame+1].translation[2] - modelCore->joints[i].keyframeTranslations[currentFrame].translation[2])*((GLfloat)factor/delta);
			}
			
			//
			// 3. Translate the matrix with the local translations
			//
			sglTranslateMatrixf(joints[i].finalMatrix, localTranslation[0], localTranslation[1], localTranslation[2]);
		}		
        
		currentFrame = -1;
		for (k = 0; k < modelCore->joints[i].numberKeyframeRotations; k++)
		{
			if (elapsedTime < modelCore->joints[i].keyframeRotations[k].time)
			{
				break;
			}
			currentFrame++;
		}
		if (currentFrame >= 0)
		{
			GLfloat	localRotation[3] = {0.0f, 0.0f, 0.0f};
            
			localRotation[0] = modelCore->joints[i].keyframeRotations[currentFrame].rotation[0];
			localRotation[1] = modelCore->joints[i].keyframeRotations[currentFrame].rotation[1];
			localRotation[2] = modelCore->joints[i].keyframeRotations[currentFrame].rotation[2];
            
			if (useQuaternion)
			{
				GLfloat rotationMatrix[16];
				GLfloat	quaternion0[4];
				
				sglAngleToQuaternionf(quaternion0, localRotation[0], localRotation[1], localRotation[2]);
                
				if (currentFrame+1 < modelCore->joints[i].numberKeyframeRotations && modelCore->joints[i].keyframeRotations[currentFrame+1].time <= animations[currentAnimation].end)
				{
					GLfloat	quaternion1[4];
					GLuint	delta = modelCore->joints[i].keyframeRotations[currentFrame+1].time - modelCore->joints[i].keyframeRotations[currentFrame].time;
					GLuint	factor = elapsedTime - modelCore->joints[i].keyframeRotations[currentFrame].time;
                    
					sglAngleToQuaternionf(quaternion1, modelCore->joints[i].keyframeRotations[currentFrame+1].rotation[0], modelCore->joints[i].keyframeRotations[currentFrame+1].rotation[1], modelCore->joints[i].keyframeRotations[currentFrame+1].rotation[2]);
					sglSlerpf(quaternion0, quaternion0, quaternion1, (GLfloat)factor/delta);
				}
				
				sglQuaternionToMatrixf(rotationMatrix, quaternion0);
				
				sglMultMatrixf(joints[i].finalMatrix, joints[i].finalMatrix, rotationMatrix);
			}
			else
			{
				if (currentFrame+1 < modelCore->joints[i].numberKeyframeRotations && modelCore->joints[i].keyframeRotations[currentFrame+1].time <= animations[currentAnimation].end)
				{
					GLuint	delta = modelCore->joints[i].keyframeRotations[currentFrame+1].time - modelCore->joints[i].keyframeRotations[currentFrame].time;
					GLuint	factor = elapsedTime - modelCore->joints[i].keyframeRotations[currentFrame].time;
                    
					localRotation[0] += (modelCore->joints[i].keyframeRotations[currentFrame+1].rotation[0] - modelCore->joints[i].keyframeRotations[currentFrame].rotation[0])*((GLfloat)factor/delta);
					localRotation[1] += (modelCore->joints[i].keyframeRotations[currentFrame+1].rotation[1] - modelCore->joints[i].keyframeRotations[currentFrame].rotation[1])*((GLfloat)factor/delta);
					localRotation[2] += (modelCore->joints[i].keyframeRotations[currentFrame+1].rotation[2] - modelCore->joints[i].keyframeRotations[currentFrame].rotation[2])*((GLfloat)factor/delta);
				}
				//
				// 2. Rotate the matrix with the local rotations
				//
				sglRotateRzRyRxMatrixf(joints[i].finalMatrix, localRotation[0], localRotation[1], localRotation[2]);
			}				
		}		
        
		//
		// 1. Move the matrix to the origin.
		//
		sglMultMatrixf(joints[i].finalMatrix, joints[i].finalMatrix, modelCore->joints[i].inverseJointMatrix);
	}
	
	for (i = 0; i < modelCore->numberVertices; i++)
	{
		if (modelCore->jointsOfVertices[i] != -1)
		{
			sglMultMatrixVectorf(&worldVertices[i*3], joints[modelCore->jointsOfVertices[i]].finalMatrix, &modelCore->localVertices[i*3]);
		}
		else
		{
			sglMultMatrixVectorf(&worldVertices[i*3], finalMatrix, &modelCore->localVertices[i*3]);
		}
	}
    
	for (i = 0; i < modelCore->numberTriangles; i++)
	{
		for (GLushort k = 0; k < 3; k++)
		{
			if (modelCore->jointsOfVertices[modelCore->triangles[i].vertexIndices[k]] != -1)
			{
				sglMultMatrixVectorf(&triangles[i].worldNormals[k*3], joints[modelCore->jointsOfVertices[modelCore->triangles[i].vertexIndices[k]]].finalMatrix, &modelCore->triangles[i].normals[k*3], 0.0f);
			}
			else
			{
				sglMultMatrixVectorf(&triangles[i].worldNormals[k*3], finalMatrix, &modelCore->triangles[i].normals[k*3], 0.0f);
			}
		}
	}	
}

GLvoid CMS3DModel::renderf() const
{
    GLuint  nkIndex;
    Groupf*	tmpGroups;
    
    
    CModelData::Groupf*	tmpModelCoreGroups;
    
    GLuint  n6Size = sizeof(GLfloat)*6;
    GLuint  n3Size = sizeof(GLfloat)*3;
    GLuint  n9Size = sizeof(GLfloat)*9;
    
    
    GLuint  tmpnVertexIndices;
    
    GLfloat* tmpTextCoord;
    GLfloat* tmpGroupTextCoord;
    
    GLfloat* tmpWorldNormals;
    GLfloat* tmpGroupNormals;
    
    
    GLfloat* tmpGroupVertices;
    
	GLboolean	textureOn = GL_FALSE;
    
	if (!modelCore)
		return;
	
	glEnableClientState(GL_VERTEX_ARRAY);
    
	glEnableClientState(GL_NORMAL_ARRAY);
	// Normals are assigned later
    
	
    
	for(GLushort i = 0; i < modelCore->numberGroups; i++)
	{
        tmpGroups = &groups[i];
        tmpModelCoreGroups = &modelCore->groups[i];
		if (tmpModelCoreGroups->materialIndex != -1)
		{
			glEnable(GL_LIGHTING);
#ifdef ANDROID
            glEnable(GL_LIGHT0); //Android
#endif
//-----------------------------------------------------------------------------------------------------------------------
// 재질 설정 
//-----------------------------------------------------------------------------------------------------------------------
// 상단에서 한꺼번에 재질을 정의 하였다.
//			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, modelCore->materials[tmpModelCoreGroups->materialIndex].ambient);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, modelCore->materials[tmpModelCoreGroups->materialIndex].diffuse);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, modelCore->materials[tmpModelCoreGroups->materialIndex].specular);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, modelCore->materials[tmpModelCoreGroups->materialIndex].emission);
//			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, modelCore->materials[tmpModelCoreGroups->materialIndex].shininess);
//-----------------------------------------------------------------------------------------------------------------------
#if 0 //DEBUG
            HLog("Mat =am:(%f,%f,%f,%f)\n,dif=(%f,%f,%f,%f)\n,sp=(%f,%f,%f,%f)\n,em=(%f,%f,%f,%f),sh=%f\n",
                   modelCore->materials[modelCore->groups[i].materialIndex].ambient[0],
                   modelCore->materials[modelCore->groups[i].materialIndex].ambient[1],
                   modelCore->materials[modelCore->groups[i].materialIndex].ambient[2],
                   modelCore->materials[modelCore->groups[i].materialIndex].ambient[3],
                   modelCore->materials[modelCore->groups[i].materialIndex].diffuse[0],
                   modelCore->materials[modelCore->groups[i].materialIndex].diffuse[1],
                   modelCore->materials[modelCore->groups[i].materialIndex].diffuse[2],
                   modelCore->materials[modelCore->groups[i].materialIndex].diffuse[3],
                   modelCore->materials[modelCore->groups[i].materialIndex].specular[0],
                   modelCore->materials[modelCore->groups[i].materialIndex].specular[1],
                   modelCore->materials[modelCore->groups[i].materialIndex].specular[2],
                   modelCore->materials[modelCore->groups[i].materialIndex].specular[3],
                   modelCore->materials[modelCore->groups[i].materialIndex].emission[0],
                   modelCore->materials[modelCore->groups[i].materialIndex].emission[1],
                   modelCore->materials[modelCore->groups[i].materialIndex].emission[2],
                   modelCore->materials[modelCore->groups[i].materialIndex].emission[3],
                   modelCore->materials[modelCore->groups[i].materialIndex].shininess);

#endif
            
//현재 어두운 것은  GL_EMISSION = 0 이다.  shiness가 0이면 빗나는것이 없다. sepectar는 반사값이 0이면 빤짝이는 효과를 낼수가 없다.
//            GLfloat mat_specular[]={0.5,0.5,0.5, 1.0};
//            GLfloat mat_emission[]={0.2,0.2,0.2, 1.0};
//            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
//            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
//            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80);
            

            
			if (modelCore->materials[tmpModelCoreGroups->materialIndex].textureAvailable)
			{
				textureOn = GL_TRUE;
                
				glEnable(GL_TEXTURE_2D);
                
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                
				glBindTexture(GL_TEXTURE_2D, modelCore->materials[tmpModelCoreGroups->materialIndex].textureName);
			}
			else
			{
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                
				glDisable(GL_TEXTURE_2D);				
                
				textureOn = GL_FALSE;				
			}						
		}
		else
		{
#ifdef ANDROID
            glDisable(GL_LIGHT0); //Android
#endif
			glDisable(GL_LIGHTING);
            
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            
			glDisable(GL_TEXTURE_2D);				
			
			textureOn = GL_FALSE;
		}	
		
		for (GLushort k = 0; k < tmpModelCoreGroups->numberTriangles; k++)
		{
//            이 내용을 아래와 같이 축소하였다.
//            for (GLushort m = 0; m < 3; m++)
//			{
//				GLushort n;
//                
//				if (textureOn)
//				{
//                    
//					for (n = 0; n < 2; n++)
//					{
//						tmpGroups->texCoord[k*6+m*2+n] = modelCore->triangles[tmpModelCoreGroups->trianglesIndices[k]].texCoord[m*2+n];
//					}
//				}
//                
//				for (n = 0; n < 3; n++)
//				{
//					tmpGroups->normals[k*9+m*3+n] = triangles[tmpModelCoreGroups->trianglesIndices[k]].worldNormals[m*3+n];
//                    
//					tmpGroups->vertices[k*9+m*3+n] = worldVertices[modelCore->triangles[tmpModelCoreGroups->trianglesIndices[k]].vertexIndices[m]*3+n];
//				}
//			}

            
            if (textureOn)
            {
                nkIndex = k*6;
                tmpTextCoord = modelCore->triangles[tmpModelCoreGroups->trianglesIndices[k]].texCoord;
                tmpGroupTextCoord = tmpGroups->texCoord;
                memcpy(tmpGroupTextCoord + nkIndex, tmpTextCoord, n6Size);
            }
            
            nkIndex = k*9;
            tmpWorldNormals = triangles[tmpModelCoreGroups->trianglesIndices[k]].worldNormals;
            tmpGroupNormals = tmpGroups->normals;
            memcpy(tmpGroupNormals + nkIndex, tmpWorldNormals, n9Size);
            
            tmpGroupVertices = tmpGroups->vertices;
            tmpnVertexIndices = modelCore->triangles[tmpModelCoreGroups->trianglesIndices[k]].vertexIndices[0]*3;
            memcpy(tmpGroupVertices + nkIndex, worldVertices + tmpnVertexIndices, n3Size);
            
            tmpnVertexIndices = modelCore->triangles[tmpModelCoreGroups->trianglesIndices[k]].vertexIndices[1]*3;
            memcpy(tmpGroupVertices + nkIndex + 3, worldVertices + tmpnVertexIndices, n3Size);
            
            tmpnVertexIndices = modelCore->triangles[tmpModelCoreGroups->trianglesIndices[k]].vertexIndices[2]*3;
            memcpy(tmpGroupVertices + nkIndex + 6, worldVertices + tmpnVertexIndices, n3Size);
		}
        
		if (textureOn)
		{
			glTexCoordPointer(2, GL_FLOAT, 0, tmpGroups->texCoord);
		}
		glNormalPointer(GL_FLOAT, 0, tmpGroups->normals);
		glVertexPointer(3, GL_FLOAT, 0, tmpGroups->vertices);
        
		glDrawArrays(GL_TRIANGLES, 0, 3 * tmpModelCoreGroups->numberTriangles);
	}
	
	//
	// Disable everything
	//
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
#ifdef ANDROID
    glDisable(GL_LIGHT0); //Android
#endif
	glDisable(GL_LIGHTING);
}

GLfloat* CMS3DModel::getWorldTriangle(int nGroup,int nTrangleIndex,int nVertexIndex)
{ 
	return &worldVertices[modelCore->triangles[modelCore->groups[nGroup].trianglesIndices[nTrangleIndex]].
                          vertexIndices[nVertexIndex]*3];
    //
    //	return &(worldVertices[modelCore->triangles[modelCore->groups[nGroup].trianglesIndices[nTrangleIndex]].vertexIndices[nVertexIndex]*3]);
    
}

bool    CMS3DModel::DevGetPoIndex(float *fV,int &nGroupIndex,int &nTriangleIndex,int &nVerIndex)
{
    float *fpp;
    nGroupIndex = 0;
    for (int i = 0; i < modelCore->numberGroups; i++)
    {
        nGroupIndex = i;
        
        int nTrCnt = modelCore->groups[i].numberTriangles;
        for(int j = 0; j < nTrCnt; j++)
        {
            nTriangleIndex = j;
            
            for(int z = 0; z < 3; z++)
            {
                nVerIndex = z;
                int nx = modelCore->triangles[modelCore->groups[i].trianglesIndices[j]].vertexIndices[z];
                fpp = &modelCore->localVertices[nx * 3];
                
//                HLog("--[%d] %f,%f,%f\n",nx,fpp[0],fpp[1],fpp[2]);
                
                float f1 = fV[0] - fpp[0];
                if(f1 < 0) f1=-f1;
                
                float f2 = fV[1] - fpp[1];
                if(f2 < 0) f2=-f2;
                
                
                float f3 = fV[2] - fpp[2];
                if(f3 < 0) f3=-f3;
                
                if(f1 < 0.000001 && f2 < 0.000001 && f3 < 0.000001)
                {
                    return true;
                }
            }
        }
    }
    
    return false;
}


bool CMS3DModel::IsPicking(CPicking* pPick,int nWinPosX,int nWinPosY)
{
    float *v1,*v2,*v3;
    GLint nwin[2];
    nwin[0] = nWinPosX;
    nwin[1] = nWinPosY;
    
    if(modelCore == 0) return false;
    
    SPoint nearPoint;
    SPoint farPoint;
    pPick->GetUnProject(nwin, nearPoint, 0);
    pPick->GetUnProject(nwin, farPoint, 1);
    
    for(GLushort i = 0; i < modelCore->numberGroups; i++)
	{
		for (GLushort k = 0; k < modelCore->groups[i].numberTriangles; k++)
		{
            v1 = getWorldTriangle(i,k,0);
            v2 = getWorldTriangle(i,k,1);
            v3 = getWorldTriangle(i,k,2);
            if(pPick->sglIsPicking(v1, v2, v3,nearPoint,farPoint)) 
                return true;
		}
	}
    return false;
}

bool CMS3DModel::IsPicking(CPicking* pPick,SPoint& nearPoint,SPoint& farPoint,int nWinPosX,int nWinPosY)
{
    float *v1,*v2,*v3;
    GLint nwin[2];
    nwin[0] = nWinPosX;
    nwin[1] = nWinPosY;
    
    if(modelCore == 0) return false;
    
//    SPoint nearPoint;
//    SPoint farPoint;
//    pPick->GetUnProject(nwin, nearPoint, 0);
//    pPick->GetUnProject(nwin, farPoint, 1);
    
    for(GLushort i = 0; i < modelCore->numberGroups; i++)
	{
		for (GLushort k = 0; k < modelCore->groups[i].numberTriangles; k++)
		{
            
            v1 = getWorldTriangle(i,k,0);
            v2 = getWorldTriangle(i,k,1);
            v3 = getWorldTriangle(i,k,2);
            if(pPick->sglIsPicking(v1, v2, v3,nearPoint,farPoint)) 
                return true;
		}
	}
    return false;
}



bool CMS3DModel::IsPickingZeroGroup(CPicking* pPick,int nWinPosX,int nWinPosY)
{
    float *v1,*v2,*v3;
    GLint nwin[2];
    int nCnt;
    nwin[0] = nWinPosX;
    nwin[1] = nWinPosY;
    
    if(modelCore == 0) return false;
    
    SPoint nearPoint;
    SPoint farPoint;
    pPick->GetUnProject(nwin, nearPoint, 0);
    pPick->GetUnProject(nwin, farPoint, 1);
    
    nCnt = modelCore->groups[0].numberTriangles;
    for (GLushort k = 0; k < nCnt; k++)
    {
        v1 = getWorldTriangle(0,k,0);
        v2 = getWorldTriangle(0,k,1);
        v3 = getWorldTriangle(0,k,2);
        if(pPick->sglIsPicking(v1, v2, v3,nearPoint,farPoint))
            return true;
    }
    return false;
}


GLvoid CMS3DModel::createDefaultAnimationf()
{
	if (!modelCore)
		return;
    
	if (animations)
	{
		delete[] animations;
	}
	numberAnimations = 1;
	
	animations = new Animationf[1];
	animations[0].start = 0;
	animations[0].end = modelCore->lastFrame*1000/modelCore->framesPerSecond;	
}

GLshort	CMS3DModel::getCurrentAnimationf()
{
	if (!modelCore)
		return -1;
	
	return currentAnimation;
}

GLvoid CMS3DModel::setNextAnimationf(GLshort next, GLboolean loop, GLboolean wait)
{	
	if (!modelCore)
		return;
    
	if (next >= numberAnimations)
		return;
	
	nextAnimation = next;
	
	if (nextAnimation != -1)
	{		
		animations[nextAnimation].loop = loop;
	}
	
	if (wait)
	{
		if (currentAnimation != -1)
		{
			animations[currentAnimation].loop = GL_FALSE;
		}
		else
		{
			running = GL_TRUE;
		}
	}
	else
	{
		currentAnimation = -1;
		elapsedTime = animations[nextAnimation].start;
		running = GL_TRUE;
	}
}

GLboolean CMS3DModel::isAnimationRunningf()
{
	if (!modelCore)
		return GL_FALSE;
    
	return running;
}

GLuint CMS3DModel::getElapsedTimef()
{
	if (!modelCore)
		return 0;
    
	return elapsedTime;
}

GLvoid CMS3DModel::setElapsedTimef(GLuint time)
{
	if (!modelCore)
		return;
    
	elapsedTime = time;
}

GLvoid CMS3DModel::setElapsedTimeByFramef(GLuint frame)
{
	if (!modelCore)
		return;
    
	elapsedTime = frame * 1000 / modelCore->framesPerSecond;
}

GLvoid CMS3DModel::resetf()
{
	modelCore = 0;
	
	if (worldVertices)
	{
		delete[] worldVertices;
		
		worldVertices = 0;
	}
    
	if (triangles)
	{
		delete[] triangles;
		
		triangles = 0;
	}
    
	if (groups)
	{
		delete[] groups;
		
		groups = 0;
	}
    
	if (joints)
	{
		delete[] joints;
		
		joints = 0;
	}	
    
	if (animations)
	{
		delete[] animations;
		
		animations = 0;
	}	
	
	position[0] = 0.0f;
	position[1] = 0.0f;
	position[2] = 0.0f;
    
	orientation[0] = 0.0f;
	orientation[1] = 0.0f;
	orientation[2] = 0.0f;
	
	scale[0] = 1.0f;
	scale[1] = 1.0f;
	scale[2] = 1.0f;
	
	if (dynamicBoundingSphere)
	{
		delete dynamicBoundingSphere;
		dynamicBoundingSphere = 0;
	}
	
	if ( pfinalmulMaxtix )
	{
		memset(pfinalmulMaxtix,0,sizeof(GLfloat)*16);
	}
    
    
    //Added By Song 2011.04.07
    //직임 또는 애니메이션 작업이 없어서 정점이 변경된 것이 없으면 매트릭스를 다시 구할 필요가 없다.
    mbeforePosition[0] = 0.0f;
	mbeforePosition[1] = 0.0f;
	mbeforePosition[2] = 0.0f;
    
    mbeforeOreination[0] = 0.0f;
	mbeforeOreination[1] = 0.0f;
	mbeforeOreination[2] = 0.0f;
    
    mbeforeMatrix[0] = 99999.9f;
}

GLvoid CMS3DModel::createBoundingSpheref(GLbyte level)
{
	dynamicBoundingSphere = new CModelBound(this, level);
}


//search 찿고자하는 조인트어레이 (즉 조인트별로 정점을 찾을 수가 있다.)
//searchSize => search[0] = 0,search[1] = 4   size는 2 즉 0,4번째 조인트에 충돌했는지를 찾는다.
GLboolean CMS3DModel::collidesBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, CMyModel& model, GLbyte* searchModel, GLubyte searchSizeModel, GLbyte* hitJointModel, GLboolean exact, GLbyte stopAtLevel)
{
    float modelPT[3];
    float PT[3];
    
    model.GetCenter(modelPT);
    GetCenter(PT);
    
    float fRadius = GetRadius();
    float fRadiusSum = (fRadius + model.GetRadius()) / 2.0f; //2.0f를 한이유는 객체가 서로 부딛히는 비율을 조금 작게 하였다.
    
    //(a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
    float fx = modelPT[0] - PT[0];
    float fy = modelPT[1] - PT[1];
    float fz = modelPT[2] - PT[2];
    
    dynamicBoundingSphere->updateDistanceSprite = fx * fx + fy * fy + fz * fz;
    if( dynamicBoundingSphere->updateDistanceSprite < (fRadiusSum * fRadiusSum))
        return true;
    return false;
}


GLboolean CMS3DModel::intersectsBoundingSpheref(GLbyte* search, GLubyte searchSize, GLbyte* hitJoint, GLfloat hitPoint[3], GLfloat start[3], GLfloat point[3], GLboolean exact, GLbyte stopAtLevel)
{
	if (dynamicBoundingSphere == 0)
		return GL_FALSE;
    
	return dynamicBoundingSphere->intersectsBoundingSpheref(search, searchSize, hitJoint, hitPoint, start, point, exact, stopAtLevel);
}


float CMS3DModel::GetCompactOtherSpriteInterval()
{
    return dynamicBoundingSphere->updateDistanceSprite;
}

float CMS3DModel::GetRadius()
{
    return dynamicBoundingSphere->getSimpleRadiusf();
}

