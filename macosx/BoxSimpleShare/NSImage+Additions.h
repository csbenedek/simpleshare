

#import <Cocoa/Cocoa.h>

@interface NSImage (Additions)

/**
 This method will create a thumbnail from existing image with size.
 
 aThumbnailSize could not be 0.
 */
- (NSImage *)createThumbnailWithSize:(CGFloat)aThumbnailSize;

// NSImage -> NSImage (fit):
-(NSImage*) scaledImageToFitSize: (NSSize)fitIn;
-(NSImage*) scaledImageToFitSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter;
-(NSImage*) scaledImageToFitSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter andBox: (BOOL)doBox;

// NSImage -> NSImage (cover):
-(NSImage*) scaledImageToCoverSize: (NSSize)fitIn;
-(NSImage*) scaledImageToCoverSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter;
-(NSImage*) scaledImageToCoverSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter andBox: (BOOL)doBox;
-(NSImage*) scaledImageToCoverSize: (NSSize)fitIn withInterpolation: (NSImageInterpolation)inter
                            andBox: (BOOL)doBox align: (NSImageAlignment)align;

// [NSImage size] -> NSSize:
-(NSSize) scaledSizeToFitSize: (NSSize)size;
-(NSSize) scaledSizeToCoverSize: (NSSize)size;

// NSSize -> NSSize:
+(NSSize) scaledSize: (NSSize)imgSize toFitSize: (NSSize)size;
+(NSSize) scaledSize: (NSSize)imgSize toCoverSize: (NSSize)size;
-(NSImage *)cw_imageResizedToSize:(CGSize)size withInterpolationQuality:(CGInterpolationQuality)quality;
@end
