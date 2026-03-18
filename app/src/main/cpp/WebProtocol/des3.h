

/*--------------------- [ Version/Command in detais] ---------------------*\
Description : des3.h
			(head file) head file for des3.c : Block Cipher DES3

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

#ifndef _DES3_H
#define _DES3_H

/*************** Header files *********************************************/
#include "des.h"

/*************** Assertions ***********************************************/

/*************** Macros ***************************************************/

/*************** Definitions / Macros *************************************/
////	DES에 관련된 상수들
#define DES3_BLOCK_LEN			DES_BLOCK_LEN			//	(=8)in BYTEs
#define DES3_USER_KEY_LEN		(3*DES_USER_KEY_LEN)	//	(16,24) in BYTEs
#define DES3_NO_ROUNDKEY		(3*DES_NO_ROUNDKEY)		//	in DWORDs

/*************** New Data Types *******************************************/
////	DES3..
typedef struct{
	DWORD		ModeID;						//	ECB or CBC
	DWORD		PadType;					//	블록암호의 Padding type
	BYTE		IV[DES3_BLOCK_LEN];			//	Initial Vector
	BYTE		ChainVar[DES3_BLOCK_LEN];	//	Chaining Variable
	BYTE		Buffer[DES3_BLOCK_LEN];		//	Buffer for unfilled block
	DWORD		BufLen; 					//	Buffer의 유효 바이트 수
	DWORD		RoundKey[DES3_NO_ROUNDKEY];	//	
} DES3_ALG_INFO;

/*************** Constant (Error Code) ************************************/

/*************** Prototypes ***********************************************/
////	데이타 타입 DES3_ALG_INFO에 mode, padding 종류 및 IV 값을 초기화한다.
void	DES3_SetAlgInfo(
		DWORD			ModeID,
		DWORD			PadType,
		BYTE			*IV,
		DES3_ALG_INFO	*AlgInfo);

////	입력된 DES3_USER_KEY_LEN바인트의 비밀키로 라운드 키 생성
RET_VAL DES3_KeySchedule(
		BYTE			*UserKey,		//	사용자 비밀키를 입력함.
		DWORD			UserKeyLen,
		DES3_ALG_INFO	*AlgInfo);		//	암복호용 Round Key가 저장됨.

////	Init/Update/Final 형식을 암호화.
RET_VAL	DES3_EncInit(
		DES3_ALG_INFO	*AlgInfo);
RET_VAL	DES3_EncUpdate(
		DES3_ALG_INFO	*AlgInfo,
		BYTE			*PlainTxt,		//	평문이 입력됨.
		DWORD			PlainTxtLen,
		BYTE			*CipherTxt, 	//	암호문이 출력됨.
		DWORD			*CipherTxtLen);
RET_VAL	DES3_EncFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE			*CipherTxt, 	//	암호문이 출력됨.
		DWORD			*CipherTxtLen);

////	Init/Update/Final 형식을 복호화.
RET_VAL	DES3_DecInit(
		DES3_ALG_INFO	*AlgInfo);
RET_VAL	DES3_DecUpdate(
		DES3_ALG_INFO	*AlgInfo,
		BYTE			*CipherTxt,		//	암호문이 입력됨.
		DWORD			CipherTxtLen,
		BYTE			*PlainTxt,		//	복호문이 출력됨.
		DWORD			*PlainTxtLen);
RET_VAL	DES3_DecFinal(
		DES3_ALG_INFO	*AlgInfo,
		BYTE			*PlainTxt,		//	복호문이 출력됨.
		DWORD			*PlainTxtLen);


int	MinkDes(		
		BYTE	*pinData,
		DWORD   pInDataSize,
		BYTE	**ppoutData,
		DWORD	EncType,
		DWORD	ModeType = AI_ECB,
		DWORD	PadType = AI_PKCS_PADDING);

int	MinkEnDes3File(		
		char	*infile,
		char	*outfile,
		DWORD	EncType,
		DWORD	ModeType = AI_ECB,
		DWORD	PadType = AI_PKCS_PADDING);
/*************** END OF FILE **********************************************/
#endif	//	_DES3_H
