//
//  FolderUtility.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 1/10/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "FolderUtility.h"
#import "StandardPaths.h"
#import "NSAlert+SynchronousSheet.h"
@implementation FolderUtility


-(BOOL)checkPermision {
    
    NSData *bookmark = [[NSUserDefaults standardUserDefaults] objectForKey:@"PathToFolder"];
    if (bookmark == nil) {
        [self tellUserTrustDesktopFolder];
    }
    
    return [self getPathToDesktopFolder];

}
-(BOOL) getPathToDesktopFolder {

    
    NSError *error = nil;
    NSData *bookmark = [[NSUserDefaults standardUserDefaults] objectForKey:@"PathToFolder"];
    if (bookmark != nil) {
        NSURL* bookmarkedURL = [NSURL URLByResolvingBookmarkData:bookmark options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:nil bookmarkDataIsStale:nil error:&error];
        
        BOOL ok = [bookmarkedURL startAccessingSecurityScopedResource];
        DbgLog(@"url = %@", [bookmarkedURL absoluteString]);
        return YES;
    }
    
    return NO;

    
}
-(BOOL)tellUserTrustDesktopFolder {

    
    NSString* message = @"SimpleShare requires access to your Desktop in order to upload files to Box. After closing this window, please click Allow' and then 'Open' to enable uploading from the Desktop.";
    
    NSAlert *alert = [NSAlert alertWithMessageText:@"SimpleShare: Getting Started" defaultButton:@"OK" alternateButton:nil otherButton:nil informativeTextWithFormat:message];
    [alert runModal];
    
    NSOpenPanel *openPanel = [[NSOpenPanel alloc] init];
    NSString* desktop = [[NSFileManager defaultManager] desktopPath];
    [openPanel setDirectoryURL:[NSURL fileURLWithPath:desktop]];
    [openPanel setTitle:@"Click \'Open\' to allow SimpleShare access to Desktop folder"];
    [openPanel setCanChooseFiles:NO];
    [openPanel setCanChooseDirectories:YES];
    [openPanel setCanCreateDirectories:NO];
    
    [openPanel beginWithCompletionHandler:^(NSInteger result){
        

        if (result == NSFileHandlingPanelOKButton) {
            for (NSURL *fileURL in [openPanel URLs]) {
                NSString *filename = [fileURL path];
                
                NSURL* url = [NSURL fileURLWithPath:[[NSFileManager defaultManager] desktopPath]];
                if(![filename hasPrefix:[url path]]) {
                    
                    continue;
                }
                [[NSUserDefaults standardUserDefaults] setObject:filename forKey:@"PathToFolder"];
                
                NSError *error = nil;
                NSData *bookmark = [fileURL bookmarkDataWithOptions:NSURLBookmarkCreationWithSecurityScope
                                     includingResourceValuesForKeys:nil
                                                      relativeToURL:nil
                                                              error:&error];
                if (error) {
                    DbgLog(@"Error creating bookmark for URL (%@): %@", fileURL, error);
                    return ;
                    //[NSApp presentError:error];
                } else {
                    [[NSUserDefaults standardUserDefaults] setObject:bookmark forKey:@"PathToFolder"];
                    [[NSUserDefaults standardUserDefaults] synchronize];
                    
                }
                break;
            }
        }        
    }];
}
@end
