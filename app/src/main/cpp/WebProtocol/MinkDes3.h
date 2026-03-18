//
//  MinkDes3.h
//  MinkOSXCore
//
//  Created by 호학 송 on 10. 4. 27..
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface MinkDes3 : NSObject {

}

+ (int) MinkDesSFile:(NSString*) sSFilePath DFile:(NSString*) sDFilePath Type:(int) nEncType;
+ (int) MinkDesBuffer:(NSData*) Buffer DFile:(NSString *)sDFilePath Type:(int) nEncType;
/*Add Kang 
 (파일을 읽어서 버퍼로(nEncType = 0은 암호화하여 아니면 복호화하여서)
 */
+ (int) MinkDesFromFile:(NSString*)szFilePath ToBuffer:(NSData**)buffer Type:(int)nEncType;

+ (NSData*) MinkDesBuffer:(NSData*) Buffer Type:(int) nEncType;
+ (NSData*) MinkDesByte:(const char*) pBuffer Size:(int)nBufferSize Type:(int) nEncType;

@end
