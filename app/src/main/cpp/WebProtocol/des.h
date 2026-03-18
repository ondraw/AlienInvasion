

/*--------------------- [ Version/Command in detais] ---------------------*\
Description : des.h
			(head file) head file for des.c : Block Cipher DES

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

#ifndef _DES_H
#define _DES_H

/*************** Header files *********************************************/
#include <string>
#include <stdlib.h>
#include <memory>
//#include <windows.h>
#include <stdio.h>
//#include "cryptcom.h"


//////////////////////////////////////////////////////////////
//Added By Song 2010.04.23 IPhone
#define USER_LITTLE_ENDIAN
typedef unsigned long DWORD;
typedef unsigned char BYTE;
//////////////////////////////////////////////////////////////

/*************** Assertions ***********************************************/
////////	Define the Endianness	////////
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN

#if defined(USER_BIG_ENDIAN)
	#define BIG_ENDIAN
#elif defined(USER_LITTLE_ENDIAN)
    #ifndef LITTLE_ENDIAN
        #define LITTLE_ENDIAN
    #endif //LITTLE_ENDIAN
#else
	#if 0
		#define BIG_ENDIAN		//	Big-Endian machine with pointer casting
	#elif defined(_MSC_VER)
		#define LITTLE_ENDIAN	//	Little-Endian machine with pointer casting
	#else
		#error
	#endif
#endif

/*************** Macros ***************************************************/
////////	rotate by using shift operations	////////
#if defined(_MSC_VER)
	#define ROTL_DWORD(x, n) _lrotl((x), (n))
	#define ROTR_DWORD(x, n) _lrotr((x), (n))
#else
	#define ROTL_DWORD(x, n) ( (DWORD)((x) << (n)) | (DWORD)((x) >> (32-(n))) )
	#define ROTR_DWORD(x, n) ( (DWORD)((x) >> (n)) | (DWORD)((x) << (32-(n))) )
#endif

////////	reverse the byte order of DWORD(DWORD:4-bytes integer) and WORD.
#define ENDIAN_REVERSE_DWORD(dwS)	( (ROTL_DWORD((dwS),  8) & 0x00ff00ff)	\
									 | (ROTL_DWORD((dwS), 24) & 0xff00ff00) )

////////	move DWORD type to BYTE type and BYTE type to DWORD type
#if defined(BIG_ENDIAN)		////	Big-Endian machine
	#define BIG_B2D(B, D)		D = *(DWORD *)(B)
	#define BIG_D2B(D, B)		*(DWORD *)(B) = (DWORD)(D)
	#define LITTLE_B2D(B, D)	D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
#elif defined(LITTLE_ENDIAN)	////	Little-Endian machine
	#define BIG_B2D(B, D)		D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define BIG_D2B(D, B)		*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
	#define LITTLE_B2D(B, D)	D = *(DWORD *)(B)
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = (DWORD)(D)
#else
	#error ERROR : Invalid DataChangeType
#endif

/*************** Definitions / Macros *************************************/
////	현재 아래의 4개 방식을 지원한다.
#define AI_ECB					1
#define AI_CBC					2
#define AI_OFB					3
#define AI_CFB					4
////	현재 아래의 두 padding을 지원한다.
#define AI_NO_PADDING			1	//	Padding 없음(입력이 16바이트의 배수)
#define AI_PKCS_PADDING			2	//	padding되는 바이트 수로 padding

////	DES에 관련된 상수들
#define DES_BLOCK_LEN			8		//	in BYTEs
#define DES_USER_KEY_LEN		8		//	in BYTEs
#define DES_NO_ROUNDS			16
#define DES_NO_ROUNDKEY			(2*DES_NO_ROUNDS)	//	in DWORDs

/*************** New Data Types *******************************************/
////////	Determine data types depand on the processor and compiler.
//#define BOOL	int					//	1-bit data type
//#define BYTE	unsigned char		//	unsigned 1-byte data type
//#define WORD	unsigned short int	//	unsigned 2-bytes data type
//#define DWORD	unsigned int		//	unsigned 4-bytes data type
#define RET_VAL		DWORD			//	return values

////	DES..
typedef struct{
	DWORD		ModeID;						//	ECB or CBC
	DWORD		PadType;					//	블록암호의 Padding type
	BYTE		IV[DES_BLOCK_LEN];			//	Initial Vector
	BYTE		ChainVar[DES_BLOCK_LEN];	//	Chaining Variable
	BYTE		Buffer[DES_BLOCK_LEN];		//	Buffer for unfilled block
	DWORD		BufLen; 					//	Buffer의 유효 바이트 수
	DWORD		RoundKey[DES_NO_ROUNDKEY];	//	라운드 키의 DWORD 수
} DES_ALG_INFO;

/*************** Constant (Error Code) ************************************/
////	Error Code - 정리하고, 적당히 출력해야 함.
#define CTR_SUCCESS					0
#define CTR_FATAL_ERROR				0x1001
#define CTR_INVALID_USERKEYLEN		0x1002	//	비밀키의 길이가 부적절함.
#define CTR_PAD_CHECK_ERROR			0x1003	//	
#define CTR_DATA_LEN_ERROR			0x1004	//	평문의 길이가 부적절함.
#define CTR_CIPHER_LEN_ERROR		0x1005	//	암호문이 블록의 배수가 아님.
#define CTR_WEAK_KEY				0x1006

/*************** Prototypes ***********************************************/
////	데이타 타입 DES_ALG_INFO에 mode, padding 종류 및 IV 값을 초기화한다.
void	DES_SetAlgInfo(
		DWORD			ModeID,
		DWORD			PadType,
		BYTE			*IV,
		DES_ALG_INFO	*AlgInfo);

////	입력된 DES_USER_KEY_LEN바인트의 비밀키로 라운드 키 생성
RET_VAL DES_KeySchedule(
		BYTE			*UserKey,		//	사용자 비밀키를 입력함.
		DWORD			UserKeyLen,
		DES_ALG_INFO	*AlgInfo);		//	암복호용 Round Key가 저장됨.

////	Init/Update/Final 형식을 암호화.
RET_VAL	DES_EncInit(
		DES_ALG_INFO	*AlgInfo);
RET_VAL	DES_EncUpdate(
		DES_ALG_INFO	*AlgInfo,
		BYTE			*PlainTxt,		//	평문이 입력됨.
		DWORD			PlainTxtLen,
		BYTE			*CipherTxt, 	//	암호문이 출력됨.
		DWORD			*CipherTxtLen);
RET_VAL	DES_EncFinal(
		DES_ALG_INFO	*AlgInfo,
		BYTE			*CipherTxt, 	//	암호문이 출력됨.
		DWORD			*CipherTxtLen);

////	Init/Update/Final 형식을 복호화.
RET_VAL	DES_DecInit(
		DES_ALG_INFO	*AlgInfo);
RET_VAL	DES_DecUpdate(
		DES_ALG_INFO	*AlgInfo,
		BYTE			*CipherTxt,		//	암호문이 입력됨.
		DWORD			CipherTxtLen,
		BYTE			*PlainTxt,		//	복호문이 출력됨.
		DWORD			*PlainTxtLen);
RET_VAL	DES_DecFinal(
		DES_ALG_INFO	*AlgInfo,
		BYTE			*PlainTxt,		//	복호문이 출력됨.
		DWORD			*PlainTxtLen);

/*************** END OF FILE **********************************************/
#endif	//	_DES_H
