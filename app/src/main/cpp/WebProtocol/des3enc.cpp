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
Description : des3.c
			(C-source file) Block Cipher DES3 - mode function

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

/*************** Header files *********************************************/
#include "des3.h"

/*************** Assertions ***********************************************/

/*************** Definitions / Macros  ************************************/
#define BlockCopy(pbDst, pbSrc) {					\
	((DWORD *)(pbDst))[0] = ((DWORD *)(pbSrc))[0];	\
	((DWORD *)(pbDst))[1] = ((DWORD *)(pbSrc))[1];	\
}
#define BlockXor(pbDst, phSrc1, phSrc2) {			\
	((DWORD *)(pbDst))[0] = ((DWORD *)(phSrc1))[0]	\
						  ^ ((DWORD *)(phSrc2))[0];	\
	((DWORD *)(pbDst))[1] = ((DWORD *)(phSrc1))[1]	\
						  ^ ((DWORD *)(phSrc2))[1];	\
}

/*************** New Data Types *******************************************/

/*************** Global Variables *****************************************/

/*************** Prototypes ***********************************************/
void	DES3_Encrypt(
		void		*CipherKey,		//	æœ/∫π»£øÎ Round Key
		BYTE		*Data);			//	¿‘√‚∑¬¿ª ¿ß«— ∫Ì∑œ¿ª ∞°∏Æ≈∞¥¬ pointer
void	DES3_Decrypt(
		void		*CipherKey,		//	æœ/∫π»£øÎ Round Key
		BYTE		*Data);			//	¿‘√‚∑¬¿ª ¿ß«— ∫Ì∑œ¿ª ∞°∏Æ≈∞¥¬ pointer

RET_VAL ECB_DecFinal(
                     DES3_ALG_INFO	*AlgInfo,
                     BYTE		*PlainTxt,
                     DWORD		*PlainTxtLen);

RET_VAL CBC_DecFinal(
                     DES3_ALG_INFO	*AlgInfo,
                     BYTE		*PlainTxt,
                     DWORD		*PlainTxtLen);

RET_VAL OFB_DecFinal(
                     DES3_ALG_INFO	*AlgInfo,
                     BYTE		*PlainTxt,
                     DWORD		*PlainTxtLen);

RET_VAL CFB_DecFinal(
                     DES3_ALG_INFO	*AlgInfo,
                     BYTE		*PlainTxt,
                     DWORD		*PlainTxtLen);

bool	GetKeyIV(
                 BYTE	Key[DES3_USER_KEY_LEN],
                 DWORD	*KeyLen,
                 BYTE	IV[DES3_BLOCK_LEN],
                 DWORD	*IVLen);

/*************** Constants ************************************************/

/*************** Constants ************************************************/

/*************** Macros ***************************************************/

/*************** Global Variables *****************************************/

/*************** Function *************************************************
*
*/
void	DES3_SetAlgInfo(
		DWORD			ModeID,
		DWORD			PadType,
		BYTE			*IV,
		DES3_ALG_INFO	*AlgInfo)
{
	AlgInfo->ModeID = ModeID;
	AlgInfo->PadType = PadType;

	if( IV!=NULL )
		memcpy(AlgInfo->IV, IV, DES3_BLOCK_LEN);
	else
		memset(AlgInfo->IV, 0, DES3_BLOCK_LEN);
}

/*************** Function *************************************************
*
*/
static RET_VAL PaddSet(
			BYTE	*pbOutBuffer,
			DWORD	dRmdLen,
			DWORD	dBlockLen,
			DWORD	dPaddingType)
{
	DWORD dPadLen;

	switch( dPaddingType ) {
		case AI_NO_PADDING :
			if( dRmdLen==0 )	return 0;
			else				return CTR_DATA_LEN_ERROR;

		case AI_PKCS_PADDING :
			dPadLen = dBlockLen - dRmdLen;
			memset(pbOutBuffer+dRmdLen, (char)dPadLen, (int)dPadLen);
			return dPadLen;

		default :
			return CTR_FATAL_ERROR;
	}
}

