

#import "NSImage+Additions.h"

@implementation NSImage (Additions)
/**
 NSImage return incorrect size if we use size property. (See here: http://stackoverflow.com/questions/11876963/get-the-correct-image-width-and-height-of-an-nsimage)
 */
- (NSSize)getOriginalImageSize {
	NSImageRep *imageRepresentation = [[self representations] objectAtIndex:0];
	NSSize imageSize = NSMakeSize(imageRepresentation.pixelsWide, imageRepresentation.pixelsHigh);
	return imageSize;
}

- (NSImage *)createThumbnailWithSize:(CGFloat)aThumbnailSize {
	NSAssert(aThumbnailSize != 0, @"aThumbnailSize could not be 0.");
	
	NSScreen *screen = [NSScreen mainScreen];
	BOOL isRetina = screen.backingScaleFactor > 1;
	
	NSSize imageSize = [self getOriginalImageSize];
	float imageAR = imageSize.width / imageSize.height;
	
	float minSideSize = aThumbnailSize / (isRetina ? 2.0 : 1.0);
	float newW = minSideSize;
	float newH = newW/imageAR;
	
	if (newH < minSideSize) {
		newH = minSideSize;
		newW = newH * imageAR;
	}
	
	NSImage *resultImage = [[NSImage alloc] initWithSize:NSMakeSize(newW, newH)];
	
	[resultImage lockFocus];
	[self drawInRect:NSMakeRect(0.0, 0.0, newW, newH)
			fromRect:NSZeroRect
		   operation:NSCompositeSourceOver
			fraction:1.0];
	[resultImage unlockFocus];
	
	return resultImage;
}


-(NSImage*) scaledImageToFitSize: (NSSize)fitIn
{
    return [self scaledImageToFitSize: fitIn withInterpolation: NSImageInterpolationHigh];
}


-(NSImage*) scaledImageToFitSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter
{
    return [self scaledImageToFitSize: fitIn withInterpolation: inter andBox: NO];
}

-(NSImage*) scaledImageToFitSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter andBox: (BOOL)doBox
{
    NSSize	size = [self scaledSizeToFitSize: fitIn];
    NSImage*	img = [[NSImage alloc] initWithSize: fitIn];
    NSRect	srcBox = { { 0, 0 }, { 0, 0 } },
    dstBox = { { 0, 0 }, { 0, 0 } };
    
    dstBox.size = size;
    srcBox.size = [self size];
    dstBox.origin.x += (fitIn.width -size.width) /2;
    dstBox.origin.y += (fitIn.height -size.height) /2;
    
    NS_DURING
    [img lockFocus];
    if( doBox )
    {
        [[NSColor whiteColor] set];
        [NSBezierPath fillRect: dstBox];
        [[NSColor blackColor] set];
    }
    [[NSGraphicsContext currentContext] setImageInterpolation: inter];
    [self drawInRect: dstBox fromRect: srcBox operation:NSCompositeSourceOver fraction:1.0];
    if( doBox )
        [NSBezierPath strokeRect: dstBox];
    [img unlockFocus];
    
    NS_VALUERETURN( img, NSImage* );
    NS_HANDLER
    NSLog(@"Couldn't scale image %@",localException);
    NS_ENDHANDLER
    
    return nil;
}


-(NSImage*) scaledImageToCoverSize: (NSSize)fitIn
{
    return [self scaledImageToCoverSize: fitIn withInterpolation: NSImageInterpolationHigh];
}


-(NSImage*) scaledImageToCoverSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter
{
    return [self scaledImageToCoverSize: fitIn withInterpolation: inter andBox: NO];
}

-(NSImage*) scaledImageToCoverSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter andBox: (BOOL)doBox
{
    return [self scaledImageToCoverSize: fitIn withInterpolation: inter andBox: doBox align: NSImageAlignCenter];
}

