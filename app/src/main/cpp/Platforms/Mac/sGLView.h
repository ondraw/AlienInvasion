//
//  untitled.h
//  CommonMac
//
//  Created by 호학 송 on 10. 9. 25..
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "sGL.h"

//OnPrepareOpenGL 초기화가 성공시 실행되는 이벤트
extern int OnPrepareOpenGL(void* pThis,SGLContext* pSContext);

//OnChangedProjection 투영설정이 변경될때 실행되는 이벤트
extern int OnChangedProjection(void* pThis, SGLContext* pSContext);

//OnBeginRender 렌더링을 초기화할때 발생하는 이벤트
extern int OnBeginRender(void* pThis,SGLContext* pSContext);

//OnRender 렌더링을 할때 발생하는 이벤트
extern int OnRender(void* pThis,SGLContext* pSContext);

//OnEndRender 렌더링이 종료할때 발행하는 이벤트
extern int OnEndRender(void* pThis,SGLContext* pSContext);


@interface sGLView : NSOpenGLView 
{

	SGLContext *mGLContext;
	CFAbsoluteTime mStartTime;
	NSTimer* mTimer;

	
	CFAbsoluteTime mTime;
	CFTimeInterval mDeltaTime;
}

@property (nonatomic,assign) SGLContext *mGLContext;
@property (nonatomic,assign) CFAbsoluteTime mTime;
#ifndef MAC
+ (NSOpenGLPixelFormat*) basicPixelFormat;
#endif
-(void) Clear;
-(void) Start;
-(void) Stop;
@end
