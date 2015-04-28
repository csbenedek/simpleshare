//
//  NSString+URLEncoding.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 24/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>
@interface NSString (URLEncoding)
-(NSString *)urlEncodeUsingEncoding:(NSStringEncoding)encoding;
@end