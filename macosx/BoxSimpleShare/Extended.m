//
//  Extended.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "Extended.h"
#import <Carbon/Carbon.h>

CFStringRef createStringForKey(CGKeyCode keyCode);

@implementation NSDictionary (EXTENDED)

- (BOOL) containsKey:(NSString *)key
{
    return [[self allKeys] containsObject:key];
}

@end

@implementation NSMutableDictionary (EXTENDED)

- (void)safeSetObject:(id)object forKey:(id)key {
    if (object && key) {
        [self setObject:object forKey:key];
    }
}

@end


@implementation NSColor (CGColorAdditions)

- (CGColorRef)CGColor
{
    NSColor *colorRGB = [self colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
    CGFloat components[4];
    [colorRGB getRed:&components[0] green:&components[1] blue:&components[2] alpha:&components[3]];
    CGColorSpaceRef theColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    CGColorRef theColor = CGColorCreate(theColorSpace, components);
    CGColorSpaceRelease(theColorSpace);
    return (CGColorRef)[(id)theColor autorelease];
}

+ (NSColor*)colorWithCGColor:(CGColorRef)aColor
{
    const CGFloat *components = CGColorGetComponents(aColor);
    CGFloat red = components[0];
    CGFloat green = components[1];
    CGFloat blue = components[2];
    CGFloat alpha = components[3];
    return [self colorWithDeviceRed:red green:green blue:blue alpha:alpha];
}

+ (NSColor*)randomColor {
    return [NSColor colorWithDeviceRed:(float)(arc4random() % 100) / 100.0
                                 green:(float)(arc4random() % 100) / 100.0
                                  blue:(float)(arc4random() % 100) / 100.0
                                 alpha:0.3];
}

@end


@implementation NSString (EXTENDED)

- (NSArray*)keyCodes {
    static NSMutableDictionary* keyCodeMapping = nil;
    
    if (!keyCodeMapping) {
        keyCodeMapping = [[NSMutableDictionary alloc] init];
        for (int i = 0; i < 128; i++) {
            NSString* string = (NSString*)createStringForKey(i);
            if (string) {
                [keyCodeMapping setObject:string forKey:[NSNumber numberWithInt:i]];
            }
            [string release];
        }
    }
    
    NSMutableArray* result = [NSMutableArray array];
    for (NSNumber* key in [keyCodeMapping allKeys]) {
        NSString* value = [keyCodeMapping objectForKey:key];
        if ([value isEqualToString:self]) {
            [result addObject:key];
        }
    }
    return [NSArray arrayWithArray:result];
}

@end

CFStringRef createStringForKey(CGKeyCode keyCode) {
    TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
    if (currentKeyboard) {
        CFDataRef layoutData =
        TISGetInputSourceProperty(currentKeyboard,
                                  kTISPropertyUnicodeKeyLayoutData);
        
        if (layoutData) {
            const UCKeyboardLayout *keyboardLayout =
            (const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);
            
            UInt32 keysDown = 0;
            UniChar chars[4];
            UniCharCount realLength;
            
            UCKeyTranslate(keyboardLayout,
                           keyCode,
                           kUCKeyActionDisplay,
                           0,
                           LMGetKbdType(),
                           kUCKeyTranslateNoDeadKeysBit,
                           &keysDown,
                           sizeof(chars) / sizeof(chars[0]),
                           &realLength,
                           chars);
            CFRelease(currentKeyboard);    
            
            return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
        }
    }
    return NULL;
}


@implementation NSAttributedString (EXTENDED)

+ (id)hyperlinkFromString:(NSString*)inString withURL:(NSURL*)aURL font:(NSFont*)font {
    NSMutableAttributedString* attrString = [[NSMutableAttributedString alloc] initWithString:inString];
    NSRange range = NSMakeRange(0, [attrString length]);
 	
    [attrString beginEditing];
    
    [attrString addAttribute:NSFontAttributeName value:font range:range];
    
    [attrString addAttribute:NSLinkAttributeName value:[aURL absoluteString] range:range];
 	
    // make the text appear in blue
    [attrString addAttribute:NSForegroundColorAttributeName value:[NSColor blueColor] range:range];
 	
    // next make the text appear with an underline
    [attrString addAttribute:
 	 NSUnderlineStyleAttributeName value:[NSNumber numberWithInt:NSSingleUnderlineStyle] range:range];
    [attrString addAttribute:NSCursorAttributeName value:[NSCursor pointingHandCursor] range:range];
    [attrString endEditing];
 	
    return [attrString autorelease];
}

@end
