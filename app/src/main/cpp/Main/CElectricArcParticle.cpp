//
//  CElectricArcParticle.cpp
//  SongGL
//

#include <math.h>
#include <stdlib.h>
#include "IHWorld.h"
#include "CElectricArcParticle.h"

static float ElectricRandomUnit()
{
    return ((float)(rand() % 200) / 100.0f) - 1.0f;
}

static void NormalizeVector3(float* x,float* y,float* z)
{
    float len = sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    if(len < 0.0001f)
    {
        *x = 1.0f;
        *y = 0.0f;
        *z = 0.0f;
        return;
    }
    *x /= len;
    *y /= len;
    *z /= len;
}

static void CrossVector3(float ax,float ay,float az,float bx,float by,float bz,float* x,float* y,float* z)
{
    *x = ay * bz - az * by;
    *y = az * bx - ax * bz;
    *z = ax * by - ay * bx;
}

static void BuildPerpendicularBasis(float nx,float ny,float nz,float* ux,float* uy,float* uz,float* vx,float* vy,float* vz)
{
    if(fabsf(ny) < 0.92f)
        CrossVector3(0.0f, 1.0f, 0.0f, nx, ny, nz, ux, uy, uz);
    else
        CrossVector3(1.0f, 0.0f, 0.0f, nx, ny, nz, ux, uy, uz);
    NormalizeVector3(ux, uy, uz);
    CrossVector3(nx, ny, nz, *ux, *uy, *uz, vx, vy, vz);
    NormalizeVector3(vx, vy, vz);
}

CElectricArcParticle::CElectricArcParticle(IHWorld* pWorld)
{
    mpWorld = pWorld;
    mState = SPRITE_RUN;
    mfRadius = 1.0f;
    mfThicknessScale = 1.0f;
    mnAliveTime = 190;
    mnElapsedTime = 0;
    memset(&mPosition, 0, sizeof(mPosition));
    memset(mArrVertices, 0, sizeof(mArrVertices));
    memset(mnVertexCount, 0, sizeof(mnVertexCount));
}

CElectricArcParticle::~CElectricArcParticle()
{
}

int CElectricArcParticle::Initialize(SPoint *pPosition,SVector *pvDirection)
{
    return Initialize(pPosition, 2.0f);
}

int CElectricArcParticle::Initialize(SPoint *pPosition,float fRadius)
{
    return Initialize(pPosition, fRadius, 1.0f);
}

int CElectricArcParticle::Initialize(SPoint *pPosition,float fRadius,float fThicknessScale)
{
    mState = SPRITE_RUN;
    mnElapsedTime = 0;
    memcpy(&mPosition, pPosition, sizeof(mPosition));
    mfRadius = fRadius;
    mfThicknessScale = fThicknessScale;
    if(mfThicknessScale < 0.45f) mfThicknessScale = 0.45f;
    if(mfThicknessScale > 1.4f) mfThicknessScale = 1.4f;
    if(mfRadius < 2.2f) mfRadius = 2.2f;
    if(mfRadius > 20.0f) mfRadius = 20.0f;

    BuildArc(0, 1.00f);
    BuildArc(1, 0.95f);
    BuildArc(2, 1.06f);
    BuildArc(3, 0.91f);
    BuildArc(4, 1.11f);
    BuildArc(5, 0.87f);
    return E_SUCCESS;
}

void CElectricArcParticle::BuildArc(int nArcIndex,float fRadiusScale)
{
    const int nSegments = 14;
    const int nStride = 3 * nSegments;
    float radius = mfRadius * fRadiusScale;
    float nx = ElectricRandomUnit();
    float ny = ElectricRandomUnit();
    float nz = ElectricRandomUnit();
    NormalizeVector3(&nx, &ny, &nz);

    float ux;
    float uy;
    float uz;
    float vx;
    float vy;
    float vz;
    BuildPerpendicularBasis(nx, ny, nz, &ux, &uy, &uz, &vx, &vy, &vz);

    float startAngle = ((float)(rand() % 360)) * PI / 180.0f;
    float endAngle = startAngle + ((140.0f + (rand() % 120)) * PI / 180.0f);
    float* pVertex = &mArrVertices[nArcIndex * nStride];

    mnVertexCount[nArcIndex] = nSegments;
    for(int i = 0; i < nSegments; i++)
    {
        float t = (float)i / (float)(nSegments - 1);
        float angle = startAngle + (endAngle - startAngle) * t;
        float shellX = ux * cosf(angle) + vx * sinf(angle);
        float shellY = uy * cosf(angle) + vy * sinf(angle);
        float shellZ = uz * cosf(angle) + vz * sinf(angle);
        float shellRadius = radius * (0.98f + 0.05f * sinf(t * PI * 2.0f));
        float jitter = radius * 0.08f * ElectricRandomUnit();
        float x = shellX * (shellRadius + jitter);
        float y = shellY * (shellRadius + jitter * 0.6f);
        float z = shellZ * (shellRadius + jitter);

        if(i != 0 && i != nSegments - 1)
        {
            float zigX = nx * (radius * 0.12f * ElectricRandomUnit());
            float zigY = ny * (radius * 0.12f * ElectricRandomUnit());
            float zigZ = nz * (radius * 0.12f * ElectricRandomUnit());
            x += zigX + ux * (radius * 0.08f * ElectricRandomUnit());
            y += zigY + uy * (radius * 0.08f * ElectricRandomUnit());
            z += zigZ + uz * (radius * 0.08f * ElectricRandomUnit());
        }

        pVertex[i * 3 + 0] = mPosition.x + x;
        pVertex[i * 3 + 1] = mPosition.y + y;
        pVertex[i * 3 + 2] = mPosition.z + z;
    }
}

