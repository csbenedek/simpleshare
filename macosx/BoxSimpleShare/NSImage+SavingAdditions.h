

#import <Cocoa/Cocoa.h>

@interface NSImage (SavingAdditions)


- (BOOL)saveToFileAtPath:(NSString*)inPath;
- (NSData*)dataRepresentation:(NSString*)extension;
@end
