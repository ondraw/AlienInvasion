//
//  HTTPDelegate.m
//  SongGL
//
//  Created by 호학 송 on 13. 10. 11..
//
//
#include "sGL.h"
#import "HTTPDelegate.h"
#include "MinkConnector.h"

@implementation HTTPDelegate
@synthesize m_nUnCompressLen,mDone,mRcvData,mOrgRcvData;
- (void)dealloc
{
    self.mRcvData = nil;
    self.mOrgRcvData = nil;
    [super dealloc];
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)aResponse
{
    
    NSHTTPURLResponse* response = (NSHTTPURLResponse*)aResponse;
    int nCode = [response statusCode];
    NSDictionary *dip = [(NSHTTPURLResponse *) response allHeaderFields];
	NSEnumerator *enumerator;
	id key;
    self.m_nUnCompressLen = 0;
	enumerator = [dip keyEnumerator];
    while ((key = [enumerator nextObject]))
    {
		NSString* szHeaderTitle = (NSString *)key;
		if([szHeaderTitle isEqualToString:@"Content-Encoding"])
		{
            //			if([(NSString*)[dip objectForKey:key] isEqualToString:@"defdes"])
            //				m_bMinkEncryp = TRUE;
		}
		else if([szHeaderTitle isEqualToString:@"Uncompressed-Content-Length"]) {
			self.m_nUnCompressLen = [((NSString*)[dip objectForKey:key]) intValue];
		}
        else if([szHeaderTitle isEqualToString:@"uncompressed-content-length"]) {
			m_nUnCompressLen = [((NSString*)[dip objectForKey:key]) intValue];
		}
		/*http 헤더 로그*/
		NSLog(@"HTTP Header %@ : %@", key, [dip objectForKey:key]);
    }
    
    if(nCode != 200)
    {
        self.mDone = YES;
        HLogE("Error Http Connection Code = %d\n",nCode);
    }
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	NSLog(@"HTTP Error[%ld] %@", (long)[error code], [error localizedDescription]);
    self.mDone = YES;
    
    HLogE("connection didFailWithError Code = %d\n",[error code]);
    //    UIApplication* app = [UIApplication sharedApplication];
    //	app.networkActivityIndicatorVisible = NO;
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
	[self.mOrgRcvData appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    self.mDone = YES;
    char *unCompData = nil;
    if([self.mOrgRcvData length] > 0)
    {
        unCompData = MinkConnector::decryptionMinkProtocol((int)[self.mOrgRcvData length] , self.m_nUnCompressLen, (char*)[self.mOrgRcvData bytes]);
        if(unCompData)
        {
            self.mRcvData = [NSMutableData dataWithBytes:unCompData length:self.m_nUnCompressLen];
            free(unCompData); unCompData = NULL;
        }
        else
        {
            HLogE("Error Can not decryptionMinkProtocol\n");
        }
    }
    else
    {
        HLogE("Rcv Data is zero\n");
    }
}


+ (BOOL) buildHeader:(NSMutableURLRequest *)request dataLength:(int)nLength orgLength:(int)nOrgLength
{
    if(request == nil) return FALSE;
    
    //connection은 아이폰에서는 무조건 keep-alive 로 설정되는 듯...//
    //[request setValue:@"close" forHTTPHeaderField:@"Connection"];
    [request setValue:@"AlienInvasion" forHTTPHeaderField:@"User-Agent"];
//    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    
    //캐쉬 사용안함.매번 새로운 요청..
    [request setValue:@"no-cache" forHTTPHeaderField:@"Cache-Control"];         //http v1.1
    [request setValue:@"no-cache" forHTTPHeaderField:@"Pragma"];				//http v1.0
    
    [request setValue:[NSString stringWithFormat:@"%d",nLength] forHTTPHeaderField:@"Content-Length"];
    
    [request setValue:@"text/plain; charset=UTF-8" forHTTPHeaderField:@"Content-Type"];
    [request setValue:@"defdes" forHTTPHeaderField:@"Accept-Encoding"];
    [request setValue:@"defdes" forHTTPHeaderField:@"Content-Encoding"];
    [request setValue:[NSString stringWithFormat:@"%d", nOrgLength] forHTTPHeaderField:@"Uncompressed-Content-Length"];
    
    return TRUE;
}

@end
