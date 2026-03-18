//
//  sGLText.m
//  SongGL
//
//  Created by 호학 송 on 11. 8. 17..
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#import <Foundation/Foundation.h>
#if defined (IPHONE)
#import <UIKit/UIKit.h>
#import <CoreText/CoreText.h>
#endif
#include "sGLText.h"
#include <stdlib.h>
#include "sGL.h"



extern CGContextRef HCreateBitmapContext (int pixelsWide,int pixelsHigh);

const char* GetText(const char* sKey)
{
    return SGLTextUnit::GetText(sKey);
}

CGContextRef HCreateBitmapContext (int pixelsWide,int pixelsHigh)
{
    
    CGContextRef    context = NULL;
    CGColorSpaceRef colorSpace;
    void *          bitmapData;
    int             bitmapByteCount;
    int             bitmapBytesPerRow;
	
    //pixelsWide+=4;
    bitmapBytesPerRow   = (pixelsWide * 4);
    bitmapByteCount     = (bitmapBytesPerRow * pixelsHigh);
	
    colorSpace = CGColorSpaceCreateDeviceRGB();
    bitmapData = malloc( bitmapByteCount );
    if (bitmapData == NULL)
    {
        fprintf (stderr, "Memory not allocated!");
		CGColorSpaceRelease( colorSpace );
        return NULL;
    }
    
    //memory reset 추가해야주야 깨끗한 이미지가 생성된다.
    memset(bitmapData, 0, bitmapByteCount);
    
    context = CGBitmapContextCreate (bitmapData,
									 pixelsWide,
									 pixelsHigh,
									 8,
									 bitmapBytesPerRow,
									 colorSpace,
									 kCGImageAlphaPremultipliedLast);
    if (context== NULL)
    {
        free (bitmapData);
		CGColorSpaceRelease( colorSpace );
        fprintf (stderr, "Context not created!");
        return NULL;
    }
    CGColorSpaceRelease( colorSpace );
    return context;
}

