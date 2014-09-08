//
//  ImageUtil.h
//  Selfie
//
//  Created by Cuongnq on 11/1/13.
//
//

#import <Foundation/Foundation.h>

@interface ImageUtil : NSObject

+ (NSImage*)thunbnailWaterMarkImage;
+ (NSImage*)thunbnailWaterMarkImage :(NSImage*)image size:(NSSize)newSize;
+ (NSImage*)roundCorners:(NSImage *)image withSize:(NSSize)newSize;
+ (NSImage*) compositeImage:(NSImage*)overlay mainImage:(NSImage*)mainImage withSize:(NSSize) newSize;
+(NSImage*)drawingPopupWithWaterMark:(NSImage*) image withSize:(NSSize)size;
+ (NSSize)scaleImage:(NSImage*) image  withSize:(NSSize)newSize;
+ (NSImage*)resizeImage :(NSImage*)image size:(NSSize)newSize ;
@end
