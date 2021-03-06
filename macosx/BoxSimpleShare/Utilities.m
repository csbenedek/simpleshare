//
//  Utilities.m
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 12/8/11.
//

#import "Utilities.h"
#import <Security/Security.h>
#import <ServiceManagement/ServiceManagement.h>

static const char *kKeychainAccountName = "OAuth";

@interface Utilities (Private)

+ (BOOL)loginItemForAppPresentShouldDelete:(BOOL)delete;

@end

@implementation Utilities

#pragma mark - Running on Lion

+ (BOOL)isRunningOnLion {
    SInt32 minorVersion;
    Gestalt(gestaltSystemVersionMinor, &minorVersion);
    return minorVersion >= 7;
}

#pragma mark - Startup item management

+ (BOOL)loginItemForAppPresentShouldDelete:(BOOL)delete {


    BOOL isEnabled  = NO;
    NSString* _identifier = @"com.simpleshare.LoginHelper";
    // the easy and sane method (SMJobCopyDictionary) can pose problems when sandboxed. -_-
    CFArrayRef cfJobDicts = SMCopyAllJobDictionaries(kSMDomainUserLaunchd);
    NSArray* jobDicts = CFBridgingRelease(cfJobDicts);
    
    if (jobDicts && [jobDicts count] > 0) {
        for (NSDictionary* job in jobDicts) {
            if ([_identifier isEqualToString:[job objectForKey:@"Label"]]) {
                isEnabled = [[job objectForKey:@"OnDemand"] boolValue];
                break;
            }
        }
    }
    
//    if (isEnabled != _enabled) {
//        [self willChangeValueForKey:@"enabled"];
//        _enabled = isEnabled;
//        [self didChangeValueForKey:@"enabled"];
//    }
    
    return isEnabled;
}



+ (BOOL)setStartAtLogin:(BOOL)enabled {

	if (!SMLoginItemSetEnabled((CFStringRef)@"com.simpleshare.LoginHelper",
                               enabled)) {
		DbgLog(@"SMLoginItemSetEnabled failed!");
        return FALSE;
	}
    return YES;
}

+ (void)addAppAsStartupItem {
    
    [self setStartAtLogin:YES];
    
//    if (![self shouldLaunchOnSystemStartup]) {
//        
//        NSString* appPath = [[NSBundle mainBundle] bundlePath];
//        
//        CFURLRef url = (CFURLRef)[NSURL fileURLWithPath:appPath]; 
//        
//        LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
//                                                                kLSSharedFileListSessionLoginItems, NULL);
//        if (loginItems) {
//            //Insert an item to the list.
//            LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(loginItems,
//                                                                         kLSSharedFileListItemLast, NULL, NULL,
//                                                                         url, NULL, NULL);
//            if (item){
//                CFRelease(item);
//            }
//            CFRelease(loginItems);
//        }	
//    }
}

+ (void)deleteAppFromLoginItem {
    
    [self setStartAtLogin:NO];
//    if ([self shouldLaunchOnSystemStartup]) {
//        [self loginItemForAppPresentShouldDelete:YES];
//    }
}

+ (BOOL)shouldLaunchOnSystemStartup {
    return [self loginItemForAppPresentShouldDelete:NO];
}

#pragma mark - Working with system Keychain

+ (NSString*)prefsKeyForName:(NSString*)itemName {
    return [NSString stringWithFormat:@"Token : %@", itemName];
}

+ (void)saveAuthToken:(NSString*)token forKeychainItemName:(NSString*)keychainItemName {
    if (token.length > 0 && keychainItemName.length > 0) {
        [self removeAuthTokenForKeychainItemName:keychainItemName];
        
        SecKeychainRef defaultKeychain = NULL;
        SecKeychainItemRef* dontWantItemRef= NULL;
        const char* utf8ServiceName = [keychainItemName UTF8String];
        const char* utf8Token = [token UTF8String];
        
        OSStatus err = SecKeychainAddGenericPassword(defaultKeychain,
                                                     (UInt32)strlen(utf8ServiceName), utf8ServiceName,
                                                     (UInt32)strlen(kKeychainAccountName), kKeychainAccountName,
                                                     (UInt32)strlen(utf8Token), utf8Token,
                                                     dontWantItemRef);
        BOOL didSucceed = (err == noErr);
        if (didSucceed) {
            NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
            [defaults setBool:YES forKey:[self prefsKeyForName:keychainItemName]];
            [defaults synchronize];
        }
    }

}