unsigned char* MakeStringToImageBuffer(char* sData,int nImgWidth,int nImgHeight,float fFntSize, float fOffSetX,float fOffSetY,bool bBold)
{
    unsigned char *pResult = NULL;
    
    CGContextRef imgcontext = HCreateBitmapContext(nImgWidth, nImgHeight);

#if 1  //아이폰과 맥둘다 된다.

    float fDummyY = fFntSize / 4; //글자의 여백이 존재한다. 폰트크기의 / 4 이면 상단으로 정확하게 맞춰진다.
//한글이 된다. 음홧홧
    
//    static int nNeedCustomBold = -1;
//    if(nNeedCustomBold == -1)
//    {
//        NSString * language = [[NSLocale preferredLanguages] objectAtIndex:0];
//        //영어 이외는 볼드체가 지원이 안뒤아.
//        if([language compare:@"EN" options:NSCaseInsensitiveSearch] == NSOrderedSame)
//            nNeedCustomBold = 0;
//        else
//            nNeedCustomBold = 1; //영어권일때는 자체 볼드체가 지원된다.
//    }
    
    if(imgcontext == nil)
    {
        HLogE("MakeStringToImageBuffer (%f,%f)%s\n",nImgWidth,nImgHeight,sData);
        return NULL;
    }

    
    CGContextSetInterpolationQuality( imgcontext, kCGInterpolationHigh );
    CGContextSetAllowsAntialiasing(imgcontext, YES);
    CGContextSetShouldAntialias(imgcontext, YES);
    CGContextSetShouldSmoothFonts(imgcontext, YES);
    NSString *sT = [NSString stringWithCString:sData encoding:NSUTF8StringEncoding];
    NSMutableAttributedString *stringAtt = [[NSMutableAttributedString alloc] initWithString:sT];

    //"Helvetica-Bold"
    CTFontRef helvetica = NULL;

    if(!bBold)
        helvetica = CTFontCreateWithName(CFSTR("Helvetica"), fFntSize, NULL);
    else
        helvetica = CTFontCreateWithName(CFSTR("Helvetica-Bold"), fFntSize, NULL);
    
    
    [stringAtt addAttribute:(id)kCTFontAttributeName value:( id)helvetica range:NSMakeRange(0, [stringAtt length])];
    
    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    CGFloat color[] = { 1.0, 1.0, 1.0, 1.0 };
    CGColorRef cgColor = CGColorCreate(rgbColorSpace, color);
    CGColorSpaceRelease(rgbColorSpace);
    
    [stringAtt addAttribute:(id)kCTForegroundColorAttributeName value:(id)cgColor range:NSMakeRange(0, [stringAtt length])];
    
    CTFramesetterRef textFramesetter = CTFramesetterCreateWithAttributedString(( CFAttributedStringRef)stringAtt);
    CGMutablePathRef textPath = CGPathCreateMutable();
    
    //CGPathAddRect(textPath, NULL, CGRectMake(fOffSetX, fDummyY - fOffSetY, nImgWidth, nImgHeight));
    //우연히 맞추었다.
    CGPathAddRect(textPath, NULL, CGRectMake(fOffSetX, - fOffSetY, nImgWidth, nImgHeight + fDummyY));
    CTFrameRef textFrame = CTFramesetterCreateFrame(textFramesetter, CFRangeMake(0,0), textPath, NULL);
    CTFrameDraw(textFrame, imgcontext);
    
    
//    if(bBold && nNeedCustomBold == 1)
    if(bBold)
    {
        CGMutablePathRef textPath3 = CGPathCreateMutable();
        CGPathAddRect(textPath3, NULL, CGRectMake(fOffSetX, - fOffSetY, nImgWidth + 0.1f, nImgHeight + fDummyY + 0.1f));
        CTFrameRef textFrame3 = CTFramesetterCreateFrame(textFramesetter, CFRangeMake(0,0), textPath, NULL);
        CTFrameDraw(textFrame3, imgcontext);
        CFRelease(textFrame3);
        CGPathRelease(textPath3);
        
        CGMutablePathRef textPath4 = CGPathCreateMutable();
        CGPathAddRect(textPath4, NULL, CGRectMake(fOffSetX, - fOffSetY, nImgWidth - 0.1f, nImgHeight + fDummyY - 0.1f));
        CTFrameRef textFrame4 = CTFramesetterCreateFrame(textFramesetter, CFRangeMake(0,0), textPath, NULL);
        CTFrameDraw(textFrame4, imgcontext);
        CFRelease(textFrame4);
        CGPathRelease(textPath4);
    }
    
    //메모리를 해제해준다.
    pResult = (unsigned char*)CGBitmapContextGetData(imgcontext);
    
    [stringAtt release];
    CGColorRelease(cgColor);
    CFRelease(textFrame);
    CGPathRelease(textPath);
    CFRelease(textFramesetter);
    CFRelease(helvetica);
    CGContextRelease(imgcontext);
    
    


#else
   
    static int nNeedBold = -1;
    if(nNeedBold == -1)
    {
        NSLocale* currentLocale = [NSLocale currentLocale];  // get the current locale.
        NSString* countryCode = [currentLocale objectForKey:NSLocaleCountryCode];
        
        //한글은 볼드체가 지원되지 않음... 그래서 아래에서 그릴때 트릭을 쓴다.
        if([countryCode compare:@"KR"] == NSOrderedSame)
            nNeedBold = 1;
        else
            nNeedBold = 0; //영어권일때는 자체 볼드체가 지원된다.
        
    }
    
    if(imgcontext == nil)
    {
        HLogE("MakeStringToImageBuffer (%f,%f)%s\n",nImgWidth,nImgHeight,sData);
        return NULL;
    }
    
    //현재 컨텍스트를 가져온다.
    CGSize szSize = CGSizeMake(nImgWidth, nImgHeight);
    if (UIGraphicsBeginImageContextWithOptions != NULL)
        UIGraphicsBeginImageContextWithOptions(szSize,NO,0.0);
    else
        // iOS is < 4.0
        UIGraphicsBeginImageContext(szSize);
    
    //메모리 컨텍스트를 푸쉬한다.
    UIGraphicsPushContext((CGContextRef)imgcontext);
    CGContextTranslateCTM(imgcontext, 0.0, szSize.height);
	CGContextScaleCTM(imgcontext, 1.0, -1.0);
    CGContextSetInterpolationQuality( imgcontext, kCGInterpolationHigh );
    CGContextSetAllowsAntialiasing(imgcontext, YES);
    CGContextSetShouldAntialias(imgcontext, YES);
    CGContextSetShouldSmoothFonts(imgcontext, YES);

    float fDummyY = fFntSize / 4; //글자의 여백이 존재한다. 폰트크기의 / 4 이면 상단으로 정확하게 맞춰진다.
    CGContextSetRGBFillColor(imgcontext, 1.0, 1.0, 1.0, 1.0);
	CGContextSetRGBStrokeColor(imgcontext, 0.0, 0.0, 0.0, 1.0);
    //NSString *sT = [NSString stringWithCString:sData encoding:NSUTF8StringEncoding];
    NSString *sT = [[NSString alloc] initWithCString:sData encoding:NSUTF8StringEncoding];
    CGPoint cgpt = CGPointMake(fOffSetX, fOffSetY - fDummyY);

    if(!bBold || nNeedBold == 1) //한글인경우는 무조건 systemFontofSize로 한다. 볼드 폰트가 없다.
        [sT drawAtPoint:cgpt withFont:[UIFont systemFontOfSize:fFntSize]];
    else
        [sT drawAtPoint:cgpt withFont:[UIFont boldSystemFontOfSize:fFntSize]];
    
    if(bBold && nNeedBold == 1)
    {
        [sT drawAtPoint:cgpt withFont:[UIFont systemFontOfSize:fFntSize + 0.3]];
        [sT drawAtPoint:cgpt withFont:[UIFont systemFontOfSize:fFntSize - 0.3]];
    }
    
    pResult = (unsigned char*)CGBitmapContextGetData(imgcontext);
    UIGraphicsPopContext();
    
    UIGraphicsEndImageContext();
    CGContextRelease(imgcontext);
    [sT release];
    
//영문만 되는 소스이다.
//    float fDummyY = fFntSize / 4; //글자의 여백이 존재한다. 폰트크기의 / 4 이면 상단으로 정확하게 맞춰진다.
//    //한글이 안된다...
//    int szNlen = strlen(sData);
//    CGContextSelectFont(imgcontext, "Helvetica", fFntSize, kCGEncodingMacRoman);
//    CGContextSetRGBFillColor(imgcontext, 1.0, 1.0, 1.0, 1.0);
//	CGContextSetRGBStrokeColor(imgcontext, 0.0, 0.0, 0.0, 1.0);
//    
//    //kCGTextStroke,kCGTextFillStroke,kCGTextFill
//    CGContextSetTextDrawingMode(imgcontext, kCGTextFill);
//    
//    //이미지를 그릴때 상하가 꺼꾸로 그려진다. (원래 이미지는 제대로 그려진다.)
//    //CGContextSetTextMatrix(imgcontext, CGAffineTransformMakeScale(1.0, -1.0));
//    
//    CGContextSetInterpolationQuality( imgcontext, kCGInterpolationHigh );
//    CGContextSetAllowsAntialiasing(imgcontext, YES);
//    CGContextSetShouldAntialias(imgcontext, YES);
//    CGContextSetShouldSmoothFonts(imgcontext, YES);
//
//    //230
//	CGContextShowTextAtPoint(imgcontext, fOffSetX, nImgHeight - (fFntSize - fDummyY + fOffSetY), sData, szNlen);
//    
//    //메모리를 해제해준다.
//    pResult = (unsigned char*)CGBitmapContextGetData(imgcontext);
//    
//    // IPhone에서 파일을 저장하려면 여기를
//    //    CGImageRef imgref = CGBitmapContextCreateImage(imgcontext);
//    //    UIImage* img = [UIImage imageWithCGImage:imgref];
//    //    NSString  *pngPath = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents/Test.png"];
//    //    [UIImagePNGRepresentation(img) writeToFile:pngPath atomically:YES];
//    //
//    
//	CGContextRelease(imgcontext);
#endif
    
    
    return pResult;
}

