//
//  HTTPDelegate.h
//  SongGL
//
//  Created by 호학 송 on 13. 10. 11..
//
//

#import <Foundation/Foundation.h>

@interface HTTPDelegate : NSObject
{
    NSMutableData   *mOrgRcvData;
    NSMutableData   *mRcvData;
    int             m_nUnCompressLen;
    BOOL            mDone;
}

@property (nonatomic, retain) NSMutableData *mOrgRcvData;
@property (nonatomic, retain) NSMutableData *mRcvData;
@property (assign) int m_nUnCompressLen;
@property (assign) BOOL mDone;

+ (BOOL) buildHeader:(NSMutableURLRequest *)request dataLength:(int)nLength orgLength:(int)nOrgLength;
@end
