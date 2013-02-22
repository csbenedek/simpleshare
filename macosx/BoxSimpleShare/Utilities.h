//
//  Utilities.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 12/8/11.
//


@interface Utilities : NSObject

+ (BOOL)isRunningOnLion;

+ (void)addAppAsStartupItem;
+ (void)deleteAppFromLoginItem;
+ (BOOL)shouldLaunchOnSystemStartup;

+ (void)saveAuthToken:(NSString*)token forKeychainItemName:(NSString*)keychainItemName;
+ (void)removeAuthTokenForKeychainItemName:(NSString*)keychainItemName;
+ (NSString*)authTokenForKeychainItemName:(NSString*)keychainItemName;

@end