+ (void)removeAuthTokenForKeychainItemName:(NSString*)keychainItemName {
    SecKeychainRef defaultKeychain = NULL;
    SecKeychainItemRef itemRef = NULL;
    const char* utf8ServiceName = [keychainItemName UTF8String];
    
    OSStatus err = SecKeychainFindGenericPassword(defaultKeychain,
                                                  (UInt32)strlen(utf8ServiceName), utf8ServiceName,
                                                  (UInt32)strlen(kKeychainAccountName), kKeychainAccountName,
                                                  0, NULL, // ignore password
                                                  &itemRef);
    if (err == noErr) {
        SecKeychainItemDelete(itemRef);
        CFRelease(itemRef);
        
        // remove our preference key
        NSString* prefKey = [self prefsKeyForName:keychainItemName];
        NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
        [defaults removeObjectForKey:prefKey];
        [defaults synchronize];
    }

}

+ (NSString*)authTokenForKeychainItemName:(NSString*)keychainItemName {
    
    NSString* prefKey = [self prefsKeyForName:keychainItemName];
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    if (![defaults boolForKey:prefKey]) {
        return nil;
    }
    
    SecKeychainRef defaultKeychain = NULL;
    const char* utf8ServiceName = [keychainItemName UTF8String];
    SecKeychainItemRef* dontWantItemRef = NULL;
    
    void* tokenBuff = NULL;
    UInt32 tokenBuffLength = 0;
    
    OSStatus err = SecKeychainFindGenericPassword(defaultKeychain,
                                                  (UInt32)strlen(utf8ServiceName), utf8ServiceName,
                                                  (UInt32)strlen(kKeychainAccountName), kKeychainAccountName,
                                                  &tokenBuffLength, &tokenBuff,
                                                  dontWantItemRef);
    
    NSString* token = nil;
    
    if (err == noErr && tokenBuff != NULL) {
        
        token = [[[NSString alloc] initWithBytes:tokenBuff
                                          length:tokenBuffLength
                                        encoding:NSUTF8StringEncoding] autorelease];
        
        SecKeychainItemFreeContent(NULL, tokenBuff);
        
    }
    return token;
}

+(BOOL)isRetinalSupported
{
    float displayScale = 1;
    if ([[NSScreen mainScreen] respondsToSelector:@selector(backingScaleFactor)]) {
        NSArray *screens = [NSScreen screens];
        for (int i = 0; i < [screens count]; i++) {
            float s = [[screens objectAtIndex:i] backingScaleFactor];
            if (s > displayScale)
                displayScale = s;
        }
    }
    return displayScale > 1;
}
+(NSImage*)resizeImage:(NSImage *)sourceImage newSize:(CGSize)newSize
{
    
    
//    NSString *newImagePath = [[[Utility documentPath] stringByAppendingPathComponent:imagefolder] stringByAppendingPathComponent:@"imageName"];
    
    
    
    [sourceImage setScalesWhenResized:YES];
    [sourceImage setSize:newSize];
    
    
    NSImage *newImage = [[NSImage alloc] initWithSize:newSize];
    
    [newImage lockFocus];
    
    NSRect frame = NSMakeRect(0, 0, newSize.width, newSize.height);
    
    [NSGraphicsContext saveGraphicsState];
    
    NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:frame xRadius:0 yRadius:0];
    [path addClip];
    
    [sourceImage drawInRect:frame fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
    
    [NSGraphicsContext restoreGraphicsState];
    
    [newImage unlockFocus];

    return newImage;
//    CGImageRef CGImage = [newImage CGImageForProposedRect:nil context:nil hints:nil];
//    NSBitmapImageRep *imgRep = [[[NSBitmapImageRep alloc] initWithCGImage:CGImage] autorelease];
//    
//    
//    NSData *data = [imgRep representationUsingType:NSPNGFileType properties: nil];
//    
//    [newImage release];
    
}


@end