#if defined (IPHONE)

UIImage* imageWithString(NSString* string, // What we want an image of.
                         UIFont * font,     // The font we'd like it in.
                         CGSize size)       // Size of the desired image.
{
    // Create a context to render into.
    //    UIGraphicsBeginImageContext(size);
    if (UIGraphicsBeginImageContextWithOptions != NULL)
        UIGraphicsBeginImageContextWithOptions(size,NO,0.0);
    else
        // iOS is < 4.0 
        UIGraphicsBeginImageContext(size);
    
    
    // Work out what size of font will give us a rendering of the string
    // that will fit in an image of the desired size. 
    
    // We do this by measuring the string at the given font size and working 
    // out the ratio scale to it by to get the desired size of image.
    
    // Measure the string size.
    CGSize stringSize;
    
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 7.f)
    {
        // code here for iOS 5.0,6.0 and so on
        stringSize = [string sizeWithFont:font];
    }
    else
    {
        // code here for iOS 7.0
        stringSize = [string sizeWithAttributes: @{NSFontAttributeName: font}];
    }
    
    // Work out what it should be scaled by to get the desired size.
    CGFloat xRatio = size.width / stringSize.width;
    CGFloat yRatio = size.height / stringSize.height;
    CGFloat ratio = MIN(xRatio, yRatio);
    
    // Work out the point size that'll give us the desired image size, and
    // create a UIFont that size.
    CGFloat oldFontSize = font.pointSize;
    CGFloat newFontSize = floor(oldFontSize * ratio); 
