

#import "NSImage+SavingAdditions.h"

@implementation NSImage (SavingAdditions)

- (BOOL)saveToFileAtPath:(NSString*)inPath;
{
    NSBitmapImageRep *bits = [self.representations objectAtIndex:0];
    
    NSData *data = nil;
    
//    if ([@"png" isEqualToString:inPath.pathExtension])
//        data = [bits representationUsingType: NSPNGFileType properties: nil];
//    else if ([@"jpg" isEqualToString:inPath.pathExtension])
//        data = [bits representationUsingType: NSJPEGFileType properties: nil];
//    else if ([@"bmp" isEqualToString:inPath.pathExtension])
//        data = [bits representationUsingType: NSBMPFileType properties: nil];
//    else
        data = [bits representationUsingType: NSTIFFFileType properties: nil];

    NSError* error;
    BOOL success = [data writeToFile: inPath options:NSDataWritingAtomic error:&error];
    if (!success)
        DbgLog(@"Error writing file to %@: %@", inPath, [error localizedDescription]);
    return success;
}

- (NSData*)dataRepresentation:(NSString*)extension;
{
    NSBitmapImageRep *bits = [self.representations objectAtIndex:0];
    
    NSData *data = nil;
    
    if ([@"png" isEqualToString:extension])
        data = [bits representationUsingType: NSPNGFileType properties: nil];
    else if ([@"jpg" isEqualToString:extension])
        data = [bits representationUsingType: NSJPEGFileType properties: nil];
    else if ([@"bmp" isEqualToString:extension])
        data = [bits representationUsingType: NSBMPFileType properties: nil];
    else
        data = [bits representationUsingType: NSTIFFFileType properties: nil];
    
    
    return  data;
}


@end