void CElectricArcParticle::RenderBeginCore(int nTime)
{
    mnElapsedTime += nTime;
    if(mnElapsedTime >= mnAliveTime)
        mState = SPRITE_READYDELETE;
}

int CElectricArcParticle::RenderBegin(int nTime)
{
    return E_SUCCESS;
}

int CElectricArcParticle::Render()
{
    if(mState != SPRITE_RUN) return E_SUCCESS;

    float fAlpha = 1.0f - ((float)mnElapsedTime / (float)mnAliveTime);
    if(fAlpha < 0.0f) fAlpha = 0.0f;

    SGLCAMERA* pCamera = mpWorld->GetCamera();
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    for(int nPass = 0; nPass < 2; nPass++)
    {
        for(int i = 0; i < 6; i++)
        {
            GLfloat ribbonVertex[14 * 2 * 3];
            float* pCenter = &mArrVertices[i * 3 * 14];

            for(int j = 0; j < mnVertexCount[i]; j++)
            {
                int prevIndex = j == 0 ? 0 : j - 1;
                int nextIndex = j == mnVertexCount[i] - 1 ? mnVertexCount[i] - 1 : j + 1;

                float* pPrev = &pCenter[prevIndex * 3];
                float* pNow = &pCenter[j * 3];
                float* pNext = &pCenter[nextIndex * 3];

                float tangentX = pNext[0] - pPrev[0];
                float tangentY = pNext[1] - pPrev[1];
                float tangentZ = pNext[2] - pPrev[2];
                NormalizeVector3(&tangentX, &tangentY, &tangentZ);

                float viewX = pCamera->viewPos.x - pNow[0];
                float viewY = pCamera->viewPos.y - pNow[1];
                float viewZ = pCamera->viewPos.z - pNow[2];
                NormalizeVector3(&viewX, &viewY, &viewZ);

                float sideX;
                float sideY;
                float sideZ;
                CrossVector3(tangentX, tangentY, tangentZ, viewX, viewY, viewZ, &sideX, &sideY, &sideZ);
                if(fabsf(sideX) + fabsf(sideY) + fabsf(sideZ) < 0.001f)
                    CrossVector3(tangentX, tangentY, tangentZ, 0.0f, 1.0f, 0.0f, &sideX, &sideY, &sideZ);
                NormalizeVector3(&sideX, &sideY, &sideZ);

                float width = mfRadius * (nPass == 0 ? 0.052f : 0.022f) * mfThicknessScale;
                width *= 0.88f + 0.14f * sinf(((float)j / (float)(mnVertexCount[i] - 1)) * PI);

                int n6i = j * 6;
                ribbonVertex[n6i + 0] = pNow[0] + sideX * width;
                ribbonVertex[n6i + 1] = pNow[1] + sideY * width;
                ribbonVertex[n6i + 2] = pNow[2] + sideZ * width;
                ribbonVertex[n6i + 3] = pNow[0] - sideX * width;
                ribbonVertex[n6i + 4] = pNow[1] - sideY * width;
                ribbonVertex[n6i + 5] = pNow[2] - sideZ * width;
            }

            if(nPass == 0)
                glColor4f(0.26f, 0.74f, 1.0f, fAlpha * 0.42f);
            else
                glColor4f(0.96f, 1.0f, 1.0f, fAlpha * 0.72f);
            glVertexPointer(3, GL_FLOAT, 0, ribbonVertex);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, mnVertexCount[i] * 2);
        }
    }

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    return E_SUCCESS;
}

int CElectricArcParticle::RenderEnd()
{
    return E_SUCCESS;
}

void CElectricArcParticle::GetPosition(SPoint* pOut)
{
    memcpy(pOut, &mPosition, sizeof(mPosition));
}
