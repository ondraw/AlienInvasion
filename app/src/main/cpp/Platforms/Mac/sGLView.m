//
//  untitled.m
//  CommonMac
//
//  Created by 호학 송 on 10. 9. 25..
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "sGLView.h"


@implementation sGLView
@synthesize mGLContext;
@synthesize mTime;


#pragma mark --
#pragma mark OpenGL 기본함수들 
- (id)initWithFrame:(NSRect)frame {
	//기본적인 픽셀 포멧으로 윈도우를 생성한다.
	NSOpenGLPixelFormat * pf = [sGLView basicPixelFormat];
    self = [super initWithFrame:frame pixelFormat: pf];
    if (self) {
		
		mGLContext = NULL;
		mStartTime = 0.0f;
	}
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
	
	NSRect rectView = [self bounds];
	if(mGLContext == NULL) return;
    
	sglRender(mGLContext,
				  rectView.size.width, 
			      rectView.size.height,
				  OnChangedProjection,
				  OnBeginRender,
				  OnRender,
				  OnEndRender,
				  (void*) self);
}

//OpenGL을 초기화를 한다.
- (void) prepareOpenGL
{
	//OpenGL을 초기화를 한다. 
	//prepareOpenGLFunction 를 실행하여 맥에서 필요한 초기화를 실행한다.
	mGLContext = sglInitialize(OnPrepareOpenGL,(void*)self);
	if(mGLContext != NULL)
		mGLContext->pViewContext = (void*)self;
		
}

// this can be a troublesome call to do anything heavyweight, as it is called on window moves, resizes, and display config changes.  So be
// careful of doing too much here.
- (void) update // window resizes, moves and display changes (resize, depth and display config change)
{
	//msgTime	= getElapsedTime ();
	[super update];
}

- (void) awakeFromNib
{
    //탭선택이벤트에서 실행된다.
	[self Start];
}

- (void) dealloc
{
	[self Clear];
	[super dealloc];
}

-(void) Clear
{
    if(mTimer)
    {
        [mTimer invalidate];
//        [mTimer release];
        mTimer = NULL;
    }
	if(mGLContext != NULL)
	{
		sglRelease(mGLContext);
		mGLContext = NULL;
		
	}
    
}


-(void) Stop
{
    if(mTimer)
    {
        [mTimer invalidate];
//        [mTimer release];
        mTimer = nil;
    }
}

-(void) Start
{
    if(mTimer == nil)
    {
        mTime = CFAbsoluteTimeGetCurrent ();  // set animation time start time
        // start animation timer
        mTimer = [NSTimer timerWithTimeInterval:(1.0f/60.0f)
                                         target:self selector:@selector(OnTimer:)
                                       userInfo:nil
                                        repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:mTimer forMode:NSDefaultRunLoopMode];
        [[NSRunLoop currentRunLoop] addTimer:mTimer forMode:NSEventTrackingRunLoopMode]; // ensure timer fires during resize
    }
}

#pragma mark --
#pragma mark awakeFromNib 에서 불리어진다.
- (void)OnTimer:(NSTimer *)timer
{
	mDeltaTime = CFAbsoluteTimeGetCurrent () - mTime;	
	//if (mDeltaTime > 60.0) // skip pauses
	//	return;
	mTime = CFAbsoluteTimeGetCurrent (); //reset time in all cases
	if(mGLContext == NULL) return;
	
	
	//화면을 그린다.
	[self drawRect:[self bounds]]; // redraw now instead dirty to enable updates during live resize
}


#pragma mark --
#pragma mark initWithFrame 에서 불리어진다.

#ifndef MAC
//기본족인 픽셀 포멧을 만든다.
+ (NSOpenGLPixelFormat*) basicPixelFormat
{
    NSOpenGLPixelFormatAttribute attributes [] = {
        NSOpenGLPFAWindow,
        NSOpenGLPFADoubleBuffer,	// double buffered
        NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16, // 16 bit depth buffer
        
        (NSOpenGLPixelFormatAttribute)nil
    };
    return [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
}
#endif

#pragma mark --
#pragma mark SongGL 이벤트

int OnPrepareOpenGL(void* pThis,SGLContext* pSContext)
{
	sGLView *pView = (sGLView*)pThis;
	//실지적으로 오픈지엘의 초기화를 한다.
	long swapInt = 1;
	[[pView openGLContext] setValues:(const GLint*)&swapInt forParameter:NSOpenGLCPSwapInterval]; 
	return 0;
}

//OnChangedProjection 뷰의 크기가 변하거나 초기화되면 투영의 값들이 변경된다. 그때 날라오는 이벤트
int OnChangedProjection(void* pThis, SGLContext* pSContext)
{
	sGLView *pView = (sGLView*)pThis;
    [[pView openGLContext] makeCurrentContext];
	return E_SUCCESS;
}

int OnBeginRender(void* pThis,SGLContext* pSContext)
{
	return E_SUCCESS;
}

int OnRender(void* pThis,SGLContext* pSContext)
{
	return E_SUCCESS;
}

int OnEndRender(void* pThis,SGLContext* pSContext)
{
	sGLView *pView = (sGLView*)pThis;
	if ([pView inLiveResize])
		glFlush ();
	else
		[[pView openGLContext] flushBuffer];
	return E_SUCCESS;
}




@end
