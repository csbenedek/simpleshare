//
//  Utilities.m
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 12/8/11.
//

#import "Utilities.h"
#import <Security/Security.h>

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
    NSString* appPath = [[NSBundle mainBundle] bundlePath];
    
	CFURLRef url = (CFURLRef)[NSURL fileURLWithPath:appPath]; 
    
	LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
                                                            kLSSharedFileListSessionLoginItems, NULL);
    
	if (loginItems) {
		UInt32 seedValue;
		NSArray* loginItemsArray = (NSArray *)LSSharedFileListCopySnapshot(loginItems, &seedValue);
		for (int i = 0; i < [loginItemsArray count]; i++){
			LSSharedFileListItemRef itemRef = (LSSharedFileListItemRef)[loginItemsArray objectAtIndex:i];
			//Resolve the item with URL
			if (LSSharedFileListItemResolve(itemRef, 0, (CFURLRef*)&url, NULL) == noErr) {
				NSString* urlPath = [(NSURL*)url path];
				if ([urlPath compare:appPath] == NSOrderedSame){
                    if (delete) {
                        LSSharedFileListItemRemove(loginItems, itemRef);
                    }
                    [loginItemsArray release];
                    return !delete;
				}
			}
		}
		[loginItemsArray release];
        CFRelease(loginItems);
	}
    return NO;
}

+ (void)addAppAsStartupItem {
    if (![self shouldLaunchOnSystemStartup]) {
        
        NSString* appPath = [[NSBundle mainBundle] bundlePath];
        
        CFURLRef url = (CFURLRef)[NSURL fileURLWithPath:appPath]; 
        
        LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
                                                                kLSSharedFileListSessionLoginItems, NULL);
        if (loginItems) {
            //Insert an item to the list.
            LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(loginItems,
                                                                         kLSSharedFileListItemLast, NULL, NULL,
                                                                         url, NULL, NULL);
            if (item){
                CFRelease(item);
            }
            CFRelease(loginItems);
        }	
    }
}

+ (void)deleteAppFromLoginItem {
    if ([self shouldLaunchOnSystemStartup]) {
        [self loginItemForAppPresentShouldDelete:YES];
    }
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

@end