/*************** Function *************************************************
*
*/
static RET_VAL PaddCheck(
			BYTE	*pbOutBuffer,
			DWORD	dBlockLen,
			DWORD	dPaddingType)
{
	DWORD i, dPadLen;

	switch( dPaddingType ) {
		case AI_NO_PADDING :
			return 0;			//	paddingµ» µ•¿Ã≈∏∞° 0πŸ¿Ã∆Æ¿”.

		case AI_PKCS_PADDING :
			dPadLen = pbOutBuffer[dBlockLen-1];
			if( ((int)dPadLen<=0) || (dPadLen>(int)dBlockLen) )
				return CTR_PAD_CHECK_ERROR;
			for( i=1; i<=dPadLen; i++)
				if( pbOutBuffer[dBlockLen-i] != dPadLen )
					return CTR_PAD_CHECK_ERROR;
			return dPadLen;

		default :
			return CTR_FATAL_ERROR;
	}
}

/**************************************************************************
*
*/
RET_VAL	DES3_EncInit(
		DES3_ALG_INFO	*AlgInfo)
{
	AlgInfo->BufLen = 0;
	if( AlgInfo->ModeID!=AI_ECB )
		memcpy(AlgInfo->ChainVar, AlgInfo->IV, DES3_BLOCK_LEN);
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL ECB_EncUpdate(
		DES3_ALG_INFO	*AlgInfo,		//	
		BYTE		*PlainTxt,		//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« pointer
		DWORD		PlainTxtLen,	//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(CipherTxt, AlgInfo->Buffer);
	DES3_Encrypt(ScheduledKey, CipherTxt);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) {
		BlockCopy(CipherTxt, PlainTxt);
		DES3_Encrypt(ScheduledKey, CipherTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//	control the case that PlainTxt and CipherTxt are the same buffer
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CBC_EncUpdate(
		DES3_ALG_INFO	*AlgInfo,		//	
		BYTE		*PlainTxt,		//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« pointer
		DWORD		PlainTxtLen,	//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	BlockXor(CipherTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	DES3_Encrypt(ScheduledKey, CipherTxt);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) {
		BlockXor(CipherTxt, CipherTxt-BlockLen, PlainTxt);
		DES3_Encrypt(ScheduledKey, CipherTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}
	BlockCopy(AlgInfo->ChainVar, CipherTxt-BlockLen);

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL OFB_EncUpdate(
		DES3_ALG_INFO	*AlgInfo,		//	
		BYTE		*PlainTxt,		//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« pointer
		DWORD		PlainTxtLen,	//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(CipherTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) {
		DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(CipherTxt, AlgInfo->ChainVar, PlainTxt);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CFB_EncUpdate(
		DES3_ALG_INFO	*AlgInfo,		//	
		BYTE		*PlainTxt,		//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« pointer
		DWORD		PlainTxtLen,	//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen + PlainTxtLen;

	//	No one block
	if( *CipherTxtLen<BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)PlainTxtLen);
		AlgInfo->BufLen += PlainTxtLen;
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that PlainTxt and CipherTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	memcpy(AlgInfo->Buffer+BufLen, PlainTxt, (int)(BlockLen - BufLen));
	PlainTxt += BlockLen - BufLen;
	PlainTxtLen -= BlockLen - BufLen;

	//	core part
	DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	BlockCopy(CipherTxt, AlgInfo->ChainVar);
	CipherTxt += BlockLen;
	while( PlainTxtLen>=BlockLen ) {
		DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, PlainTxt);
		BlockCopy(CipherTxt, AlgInfo->ChainVar);
		PlainTxt += BlockLen;
		CipherTxt += BlockLen;
		PlainTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, PlainTxt, (int)PlainTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + PlainTxtLen;
	*CipherTxtLen -= PlainTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL	DES3_EncUpdate(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« pointer
		DWORD		PlainTxtLen,	//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	switch( AlgInfo->ModeID ) {
		case AI_ECB :	return ECB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen,
											 CipherTxt, CipherTxtLen);
		case AI_CBC :	return CBC_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen,
											 CipherTxt, CipherTxtLen);
		case AI_OFB :	return OFB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen,
											 CipherTxt, CipherTxtLen);
		case AI_CFB :	return CFB_EncUpdate(AlgInfo, PlainTxt, PlainTxtLen,
											 CipherTxt, CipherTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

/**************************************************************************
*
*/
static RET_VAL ECB_EncFinal(
		DES3_ALG_INFO	*AlgInfo,		//	
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	DWORD		PaddByte;

	//	Padding
	PaddByte = PaddSet(AlgInfo->Buffer, BufLen, BlockLen, AlgInfo->PadType);
	if( PaddByte>BlockLen )		return PaddByte;

	if( PaddByte==0 ) {
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	core part
	BlockCopy(CipherTxt, AlgInfo->Buffer);
	DES3_Encrypt(ScheduledKey, CipherTxt);

	//
	*CipherTxtLen = BlockLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CBC_EncFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	DWORD		PaddByte;

	//	Padding
	PaddByte = PaddSet(AlgInfo->Buffer, BufLen, BlockLen, AlgInfo->PadType);
	if( PaddByte>BlockLen )		return PaddByte;

	if( PaddByte==0 ) {
		*CipherTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	core part
	BlockXor(CipherTxt, AlgInfo->Buffer, AlgInfo->ChainVar);
	DES3_Encrypt(ScheduledKey, CipherTxt);
	BlockCopy(AlgInfo->ChainVar, CipherTxt);

	//
	*CipherTxtLen = BlockLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL OFB_EncFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;
	DWORD		i;

	//	Check Output Memory Size
	*CipherTxtLen = BlockLen;

	//	core part
	DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	for( i=0; i<BufLen; i++)
		CipherTxt[i] = (BYTE) (AlgInfo->Buffer[i] ^ AlgInfo->ChainVar[i]);

	//
	*CipherTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CFB_EncFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*CipherTxtLen = BufLen;

	//	core part
	DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	memcpy(CipherTxt, AlgInfo->ChainVar, BufLen);

	//
	*CipherTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL	DES3_EncFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		*CipherTxtLen)	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
{
	switch( AlgInfo->ModeID ) {
		case AI_ECB :	return ECB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_CBC :	return CBC_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_OFB :	return OFB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		case AI_CFB :	return CFB_EncFinal(AlgInfo, CipherTxt, CipherTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

/**************************************************************************
*
*/
RET_VAL	DES3_DecInit(
		DES3_ALG_INFO	*AlgInfo)
{
	AlgInfo->BufLen = 0;
	if( AlgInfo->ModeID!=AI_ECB )
		memcpy(AlgInfo->ChainVar, AlgInfo->IV, DES3_BLOCK_LEN);
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL ECB_DecUpdate(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	¿‘∑¬µ«¥¬ æœ»£πÆ¿« pointer
		DWORD		CipherTxtLen,	//	¿‘∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*PlainTxt,		//	∆ÚπÆ¿Ã √‚∑¬µ… pointer
		DWORD		*PlainTxtLen)	//	√‚∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( CipherTxt==PlainTxt )	return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	DES3_Decrypt(ScheduledKey, PlainTxt);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) {
		BlockCopy(PlainTxt, CipherTxt);
		DES3_Decrypt(ScheduledKey, PlainTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CBC_DecUpdate(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	¿‘∑¬µ«¥¬ æœ»£πÆ¿« pointer
		DWORD		CipherTxtLen,	//	¿‘∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*PlainTxt,		//	∆ÚπÆ¿Ã √‚∑¬µ… pointer
		DWORD		*PlainTxtLen)	//	√‚∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN, BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( CipherTxt==PlainTxt )	return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	DES3_Decrypt(ScheduledKey, PlainTxt);
	BlockXor(PlainTxt, PlainTxt, AlgInfo->ChainVar);
	PlainTxt += BlockLen;

	if( CipherTxtLen<=BlockLen ) {
		BlockCopy(AlgInfo->ChainVar, AlgInfo->Buffer);
	}
	else {
		if( CipherTxtLen>BlockLen ) {
			BlockCopy(PlainTxt, CipherTxt);
			DES3_Decrypt(ScheduledKey, PlainTxt);
			BlockXor(PlainTxt, PlainTxt, AlgInfo->Buffer);
			CipherTxt += BlockLen;
			PlainTxt += BlockLen;
			CipherTxtLen -= BlockLen;
		}
		while( CipherTxtLen>BlockLen ) {
			BlockCopy(PlainTxt, CipherTxt);
			DES3_Decrypt(ScheduledKey, PlainTxt);
			BlockXor(PlainTxt, PlainTxt, CipherTxt-BlockLen);
			CipherTxt += BlockLen;
			PlainTxt += BlockLen;
			CipherTxtLen -= BlockLen;
		}
		BlockCopy(AlgInfo->ChainVar, CipherTxt-BlockLen);
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL OFB_DecUpdate(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	¿‘∑¬µ«¥¬ æœ»£πÆ¿« pointer
		DWORD		CipherTxtLen,	//	¿‘∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*PlainTxt,		//	∆ÚπÆ¿Ã √‚∑¬µ… pointer
		DWORD		*PlainTxtLen)	//	√‚∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(PlainTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) {
		DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(PlainTxt, AlgInfo->ChainVar, CipherTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
static RET_VAL CFB_DecUpdate(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	¿‘∑¬µ«¥¬ æœ»£πÆ¿« pointer
		DWORD		CipherTxtLen,	//	¿‘∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*PlainTxt,		//	∆ÚπÆ¿Ã √‚∑¬µ… pointer
		DWORD		*PlainTxtLen)	//	√‚∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen + CipherTxtLen;

	//	No one block
	if( BufLen+CipherTxtLen <= BlockLen ) {
		memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)CipherTxtLen);
		AlgInfo->BufLen += CipherTxtLen;
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}

	//	control the case that CipherTxt and PlainTxt are the same buffer
	if( PlainTxt==CipherTxt )
		return CTR_FATAL_ERROR;

	//	first block
	*PlainTxtLen = BufLen + CipherTxtLen;
	memcpy(AlgInfo->Buffer+BufLen, CipherTxt, (int)(BlockLen - BufLen));
	CipherTxt += BlockLen - BufLen;
	CipherTxtLen -= BlockLen - BufLen;

	//	core part
	DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(PlainTxt, AlgInfo->ChainVar, AlgInfo->Buffer);
	BlockCopy(AlgInfo->ChainVar, AlgInfo->Buffer);
	PlainTxt += BlockLen;
	while( CipherTxtLen>BlockLen ) {
		DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
		BlockXor(PlainTxt, AlgInfo->ChainVar, CipherTxt);
		BlockCopy(AlgInfo->ChainVar, CipherTxt);
		CipherTxt += BlockLen;
		PlainTxt += BlockLen;
		CipherTxtLen -= BlockLen;
	}

	//	save remained data
	memcpy(AlgInfo->Buffer, CipherTxt, (int)CipherTxtLen);
	AlgInfo->BufLen = (AlgInfo->BufLen&0xF0000000) + CipherTxtLen;
	*PlainTxtLen -= CipherTxtLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL	DES3_DecUpdate(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*CipherTxt, 	//	æœ»£πÆ¿Ã √‚∑¬µ… pointer
		DWORD		CipherTxtLen,	//	√‚∑¬µ«¥¬ æœ»£πÆ¿« πŸ¿Ã∆Æ ºˆ
		BYTE		*PlainTxt,		//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« pointer
		DWORD		*PlainTxtLen)	//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	switch( AlgInfo->ModeID ) {
		case AI_ECB :	return ECB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen,
											 PlainTxt, PlainTxtLen);
		case AI_CBC :	return CBC_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen,
											 PlainTxt, PlainTxtLen);
		case AI_OFB :	return OFB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen,
											 PlainTxt, PlainTxtLen);
		case AI_CFB :	return CFB_DecUpdate(AlgInfo, CipherTxt, CipherTxtLen,
											 PlainTxt, PlainTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

/**************************************************************************
*
*/
RET_VAL ECB_DecFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	∆ÚπÆ¿Ã √‚∑¬µ… pointer
		DWORD		*PlainTxtLen)	//	√‚∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	RET_VAL		ret;

	//	Check Output Memory Size
	if( BufLen==0 ) {
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}
	*PlainTxtLen = BlockLen;

	if( BufLen!=BlockLen )	return CTR_CIPHER_LEN_ERROR;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	DES3_Decrypt(ScheduledKey, PlainTxt);

	//	Padding Check
	ret = PaddCheck(PlainTxt, BlockLen, AlgInfo->PadType);
	if( ret==(DWORD)-3 )	return CTR_PAD_CHECK_ERROR;
	if( ret==(DWORD)-1 )	return CTR_FATAL_ERROR;

	*PlainTxtLen = BlockLen - ret;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL CBC_DecFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	∆ÚπÆ¿Ã √‚∑¬µ… pointer
		DWORD		*PlainTxtLen)	//	√‚∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BlockLen=DES3_BLOCK_LEN, BufLen=AlgInfo->BufLen;
	RET_VAL		ret;

	//	Check Output Memory Size
	if( BufLen==0 ) {
		*PlainTxtLen = 0;
		return CTR_SUCCESS;
	}
	*PlainTxtLen = BlockLen;

	if( BufLen!=BlockLen )	return CTR_CIPHER_LEN_ERROR;

	//	core part
	BlockCopy(PlainTxt, AlgInfo->Buffer);
	DES3_Decrypt(ScheduledKey, PlainTxt);
	BlockXor(PlainTxt, PlainTxt, AlgInfo->ChainVar);
	BlockCopy(AlgInfo->ChainVar, AlgInfo->Buffer);

	//	Padding Check
	ret = PaddCheck(PlainTxt, BlockLen, AlgInfo->PadType);
	if( ret==(DWORD)-3 )	return CTR_PAD_CHECK_ERROR;
	if( ret==(DWORD)-1 )	return CTR_FATAL_ERROR;

	*PlainTxtLen = BlockLen - ret;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL OFB_DecFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	∆ÚπÆ¿Ã √‚∑¬µ… pointer
		DWORD		*PlainTxtLen)	//	√‚∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		i, BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen;

	//	core part
	DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	for( i=0; i<BufLen; i++)
		PlainTxt[i] = (BYTE) (AlgInfo->Buffer[i] ^ AlgInfo->ChainVar[i]);

	*PlainTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}


/**************************************************************************
*
*/
RET_VAL CFB_DecFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	∆ÚπÆ¿Ã √‚∑¬µ… pointer
		DWORD		*PlainTxtLen)	//	√‚∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	DWORD		*ScheduledKey=AlgInfo->RoundKey;
	DWORD		BufLen=AlgInfo->BufLen;

	//	Check Output Memory Size
	*PlainTxtLen = BufLen;

	//	core part
	DES3_Encrypt(ScheduledKey, AlgInfo->ChainVar);
	BlockXor(AlgInfo->ChainVar, AlgInfo->ChainVar, AlgInfo->Buffer);
	memcpy(PlainTxt, AlgInfo->ChainVar, BufLen);

	*PlainTxtLen = BufLen;

	//
	return CTR_SUCCESS;
}

/**************************************************************************
*
*/
RET_VAL	DES3_DecFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE		*PlainTxt,		//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« pointer
		DWORD		*PlainTxtLen)	//	¿‘∑¬µ«¥¬ ∆ÚπÆ¿« πŸ¿Ã∆Æ ºˆ
{
	switch( AlgInfo->ModeID ) {
		case AI_ECB :	return ECB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_CBC :	return CBC_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_OFB :	return OFB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		case AI_CFB :	return CFB_DecFinal(AlgInfo, PlainTxt, PlainTxtLen);
		default :		return CTR_FATAL_ERROR;
	}
}

/*************** END OF FILE **********************************************/




bool	GetKeyIV(
		BYTE	Key[DES3_USER_KEY_LEN],
		DWORD	*KeyLen,
		BYTE	IV[DES3_BLOCK_LEN],
		DWORD	*IVLen)
{
	DWORD	ch, i, j;
	FILE	*pfile;

	if( (pfile=fopen("key.dat", "r"))==NULL )
	{
		return false;
	}

	fscanf(pfile, "%d", (int*)&j);
	*KeyLen = j;
	for( i=0; i<j; i++) {
		fscanf(pfile, "%X", (int*)&ch);
		Key[i] = (BYTE) ch;
	}

	fscanf(pfile, "%d", (int*)&j);
	if( j!=DES3_BLOCK_LEN ) {
		*IVLen = 0;
		for( i=0; i<DES3_BLOCK_LEN; i++)
			IV[i] = (BYTE) 0;
	}
	else {
		*IVLen = DES3_BLOCK_LEN;
		for( i=0; i<DES3_BLOCK_LEN; i++) {
			fscanf(pfile, "%X", (int*)&ch);
			IV[i] = (BYTE) ch;
		}
	}

	fclose(pfile);
	return true;
}



int	MinkDes(		
		BYTE	*pinData,
		DWORD   pInDataSize,
		BYTE	**ppoutData,
		DWORD	EncType,
		DWORD	ModeType,
		DWORD	PadType)
{
	int nResult = 0;
//	int   nReadUnit = 1024;
	BYTE *pOutData;
	DWORD nReadTotal = 0;
	DWORD nWriteBefor = 0;
	DWORD nWriteTotal = 0;
	/*
	BYTE	UserKey[DES3_USER_KEY_LEN]={
		0x01, 0x23, 0x45, 0x67, 0x89, 
		0xAB, 0xCD, 0xEF, 0xf0, 0xe1, 
		0xd2, 0xc3, 0xb4, 0xa5, 0x96, 
		0x87, 0xfe, 0xdc, 0xba, 0x98, 
		0x76, 0x54, 0x32, 0x10};
	BYTE	IV[DES3_BLOCK_LEN] = {0xFE ,0xDC ,0xBA ,0x98, 0x76, 0x54, 0x32, 0x10};
	*/
	BYTE	UserKey[DES3_USER_KEY_LEN]={
		0xBC ,0xDB ,0xC8 ,0xA3 ,0xC7 ,
		0xD0 ,0xBE ,0xE7 ,0xC0 ,0xAF ,
		0xB9 ,0xCE ,0xB9 ,0xDA ,0xC7 ,
		0xFC ,0xBC ,0xF6 ,0xC0 ,0xAF ,
		0xBB ,0xF3 ,0xC3 ,0xB5};
	BYTE	IV[DES3_BLOCK_LEN] = {0xBC ,0xF6 ,0xC0 ,0xAF ,0xBB ,0xF3 ,0xC3 ,0xB5};

	BYTE	SrcData[1024+32], DstData[1024+32];
//	DWORD	UKLen = 24, IVLen = 8, SrcLen, DstLen;
	DWORD	UKLen = 24, SrcLen, DstLen;

	RET_VAL	ret;
	DES3_ALG_INFO	AlgInfo;

	
	pOutData = NULL;




	//GetKeyIV(UserKey, &UKLen, IV, &IVLen);

	DES3_SetAlgInfo(ModeType, PadType, IV, &AlgInfo);
	ret = DES3_KeySchedule(UserKey, UKLen, &AlgInfo);
	if( ret!=CTR_SUCCESS )	
	{
		nResult = -9;
		goto EXIT;
	}

	/*pOutData = (BYTE*)malloc(1024);
	nResult = 1024;
	*ppoutData = pOutData;
	return nResult;*/

	if( EncType==0 ) {		//	Encryption
		
		ret = DES3_EncInit(&AlgInfo);
		if( ret!=CTR_SUCCESS )	
		{
			nResult = -3;
			goto EXIT;
		}	
		
		for(  ;  ;  ) {
			SrcLen = 1024;

			if(pInDataSize == nReadTotal) break;
			else if(pInDataSize - nReadTotal < 1024)
				SrcLen = pInDataSize - nReadTotal;
			else if(nReadTotal > pInDataSize)
				break;
			
			memcpy(SrcData,pinData+nReadTotal,SrcLen);
			nReadTotal+= SrcLen;
			DstLen = 1024;
			
			ret = DES3_EncUpdate(&AlgInfo, SrcData, SrcLen, DstData, &DstLen);
			if( ret!=CTR_SUCCESS )	
			{
				nResult = -8;
				goto EXIT;
			}
			nWriteBefor = nWriteTotal;
			nWriteTotal += DstLen;
			if(pOutData == NULL)
				pOutData = (BYTE*)malloc(nWriteTotal);
			else
				pOutData = (BYTE*)realloc(pOutData,nWriteTotal);
			memcpy(pOutData+nWriteBefor,DstData,DstLen);
		}
		
		
		DstLen = 1024;
		ret = DES3_EncFinal(&AlgInfo, DstData, &DstLen);
		if( ret!=CTR_SUCCESS )	
		{
			nResult = -4;
			goto EXIT;
		}
		nWriteBefor = nWriteTotal;
		nWriteTotal += DstLen;
		if(pOutData == NULL)
			pOutData = (BYTE*)malloc(nWriteTotal);
		else
			pOutData = (BYTE*)realloc(pOutData,nWriteTotal);
		memcpy(pOutData+nWriteBefor,DstData,DstLen);
		
		nResult = nWriteTotal;

		
		
	}
	else {					//	Decryption
		ret = DES3_DecInit(&AlgInfo);
		if( ret!=CTR_SUCCESS )	
		{
			nResult = -5;
			goto EXIT;
		}
		for(  ;  ;  ) 
		{
			
			SrcLen = 1024;
			if(pInDataSize == nReadTotal) break;
			else if(pInDataSize - nReadTotal < 1024)
				SrcLen = pInDataSize - nReadTotal;
			else if(nReadTotal > pInDataSize)
				break;
			memcpy(SrcData,pinData+nReadTotal,SrcLen);
			nReadTotal+= SrcLen;

			DstLen = 1024;
			ret = DES3_DecUpdate(&AlgInfo, SrcData, SrcLen, DstData, &DstLen);
			if( ret!=CTR_SUCCESS )	
			{
				nResult = -6;
				goto EXIT;
			}

			nWriteBefor = nWriteTotal;
			nWriteTotal += DstLen;
			if(pOutData == NULL)
				pOutData = (BYTE*)malloc(nWriteTotal);
			else
				pOutData = (BYTE*)realloc(pOutData,nWriteTotal);
			memcpy(pOutData+nWriteBefor,DstData,DstLen);
		}

		DstLen = 1024;
		ret = DES3_DecFinal(&AlgInfo, DstData, &DstLen);
		if( ret!=CTR_SUCCESS )	
		{
			nResult = -7;
			goto EXIT;
		}
		nWriteBefor = nWriteTotal;
		nWriteTotal += DstLen;
		if(pOutData == NULL)
				pOutData = (BYTE*)malloc(nWriteTotal);
		else
			pOutData = (BYTE*)realloc(pOutData,nWriteTotal);

		if( pOutData == NULL )	
		{
			nResult = -8;
			goto EXIT;
		}
		memcpy(pOutData+nWriteBefor,DstData,DstLen);
		nResult = nWriteTotal;
	}

EXIT:	
	if(nResult < 0 && pOutData)
	{
		free(pOutData);
		pOutData = NULL;
	}
	*ppoutData = pOutData;
	return nResult;
}



int	MinkEnDes3File(		
		char	*infile,
		char	*outfile,
		DWORD	EncType,
		DWORD	ModeType,
		DWORD	PadType)
{	
	int nResult = 0;
	FILE *pIn=NULL,*pOut=NULL;
	
	//BYTE	UserKey[DES3_USER_KEY_LEN];
	//BYTE	IV[DES3_BLOCK_LEN];

	BYTE	UserKey[DES3_USER_KEY_LEN]={
		0xBC ,0xDB ,0xC8 ,0xA3 ,0xC7 ,
		0xD0 ,0xBE ,0xE7 ,0xC0 ,0xAF ,
		0xB9 ,0xCE ,0xB9 ,0xDA ,0xC7 ,
		0xFC ,0xBC ,0xF6 ,0xC0 ,0xAF ,
		0xBB ,0xF3 ,0xC3 ,0xB5};
	BYTE	IV[DES3_BLOCK_LEN] = {0xBC ,0xF6 ,0xC0 ,0xAF ,0xBB ,0xF3 ,0xC3 ,0xB5};

	BYTE	SrcData[1024+32], DstData[1024+32];
	//DWORD	UKLen, IVLen, SrcLen, DstLen;
	DWORD	UKLen = 24, IVLen = 8, SrcLen, DstLen;

	RET_VAL	ret;
	DES3_ALG_INFO	AlgInfo;

	
	GetKeyIV(UserKey, &UKLen, IV, &IVLen);	

	if( (pIn = fopen(infile, "rb"))==NULL ) 
	{
		nResult = -1;
		goto EXIT;
	}

	if( (pOut=fopen(outfile, "wb"))==NULL ) {
		nResult = -2;
		goto EXIT;
	}
	

	DES3_SetAlgInfo(ModeType, PadType, IV, &AlgInfo);
	ret = DES3_KeySchedule(UserKey, UKLen, &AlgInfo);
	if( ret!=CTR_SUCCESS )	
	{
		nResult = -9;
		goto EXIT;
	}

	if( EncType==0 ) {		//	Encryption
		ret = DES3_EncInit(&AlgInfo);
		if( ret!=CTR_SUCCESS )	
		{
			nResult = -3;
			goto EXIT;
		}

		for(  ;  ;  ) {
			SrcLen = fread(SrcData, sizeof(BYTE), 1024, pIn);
			if( SrcLen==0 )	break;

			DstLen = 1024;
			ret = DES3_EncUpdate(&AlgInfo, SrcData, SrcLen, DstData, &DstLen);
			if( ret!=CTR_SUCCESS )	
			{
				nResult = -8;
				goto EXIT;
			}

			fwrite(DstData, sizeof(BYTE), DstLen, pOut);
		}

		DstLen = 1024;
		ret = DES3_EncFinal(&AlgInfo, DstData, &DstLen);
		if( ret!=CTR_SUCCESS )	
		{
			nResult = -4;
			goto EXIT;
		}
		fwrite(DstData, sizeof(BYTE), DstLen, pOut);
	}
	else {					//	Decryption
		ret = DES3_DecInit(&AlgInfo);
		if( ret!=CTR_SUCCESS )	
		{
			nResult = -5;
			goto EXIT;
		}

		for(  ;  ;  ) {
			SrcLen = fread(SrcData, sizeof(BYTE), 1024, pIn);
			if( SrcLen==0 )	break;

			DstLen = 1024;
			ret = DES3_DecUpdate(&AlgInfo, SrcData, SrcLen, DstData, &DstLen);
			if( ret!=CTR_SUCCESS )	
			{
				nResult = -6;
				goto EXIT;
			}

			fwrite(DstData, sizeof(BYTE), DstLen, pOut);
		}

		DstLen = 1024;
		ret = DES3_DecFinal(&AlgInfo, DstData, &DstLen);
		if( ret!=CTR_SUCCESS )	
		{
			nResult = -7;
			goto EXIT;
		}
		fwrite(DstData, sizeof(BYTE), DstLen, pOut);
	}


EXIT:
	if(pIn)
		fclose(pIn);
	if(pOut)
		fclose(pOut);

	return nResult;
}