//    ratio = newFontSize / oldFontSize;
    font = [font fontWithSize:newFontSize];
    
    // What size is the string with this new font?
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 7.f)
        stringSize = [string sizeWithFont:font];
    else
        stringSize = [string sizeWithAttributes: @{NSFontAttributeName: font}];
    
    // Work out where the origin of the drawn string should be to get it in 
    // the centre of the image.
    CGPoint textOrigin = CGPointMake((size.width - stringSize.width) / 2, 
                                     (size.height - stringSize.height) / 2);
    
    // Draw the string into out image.
    [string drawAtPoint:textOrigin withFont:font];
    
    // We actually don't have the scaling right, because the rendered
    // string probably doesn't actually fill the entire pixel area of the 
    // box we were given.  We'll use what we just drew to work out the /real/ 
    // size we need to draw at to fill the image.
    
    // First, we work out what area the drawn string /actually/ covered.
    
    // Get a raw bitmap of what we've drawn.
    CGImageRef maskImage = [UIGraphicsGetImageFromCurrentImageContext() 
                            CGImage];
    CFDataRef imageData = CGDataProviderCopyData(
                                                 CGImageGetDataProvider(maskImage));
    uint32_t *bitmap = (uint32_t *)CFDataGetBytePtr(imageData);
    size_t rowBytes = CGImageGetBytesPerRow(maskImage);
    
    // Now, go through the pixels one-by-one working out the area in which the
    // image is not still blank.
    size_t minx = size.width, maxx = 0, miny = size.height, maxy = 0;
    uint32_t *rowBase = bitmap;
    for(size_t y = 0; y < size.width; ++y, rowBase += rowBytes) {
        uint32_t *component = rowBase;
        for(size_t x = 0; x < size.width; ++x, component += 4) {    
            if(*component != 0) {
                if(x < minx) {
                    minx = x;
                } else if(x > maxx) {
                    maxx = x;
                }
                if(y < miny) {
                    miny = y;
                } else if(y > maxy) {
                    maxy = y;
                }
            }
        }
    }
    CFRelease(imageData); // We're done with this data now.
    
    // Put the area we just found into a CGRect.
    CGRect boundingBox =
    CGRectMake(minx, miny, maxx - minx + 1, maxy - miny + 1);
    
    // We're going to have to move string we're drawing as well as scale it,
    // so we work out how the origin we used to draw the string relates to the 
    // 'real' origin of the filled area.
    CGPoint goodBoundingBoxOrigin = 
    CGPointMake((size.width - boundingBox.size.width) / 2,
                (size.height - boundingBox.size.height) / 2);
    CGFloat textOriginXDiff = goodBoundingBoxOrigin.x - boundingBox.origin.x;
    CGFloat textOriginYDiff = goodBoundingBoxOrigin.y - boundingBox.origin.y;
    
    // Work out how much we'll need to scale by to fill the entire image.
    xRatio = size.width / boundingBox.size.width;
    yRatio = size.height / boundingBox.size.height;
    ratio = MIN(xRatio, yRatio);
    
    // Now, work out the font size we really need based on our scaling ratio.
    // newFontSize is still holding the size we used to draw with.
    oldFontSize = newFontSize;
    newFontSize = floor(oldFontSize * ratio); 
    ratio = newFontSize / oldFontSize;
    font = [font fontWithSize:newFontSize];
    
    // Work out where to place the string.
    // We offset the origin by the difference between the string-drawing origin 
    // and the 'real' image origin we measured above, scaled up to the new size.
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 7.f)
        stringSize = [string sizeWithFont:font];
    else
        stringSize = [string sizeWithAttributes: @{NSFontAttributeName: font}];

    
    textOrigin = CGPointMake((size.width - stringSize.width) / 2, 
                             (size.height - stringSize.height) / 2);    
    textOrigin.x += textOriginXDiff * ratio;
    textOrigin.y += textOriginYDiff * ratio;
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    // Clear the context to remove our old, too-small, rendering.
    CGContextClearRect(context, 
                       CGRectMake(0, 0, size.width, size.height));
    
    //CGContextSetRGBFillColor(context, 1.0, 1.0, 1.0, 1.0);
    //CGContextSetRGBStrokeColor(context, 1.0, 0.0, 0.0, 1.0);
    
    // Draw the string again, in the right place, at the right size this time!
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 7.f)
        [string drawAtPoint:textOrigin withFont:font];
    else
        stringSize = [string sizeWithAttributes: @{NSFontAttributeName: font}];    
    
    // We're done!  Grab the image and return it! 
    // (Don't forget to end the image context first though!)
    UIImage *retImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return retImage;
}

