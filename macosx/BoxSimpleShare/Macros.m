//
//  Macros.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <CommonCrypto/CommonDigest.h>
#import "JSON.h"
#import "Macros.h"

@implementation Macros

void (^SSLog)(id, ...) = ^void (id args, ...)
{
    #if SHOW_LOGS
        va_list list;
        va_start(list, args);
        NSString *str = [[NSString alloc] initWithFormat:args arguments:list];
        NSLog(@"[SSLog] >> %@", str);
        [str release];
        va_end(list);
    #endif
};

NSString * (^MD5)(NSString *) = ^NSString *(NSString *string)
{
    const char *cStr = [string UTF8String];
	unsigned char result[CC_MD5_DIGEST_LENGTH];
	CC_MD5( cStr, (uint)strlen(cStr), result );
	return [NSString 
			stringWithFormat: @"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			result[0], result[1],
			result[2], result[3],
			result[4], result[5],
			result[6], result[7],
			result[8], result[9],
			result[10], result[11],
			result[12], result[13],
			result[14], result[15]
			];
};

NSString * (^URLEncode)(NSString *) = ^NSString * (NSString *string)
{ 
    CFStringRef urlString = CFURLCreateStringByAddingPercentEscapes(NULL, (CFStringRef)string,  NULL, (CFStringRef)@"!*'\"();:@&=+$,/?%#[]% ", kCFStringEncodingUTF8 );
    return [(NSString *)urlString autorelease];
};	

NSString * (^returnValueWithAccurateAnnotation)(long long) = ^NSString *(long long value)
{
    NSString *identifier = @"";
    double newValue = 0;
//    if (value > (1 * 1024 * 1024 * 1024 * 1024))
//    {
//        newValue = ((float)value / (1 * 1024 * 1024 * 1024 * 1024));
//        identifier = @"TB";
//    }
//    else
        if (value > (1 * 1024 * 1024 * 1024))
        {
            newValue = ((float)value / (1 * 1024 * 1024 * 1024));
            identifier = @"GB";
        }
        else
            if (value > (1 * 1024 * 1024))
            {
                newValue = ((float)value / (1 * 1024 * 1024));
                identifier = @"MB";
            }
            else
                if (value > (1 * 1024))
                {
                    newValue = ((float)value / (1 * 1024));
                    identifier = @"KB";
                }
                else
                {
                    identifier = @"Bytes";
                }
    
    return [NSString stringWithFormat:@"%.2f %@", newValue, identifier];
};

static NSString *apiKey = @"AIzaSyCuDU5hHOuVyD_1bKg5QRu0JydNhNSwCOE";

NSString * (^URLShortner)(NSString *) = ^NSString *(NSString * longURL)
{
    NSString *url = [NSString stringWithFormat:@"https://www.googleapis.com/urlshortener/v1/url?key=%@", apiKey]; 
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    
    NSMutableDictionary *dic = [[NSMutableDictionary new] autorelease];
    [dic setValue:longURL forKey:@"longUrl"];
    
    [request setHTTPBody:[[dic JSONRepresentation] dataUsingEncoding:NSUTF8StringEncoding]];
    
    [request setHTTPMethod:@"POST"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    
    NSURLResponse *response = nil;
    NSError *error = nil;
    
    NSData *data = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];
    
    if (response && !error && data.length > 0)
    {
        NSString *str = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
        
        NSDictionary *dic = [str JSONValue];
        
        if (dic && [[dic allKeys] containsObject:@"id"] && [[dic allKeys] containsObject:@"longUrl"])
        {
            if ([[dic valueForKey:@"longUrl"] isEqual:longURL] && [[dic valueForKey:@"kind"] isEqual:@"urlshortener#url"])
            {
                return [dic valueForKey:@"id"];
                data = nil;
            }
        }
    }
    data = nil;
    return nil;
};

BOOL (^CopyURLToClipBoardForType)(NSString *,  NSString *type, ...) = ^BOOL(NSString *url, NSString *type, ...) NS_REQUIRES_NIL_TERMINATION
{
    NSPasteboard* pBoard= [NSPasteboard generalPasteboard];
    va_list list;
    va_start(list, type);
    id obj = va_arg(list, NSString*);
    
    NSMutableArray *array = [NSMutableArray new];
    
    while(obj != nil) 
    {
        [array addObject:obj];
        obj = va_arg(list, NSString*);
    }
    
    va_end(list);
    
    [pBoard declareTypes:array owner:nil];
    [pBoard setString:url forType:NSStringPboardType];
    
    [array release];
    
    return YES;
};

@end