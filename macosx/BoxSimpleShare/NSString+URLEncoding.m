//
//  NSString+URLEncoding.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 24/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "NSString+URLEncoding.h"
@implementation NSString (URLEncoding)
-(NSString *)urlEncodeUsingEncoding:(NSStringEncoding)encoding {
	return (NSString *)CFURLCreateStringByAddingPercentEscapes(NULL,
                                                               (CFStringRef)self,
                                                               NULL,
                                                               (CFStringRef)@"!*'\"();:@&=+$,/?%#[]% ",
                                                               CFStringConvertNSStringEncodingToEncoding(encoding));
}
@end