#if defined IPHONE
//--------------------------------------------------------------------------------------
// 로그를 쌓을때 사용한다.
// 메세지의 크기는 BUFSIZ=1024이다.
//--------------------------------------------------------------------------------------
void sglLog ( const char *formatStr, ... )
{
    va_list params;
    char *buf = NULL;//[BUFSIZ];
    va_start ( params, formatStr );
// 이방식또한 추천한다.
//    int nLen = vsnprintf(NULL,0,formatStr,params);
//    buf = (char*)malloc(nLen*sizeof(char));
//    vsprintf ( buf, formatStr, params );
    //간단하게 이렇게
    vasprintf(&buf,formatStr,params);
	fprintf(stderr,"%s", buf);
    if(buf) free(buf);
    va_end ( params );
}

void sglLogNet ( const char *formatStr, ... )
{
    va_list params;
    char *buf = NULL;//[BUFSIZ];
    va_start ( params, formatStr );
    vasprintf(&buf,formatStr,params);
//	fprintf(stderr,"%s", buf);
    NSString* sx = [[NSString alloc] initWithCString:buf encoding:NSUTF8StringEncoding];
    NSLog(@"[NET]%@",sx);
    [sx release];
    if(buf) free(buf);
    va_end ( params );
}


void sglLogE ( const char *formatStr, ... )
{
    va_list params;
    char *buf = NULL;//[BUFSIZ];
    va_start ( params, formatStr );
    vasprintf(&buf,formatStr,params);
//	fprintf(stderr,"[ERROR]%s", buf);
    NSString* sx = [[NSString alloc] initWithCString:buf encoding:NSUTF8StringEncoding];
    NSLog(@"[ERROR]%@",sx);
    [sx release];
    if(buf) free(buf);
    va_end ( params );
}
#endif

#endif