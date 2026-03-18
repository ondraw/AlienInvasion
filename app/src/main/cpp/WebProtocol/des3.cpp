/***************************************************************************
* Copyright (c) 2000-2004, Future Systems, Inc. / Seoul, Republic of Korea *
* All Rights Reserved.                                                     *
*                                                                          *
* This document contains proprietary and confidential information.  No     *
* parts of this document or the computer program it embodies may be in     *
* any way copied, duplicated, reproduced, translated into a different      *
* programming language, or distributed to any person, company, or          *
* corporation without the prior written consent of Future Systems, Inc.    *
*                              Hyo Sun Hwang                               *
*                372-2 YangJae B/D 6th Floor, Seoul, Korea                 *
*                           +82-2-578-0581 (552)                           *
***************************************************************************/

/*--------------------- [ Version/Command in detais] ---------------------*\
Description : des.c
			(C-source file) Block Cipher DES - core function

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

/*************** Header files *********************************************/
#include "des3.h"

/*************** Constants ************************************************/

/*************** New Data Types *******************************************/
typedef struct{
	DWORD	RK1[DES3_NO_ROUNDKEY/3];
	DWORD	RK2[DES3_NO_ROUNDKEY/3];
	DWORD	RK3[DES3_NO_ROUNDKEY/3];
} DES3_ROUNDKEY_CTX;

/*************** Macros ***************************************************/

/*************** Global Variables *****************************************/
RET_VAL DES_KeySchedule(
		BYTE		*UserKey,		//	사용자 비밀키 입력
		DWORD		UserKeyLen,		//	사용자 비밀키의 바이트 수
		DES_ALG_INFO	*AlgInfo);	//	암호용/복호용 Round Key 생성/저장
void	DES_Encrypt(
		void		*CipherKey,		//	암/복호용 Round Key
		BYTE		*Data);			//	입출력을 위한 블록을 가리키는 pointer
void	DES_Decrypt(
		void		*CipherKey,		//	암/복호용 Round Key
		BYTE		*Data);			//	입출력을 위한 블록을 가리키는 pointer

void	DES3_Encrypt(
                     void		*CipherKey,
                     BYTE		*Data);
void	DES3_Decrypt(
                     void		*CipherKey,
                     BYTE		*Data);


/*************** Function *************************************************
* 
*/
RET_VAL DES3_KeySchedule(
		BYTE		*UserKey,		//	사용자 비밀키 입력
		DWORD		UserKeyLen,		//	사용자 비밀키의 바이트 수
		DES3_ALG_INFO	*AlgInfo)	//	암호용/복호용 Round Key 생성/저장
{
	DWORD		i;
	RET_VAL		ret;
	DES_ALG_INFO	DesInfo;
	DES3_ROUNDKEY_CTX	*RoundKey=(DES3_ROUNDKEY_CTX *)AlgInfo->RoundKey;

	//	UserKey의 길이가 부적절한 경우 error 처리
	if( (UserKeyLen!=2*DES_USER_KEY_LEN) && (UserKeyLen!=3*DES_USER_KEY_LEN) )
		return CTR_INVALID_USERKEYLEN;

	////	Step 1 : 라운드키 생성
	//	Key generation part
	ret = DES_KeySchedule(UserKey+0, 8, &DesInfo);
	if( ret!=CTR_SUCCESS )	return ret;
	for( i=0; i<DES_NO_ROUNDKEY; i++)
		RoundKey->RK1[i] = DesInfo.RoundKey[i];

	ret = DES_KeySchedule(UserKey+8, 8, &DesInfo);
	if( ret!=CTR_SUCCESS )	return ret;
	for( i=0; i<DES_NO_ROUNDKEY; i++)
		RoundKey->RK2[i] = DesInfo.RoundKey[i];

	if( UserKeyLen==3*DES_USER_KEY_LEN )
		ret = DES_KeySchedule(UserKey+16, 8, &DesInfo);
	else
		ret = DES_KeySchedule(UserKey+0, 8, &DesInfo);
	if( ret!=CTR_SUCCESS )	return ret;
	for( i=0; i<DES_NO_ROUNDKEY; i++)
		RoundKey->RK3[i] = DesInfo.RoundKey[i];

	return CTR_SUCCESS;
}

/*************** Function *************************************************
*	
*/
void	DES3_Encrypt(
		void		*CipherKey,		//	암/복호용 Round Key
		BYTE		*Data)			//	입출력을 위한 블록을 가리키는 pointer
{
	DES3_ROUNDKEY_CTX	*RoundKeyCtx=(DES3_ROUNDKEY_CTX *) CipherKey;

	DES_Encrypt(&(RoundKeyCtx->RK1), Data);
	DES_Decrypt(&(RoundKeyCtx->RK2), Data);
	DES_Encrypt(&(RoundKeyCtx->RK3), Data);
}

/*************** Function *************************************************
*	
*/
void	DES3_Decrypt(
		void		*CipherKey,		//	암/복호용 Round Key
		BYTE		*Data)			//	입출력을 위한 블록을 가리키는 pointer
{
	DES3_ROUNDKEY_CTX	*RoundKeyCtx=(DES3_ROUNDKEY_CTX *) CipherKey;

	DES_Decrypt(&(RoundKeyCtx->RK3), Data);
	DES_Encrypt(&(RoundKeyCtx->RK2), Data);
	DES_Decrypt(&(RoundKeyCtx->RK1), Data);
}

/*************** END OF FILE **********************************************/