-(NSImage*) scaledImageToCoverSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter
                            andBox: (BOOL)doBox align: (NSImageAlignment)align
{
    if (CGSizeEqualToSize(fitIn, CGSizeMake(0, 0)))
    {
        return nil;
    }
        
    NSSize	size = [self scaledSizeToCoverSize: fitIn];
    NSImage*	img = [[NSImage alloc] initWithSize: fitIn];
    NSRect	srcBox = { { 0, 0 }, { 0, 0 } },
    dstBox = { { 0, 0 }, { 0, 0 } },
    clipBox = { { 0, 0 }, { 0, 0 } };
    
    dstBox.size = size;
    srcBox.size = [self size];
    clipBox.size = fitIn;
    
    // Center it:
    switch( align )
    {
        case NSImageAlignCenter: // Center h and v.
            dstBox.origin.x -= (dstBox.size.width - clipBox.size.width) /2;
            dstBox.origin.y -= (dstBox.size.height - clipBox.size.height) /2;
            break;
            
        case NSImageAlignTop: // Center h, v at top.
            dstBox.origin.x -= (dstBox.size.width - clipBox.size.width) /2;
            dstBox.origin.y -= dstBox.size.height - clipBox.size.height;
            break;
            
        case NSImageAlignTopLeft: // h at left, v at top.
            dstBox.origin.y -= dstBox.size.height - clipBox.size.height;
            break;
            
        case NSImageAlignLeft: // h at left, center v.
            dstBox.origin.y -= (dstBox.size.height - clipBox.size.height) /2;
            break;
            
        case NSImageAlignBottomLeft: // h at left, v at bottom
        default:
            break;
            
        case NSImageAlignBottom: // center h, v at bottom.
            dstBox.origin.x -= (dstBox.size.width - clipBox.size.width) /2;
            break;
            
        case NSImageAlignBottomRight: // h at right, v at bottom.
            dstBox.origin.x -= dstBox.size.width - clipBox.size.width;
            break;
            
        case NSImageAlignRight: // h at right, center v.
            dstBox.origin.x -= dstBox.size.width - clipBox.size.width;
            dstBox.origin.y -= (dstBox.size.height - clipBox.size.height) /2;
            break;
            
        case NSImageAlignTopRight: // h at right, v at top.
            dstBox.origin.y -= dstBox.size.height - clipBox.size.height;
            dstBox.origin.x -= dstBox.size.width - clipBox.size.width;
            break;
            
    }
    
    NS_DURING
    [img lockFocus];
    [NSBezierPath clipRect: clipBox];
    [[NSGraphicsContext currentContext] setImageInterpolation: inter];
    [self drawInRect: dstBox fromRect: srcBox operation:NSCompositeSourceOver fraction:1.0];
    if( doBox )
    {
        [[NSColor blackColor] set];
        [NSBezierPath strokeRect: clipBox];
    }
    [img unlockFocus];
    
    NS_VALUERETURN( img, NSImage* );
    NS_HANDLER
    NSLog(@"Couldn't scale image %@",localException);
    NS_ENDHANDLER
    
    return nil;
}


-(NSSize) scaledSizeToFitSize: (NSSize)size
{
    return [[self class] scaledSize: [self size] toFitSize: size];
}


+(NSSize) scaledSize: (NSSize)imgSize toFitSize: (NSSize)size
{
    NSSize finalSize;
    
    float widthRatio = size.width / imgSize.width;
    float heightRatio = size.height / imgSize.height;
    
    if ( widthRatio < heightRatio )
    {
        finalSize.width = size.width;
        finalSize.height = imgSize.height * widthRatio;
    }
    else
    {
        finalSize.width = imgSize.width * heightRatio;
        finalSize.height = size.height;
    }
    
    return finalSize;
}


-(NSSize) scaledSizeToCoverSize: (NSSize)size
{
    return [[self class] scaledSize: [self size] toCoverSize: size];
}


+(NSSize) scaledSize: (NSSize)imgSize toCoverSize: (NSSize)size
{
    NSSize finalSize = imgSize;
    float ratio = imgSize.height / imgSize.width;
    
    /*if( imgSize.width == size.width
     && imgSize.height == size.height )
     return imgSize;*/
    
    finalSize.width = size.width;
    finalSize.height = finalSize.width * ratio;
    
    if( finalSize.height < size.height )
    {
        ratio = imgSize.width / imgSize.height;
        finalSize.height = size.height;
        finalSize.width = finalSize.height * ratio;
    }
    
    return( finalSize );
}
- (void) saveAsImageType: (NSBitmapImageFileType) imageType withDPI: (CGFloat) dpiValue atPath: (NSString *) filePath
{
    NSBitmapImageRep *rep = [[self representations] objectAtIndex: 0];
    
    NSSize pointsSize = rep.size;
    NSSize pixelSize = NSMakeSize(rep.pixelsWide, rep.pixelsHigh);
    
    CGFloat currentDPI = ceilf((72.0f * pixelSize.width)/pointsSize.width);
    NSLog(@"current DPI %f", currentDPI);
    
    NSSize updatedPointsSize = pointsSize;
    
    updatedPointsSize.width = ceilf((72.0f * pixelSize.width)/dpiValue);
    updatedPointsSize.height = ceilf((72.0f * pixelSize.height)/dpiValue);
    
    [rep setSize:updatedPointsSize];
    
    NSData *data = [rep representationUsingType: imageType properties: nil];
    [data writeToFile: filePath atomically: NO];
    
}

-(NSImage *)cw_imageResizedToSize:(CGSize)size
		 withInterpolationQuality:(CGInterpolationQuality)quality {
	CGImageSourceRef imageSource = CGImageSourceCreateWithData((__bridge CFDataRef)[self TIFFRepresentation], NULL);
	CGImageRef image = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);
	CGRect rect = { CGPointZero, size };
	CGContextRef context = CGBitmapContextCreate(NULL, size.width, size.height,
												 CGImageGetBitsPerComponent(image), 0,
												 CGImageGetColorSpace(image),
												 kCGBitmapAlphaInfoMask & kCGImageAlphaPremultipliedLast);
	if (context == NULL) {
		//CWLogInfo(@"ERROR: Received NULL CGContextRef");
		CFRelease(imageSource);
		CGImageRelease(image);
		return nil;
	}
    
	CGContextSetInterpolationQuality(context, quality);
	CGContextDrawImage(context, rect, image);
	CGImageRef cgImage = CGBitmapContextCreateImage(context);
	NSImage *nsimage = [[NSImage alloc] initWithCGImage:cgImage size:size];
    
	CGImageRelease(cgImage);
	CGContextRelease(context);
	CFRelease(imageSource);
	CGImageRelease(image);
    
	return nsimage;
}
@end