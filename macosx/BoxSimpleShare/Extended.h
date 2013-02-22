//
//  Extended.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSDictionary (EXTENDED)

- (BOOL) containsKey:(NSString *)key;

@end

@interface NSMutableDictionary (EXTENDED)

- (void)safeSetObject:(id)object forKey:(id)key;

@end


@interface NSColor (CGColorAdditions)

@property (readonly) CGColorRef CGColor;
+ (NSColor*)colorWithCGColor:(CGColorRef)aColor;

+ (NSColor*)randomColor;

@end


@interface NSString (EXTENDED)

- (NSArray*)keyCodes;

@end

@interface NSAttributedString (EXTENDED)

+ (id)hyperlinkFromString:(NSString*)inString withURL:(NSURL*)aURL font:(NSFont*)font;

@end
