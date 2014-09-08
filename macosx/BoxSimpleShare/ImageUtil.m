//
//  ImageUtil.m
//  Selfie
//
//  Created by Cuongnq on 11/1/13.
//
//

#import "ImageUtil.h"

@implementation ImageUtil

+ (NSImage*)thunbnailWaterMarkImage {
    static NSImage* _thumbnail = nil;
    
    if (_thumbnail == nil) {
        
        NSSize newSize = NSMakeSize(300, 300);
        NSImage* image = [NSImage imageNamed:@"watermark2.png"];
        _thumbnail =  [[self class] thunbnailWaterMarkImage:image size:newSize];
    }
    
    return _thumbnail;
}
+ (NSImage*)thunbnailWaterMarkImage :(NSImage*)image size:(NSSize)newSize {
   
        NSImage *composedImage = [[NSImage alloc] initWithSize:newSize];
        
        float width  = image.size.width;
        float height = image.size.height;
        
        float targetWidth  = newSize.width;
        float targetHeight = newSize.height;
        float scaleFactor  = 0.0;
        float scaledWidth  = targetWidth;
        float scaledHeight = targetWidth;
        
        NSPoint thumbnailPoint = NSZeroPoint;
        float widthFactor  = targetWidth / width;
        float heightFactor = targetHeight / height;
        
        if ( widthFactor < heightFactor )
            scaleFactor = widthFactor;
        else
            scaleFactor = heightFactor;
        
        scaledWidth  = width  * scaleFactor;
        scaledHeight = height * scaleFactor;
        
        if ( widthFactor < heightFactor )
            thumbnailPoint.y = (targetHeight - scaledHeight) * 0.5;
        
        else if ( widthFactor > heightFactor )
            thumbnailPoint.x = (targetWidth - scaledWidth) * 0.5;
        
        [composedImage lockFocus];
        NSRect thumbnailRect;
        thumbnailRect.origin = thumbnailPoint;
        thumbnailRect.size.width = scaledWidth;
        thumbnailRect.size.height = scaledHeight;
        [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
        NSBezierPath *clipPath = [NSBezierPath bezierPathWithRoundedRect:thumbnailRect xRadius:10 yRadius:10];
        [clipPath setWindingRule:NSEvenOddWindingRule];
        [clipPath addClip];
        [image drawInRect:thumbnailRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
        
        [composedImage unlockFocus];
        
    
    return composedImage;
}

+ (NSImage*) compositeImage:(NSImage*)overlay mainImage:(NSImage*)mainImage withSize:(NSSize) newSize {
    
    
    NSImage *finalImage = [[NSImage alloc] initWithSize:NSMakeSize(newSize.width , newSize.height)];
    
    
    [finalImage lockFocus];
    
    [mainImage drawInRect:NSMakeRect(0, 0, newSize.width, newSize.height)
                 fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
    
    [overlay drawInRect:NSMakeRect(0, 0, newSize.width, newSize.height)
               fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
    
    [finalImage unlockFocus];
    return finalImage;
}

+ (NSImage*)roundCorners:(NSImage *)image withSize:(NSSize)newSize
{
    
    NSImage *composedImage = [[NSImage alloc] initWithSize:newSize];
    
    [composedImage lockFocus];
    
    [composedImage setFlipped:YES];
    [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
    
    NSRect imageFrame =  NSRectFromCGRect(CGRectMake(0, 0, newSize.width, newSize.height));
    NSBezierPath *clipPath = [NSBezierPath bezierPathWithRoundedRect:imageFrame xRadius:10 yRadius:10];
    [clipPath setWindingRule:NSEvenOddWindingRule];
    [clipPath addClip];

//    [image drawInRect: NSMakeRect(0, 0, newSize.width, newSize.height) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
//    [NSGraphicsContext saveGraphicsState];
//    
//    NSAffineTransform *transform = [NSAffineTransform transform];
//    [transform translateXBy:point.x yBy:point.y + [image size].height];
//    [transform scaleXBy:1.0 yBy:-1.0];
//    [transform set];
    [image drawAtPoint:NSZeroPoint fromRect: NSZeroRect operation:NSCompositeSourceOver fraction:1];
//    [NSGraphicsContext restoreGraphicsState];

//    [composedImage unlockFocus];
    
    return composedImage;
}

+(NSImage*)drawingPopupWithWaterMark:(NSImage*) image withSize:(NSSize)size {
    
   // NSSize size = image.size;
    NSImage* watermark = [NSImage imageNamed:@"watermark_popup"];
    watermark = [self thunbnailWaterMarkImage:watermark size:size];
    return [[self class] compositeImage:watermark mainImage:image withSize:size];
    
}


+ (NSImage*)resizeImage :(NSImage*)image size:(NSSize)newSize {
    
    NSImage *composedImage = [[NSImage alloc] initWithSize:newSize];
    
    float width  = image.size.width;
    float height = image.size.height;
    
    float targetWidth  = newSize.width;
    float targetHeight = newSize.height;
    float scaleFactor  = 0.0;
    float scaledWidth  = targetWidth;
    float scaledHeight = targetWidth;
    
    NSPoint thumbnailPoint = NSZeroPoint;
    float widthFactor  = targetWidth / width;
    float heightFactor = targetHeight / height;
    
    if ( widthFactor < heightFactor )
        scaleFactor = widthFactor;
    else
        scaleFactor = heightFactor;
    
    scaledWidth  = width  * scaleFactor;
    scaledHeight = height * scaleFactor;
    
    if ( widthFactor < heightFactor )
        thumbnailPoint.y = (targetHeight - scaledHeight) * 0.5;
    
    else if ( widthFactor > heightFactor )
        thumbnailPoint.x = (targetWidth - scaledWidth) * 0.5;
    
    [composedImage lockFocus];
    NSRect thumbnailRect;
    thumbnailRect.origin = thumbnailPoint;
    thumbnailRect.size.width = scaledWidth;
    thumbnailRect.size.height = scaledHeight;
    [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
//    NSBezierPath *clipPath = [NSBezierPath bezierPathWithRoundedRect:thumbnailRect xRadius:0.0 yRadius:0.0];
//    [clipPath setWindingRule:NSEvenOddWindingRule];
//    [clipPath addClip];
    [image drawInRect:thumbnailRect fromRect:NSZeroRect operation:NSCompositeCopy fraction:1.0];
    
    [composedImage unlockFocus];
    
    
    return composedImage;
}

+ (NSSize)scaleImage:(NSImage*) image  withSize:(NSSize)newSize{
    
        float width  = image.size.width;
        float height = image.size.height;
    
    
        float targetWidth  = newSize.width;
        float targetHeight = newSize.height;
    
        if (targetWidth <= targetHeight) {
            if (targetWidth > 640) {
                targetWidth = 640;
            }
        }
        else if (targetWidth > targetHeight){
            
            if (targetHeight > 640) {
                targetHeight = 640;
            }
        }

        float scaleFactor  = 0.0;
        float scaledWidth  = targetWidth;
        float scaledHeight = targetWidth;
        
        float widthFactor  = targetWidth / width;
        float heightFactor = targetHeight / height;
        
        if ( widthFactor < heightFactor )
            scaleFactor = widthFactor;
        else
            scaleFactor = heightFactor;
        
        scaledWidth  = width  * scaleFactor;
        scaledHeight = height * scaleFactor;
    return NSMakeSize(scaledWidth, scaledHeight);
    

}



@end
