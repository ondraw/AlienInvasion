//
//  MinkDes3.m
//  MinkOSXCore
//
//  Created by 호학 송 on 10. 4. 27..
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MinkDes3.h"
#import "des3.h"

@implementation MinkDes3

bool saveFile(const char *filename, const char *buffer, size_t len, bool binary);

bool saveFile(const char *filename, const char *buffer, size_t len, bool binary)
{
	const char *mode = binary ? "wb" : "w" ;
	
    FILE *f = fopen(filename, mode);
	
    if(!f)
        return false;
	
    if (fwrite(buffer, sizeof(char), len, f) != len) {
        fclose(f);
        return false;
    }
    fclose(f);
	
    return true;
}

+ (int) MinkDesSFile:(NSString*) sSFilePath DFile:(NSString*) sDFilePath Type:(int) nEncType;
{
	return MinkEnDes3File((char*)[sSFilePath UTF8String],(char*)[sDFilePath UTF8String],nEncType);
}

//버퍼 데이터를 -> 파일로 
+ (int) MinkDesBuffer:(NSData*) Buffer DFile:(NSString *)sDFilePath Type:(int) nEncType
{
	int nResult = 0;
	BYTE* pOutBuffer = NULL;
	BYTE* pBuffer = (BYTE*)[Buffer bytes];
	int nSize = [Buffer length];
	nResult = MinkDes(pBuffer,nSize,&pOutBuffer,nEncType);
	if(nResult <= 0) return nResult;
	if(saveFile([sDFilePath UTF8String],(const char*)pOutBuffer,nResult,true) == false)
	{
		free(pOutBuffer);
		return -1;
	}
	free(pOutBuffer);
	return nResult;
}

//파일에서 읽어서 -> 버퍼로 
+ (int) MinkDesFromFile:(NSString*)szFilePath ToBuffer:(NSData**)buffer Type:(int)nEncType
{
	NSData *srcData = [NSData dataWithContentsOfFile:szFilePath];
	if(srcData==nil)
		return -1;
	
	int nResult = 0;
	BYTE* pOutBuffer = NULL;
	BYTE* pBuffer = (BYTE*)[srcData bytes];
	int nSize = [srcData length];
	nResult = MinkDes(pBuffer,nSize,&pOutBuffer,nEncType);
	if(nResult <= 0) return nResult;
	
	
	*buffer = [NSData dataWithBytes:pOutBuffer length:nResult];
	free(pOutBuffer);
	return nResult;
}



+ (NSData*) MinkDesBuffer:(NSData*) Buffer Type:(int) nEncType
{
	int nResult = 0;
	BYTE* pOutBuffer = NULL;
	BYTE* pBuffer = (BYTE*)[Buffer bytes];
	int nSize = [Buffer length];
	nResult = MinkDes(pBuffer,nSize,&pOutBuffer,nEncType);
	if(nResult <= 0) return nil;
	NSData *pOutData = [[NSData alloc] initWithBytesNoCopy:pOutBuffer length:nResult];
	//free(pOutBuffer);
	return pOutData;
}


+ (NSData*) MinkDesByte:(const char*) pBuffer Size:(int)nBufferSize Type:(int) nEncType
{
	int nResult = 0;
	BYTE* pOutBuffer = NULL;
	nResult = MinkDes((BYTE*)pBuffer,nBufferSize,&pOutBuffer,nEncType);
	if(nResult <= 0) return nil;
	NSData *pOutData = [[NSData alloc] initWithBytesNoCopy:pOutBuffer length:nResult];
	//free(pOutBuffer);
	return pOutData;
}


@end
