//
//  MainController.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/8/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "MainController.h"
#import "BoxSimpleShareAppDelegate.h"

//#import "BoxUser.h"
#import "BoxNetUser.h"
#import "BoxNetHandler.h"
#import "UploadOperation.h"
#import "PreferenceManager.h"
#import "OAuth2Client.h"
#import "Mixpanel.h"
#import "HTTPRequestHandler.h"
#import "ASINetworkQueue.h"
#import "Extended.h"
#import "Utilities.h"
#import "LoginWindowController.h"
#import "YoutubeAuthenticateManager.h"
#import "YoutubeUploadWindowController.h"
#import "ITSwitch.h"

static NSString* SignUpURL = @"https://www.box.com/signup/personal";
static NSString* LearnMoreURL = @"https://app.box.com/signup/personal/";  //@"http://www.box.com/business/features/";

@interface MainController (Private)

- (void)setUser:(BoxNetUser*)user;

@end

@implementation MainController

@synthesize loginName; 
@synthesize loginID;
@synthesize loginUserImage;
@synthesize logoutBtn;
@synthesize goToUploadFolderBtn;
@synthesize loginBtn;
@synthesize verField1;
@synthesize verField2;
@synthesize storageStatus;
@synthesize progressIndicator;
@synthesize statusLine1;
@synthesize statusLine2;
@synthesize switchViews;
@synthesize requestsTable;
@synthesize msg;
@synthesize requestsTableScrollView;
@synthesize imageHost;
@synthesize delete_screenshot_after_upload_check;
@synthesize delete_all_after_upload_check;
@synthesize disable_automatic_upload_check;
@synthesize copy_url_to_clipboard_check;
@synthesize shorten_links_check;
@synthesize launch_at_startup_check;
@synthesize mute_audio_check;
@synthesize screencast_format_index;
@synthesize screen_cast_hot_key;
@synthesize upload_hot_key;
@synthesize screencast_format;
@synthesize uploadhost_index = uploadHost_index;
@synthesize upload_video_host_index;
@synthesize window;

- (id) init
{
    if ((self = [super init]))
    {
        AddNotificationObserver(self, @selector(loginResponse:), @"LOGIN_RESPONSE", nil);
        AddNotificationObserver(self, @selector(updateMainViewContent:), @"UPDATE_MAINVIEW_CONTENT", nil);
        
        // REQUEST 
        AddNotificationObserver(self, @selector(updateRequestProgress:), REQUEST_PROGRESS, nil);
        AddNotificationObserver(self, @selector(updateQueueProgress:), QUEUE_PROGRESS, nil);
        AddNotificationObserver(self, @selector(youTubeLoginSuccessed:), YOUTUBE_LOGIN, nil);
    }
    return self;
}

- (void)awakeFromNib {
    
//    NSFont* font = [registerForFreeLabel font];
//    NSAttributedString* string = [NSAttributedString hyperlinkFromString:InterfaceString(@"RegisterString") 
//                                                                 withURL:[NSURL URLWithString:@""]
//                                                                    font:font];
//    [registerForFreeLabel setAttributedStringValue:string];
//    
//    font = learnMoreLabel.font;
//    string = [NSAttributedString hyperlinkFromString:InterfaceString(@"LearMoreString") 
//                                             withURL:[NSURL URLWithString:@""]
//                                                font:font];
//    [learnMoreLabel setAttributedStringValue:string];
    
    prefSyncTimer = [NSTimer scheduledTimerWithTimeInterval:60 target:self selector:@selector(serializePrefIfDirty) userInfo:nil repeats:YES];
    
    
    self.launch_at_startup_check = [Utilities shouldLaunchOnSystemStartup];
    
    
    youtubeWindowController = [[LoginWindowController alloc] initWithWindowNibName:@"LoginWindowController"];
    
    NSDictionary* dict = [BoxNetUser youTubeUser];
    if (dict) {
        [[YoutubeAuthenticateManager shareManager] updateAuthenticateToken:dict];
        
        self.isYouTubeLogin = TRUE;
        
        NSLog(@"YouTube user loaded");
        
    }
    
    //load Imgur preference
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    
    self.isImgur = [defaults  boolForKey:@"isImgur"];
    
    
    
    
    //it seems this is not used anymore
    //[self setYoutubState:dict == nil];
    
    [copy_url_to_clipboard setTintColor:[NSColor greenColor]];
    [delete_screenshot_after_upload setTintColor:[NSColor greenColor]];
    [delete_all_after_upload setTintColor:[NSColor greenColor]];
    [launchAtLogin setTintColor:[NSColor greenColor]];
    [shorten_links setTintColor:[NSColor greenColor]];

   // [YoutubeUploadWindowController sharedWindowController];
}

- (void) setAppVersion
{
    [verField1 setStringValue:APP_VERSION];
    [verField2 setStringValue:APP_VERSION];
    
    [progressIndicator setMinValue:0.0];
    [progressIndicator setMaxValue:1.0];
    [progressIndicator setDoubleValue:0.0];
    [statusLine1 setStringValue:@"Idle"];
    [statusLine2 setStringValue:[NSString stringWithFormat:@"Request(s) in Queue: %i", 0]];
    
    // try to load saved user
//    BoxUser* savedUser = [BoxUser savedUser];
//    if (savedUser) {
//        [self setBoxUser:savedUser];
//    }
//    [userEmail setStringValue:@"sap.dcc@gmail.com"];
//    [userPassword setStringValue:@"sapDcc123-"];
    
//    BoxNetUser* savedUser = [BoxNetUser savedUser];
//    if (savedUser) {
//        [self setUser:savedUser];
//    }
        
//    [userEmail setStringValue:@"alexandr.kievsky@gmail.com"];
//    [userPassword setStringValue:@"qwerty1234"];
    
    [requestsTable setDataSource:self];
    [requestsTable setDelegate:self];
}

- (void) wireAllButtons
{
    [loginBtn setTarget:self];
    [loginBtn setAction:@selector(doLogin:)];
    
    [logoutBtn setTarget:self];
    [logoutBtn setAction:@selector(doLogout:)];
    
    [switchViews setTarget:self];
    [switchViews setAction:@selector(switchView:)];
    
    [goToUploadFolderBtn setTarget:self];
    [goToUploadFolderBtn setAction:@selector(gotoUploadFolder:)];
    
    [screencast_format setTarget:self];
    [screencast_format setAction:@selector(screencastFormatValueChanged:)];
    
    [imageHost setTarget:self];
    [imageHost setAction:@selector(uploadhostChanged:)];
    
    //[self.youtubeLogoutBtn setTarget:self];
    //[self.youtubeLogoutBtn setAction:@selector(doYoutubeLogin:)];
    
}
#pragma mark
#pragma mark Process Notifications methods

-(void)processDoLogoutNotification:(NSNotification *)notification{
    
    [self doLogout:self];
    
    
}


#pragma mark
#pragma mark Actions

- (void) gotoUploadFolder:(id)sender
{
//  NSString* uploadFolderPublicName = [[BoxNetHandler sharedHandler] defaultFolderPublicName];
//	NSString *boxAccountUrl = [NSString stringWithFormat:@"http://www.box.com/shared/%@", uploadFolderPublicName];
//	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:boxAccountUrl]];
	
	NSString* uploadFolderPublicName = [[BoxNetHandler sharedHandler] defaultFolderPublicName];
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:uploadFolderPublicName]];
}

// PREFERENCE PAGE

- (NSString*)videoFormatExtension {
    return @"mp4";// [screencast_format objectValueOfSelectedItem];
}

- (void) screencastFormatValueChanged:(id)sender
{
    screencast_format_index = (int)[screencast_format indexOfSelectedItem];
    prefChanged = YES;
}

-(void)uploadhostChanged :(id) sender
{
    uploadHost_index = (int)[imageHost indexOfSelectedItem];
    prefChanged = YES;

}

- (IBAction) preferencePageValueChangeNotification:(id)sender
{
    // mark preference as dirty and need to be serialized
    prefChanged = YES;
}

- (void) serializePrefIfDirty
{
    if (prefChanged)
    {
        prefChanged = NO;
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            [PreferenceManager savePreference];
        });
    }
}

// -------------------------------------------

- (void) doLogin:(id)sender {
    [loginBtn setEnabled:NO];
    [errorMsg setStringValue:@""];
/*
    if ([[userEmail stringValue] isEqual:@""] || [[userPassword stringValue] isEqual:@""]) {
        return;
    }
*/
    //PostNotificationWithObject(@"SHOW_LOADING", [NSString stringWithString:@"Logging In ..."]);

	[[OAuth2Client sharedInstance] authorize];
	
	/*
    NSMutableDictionary *dic = [NSMutableDictionary dictionary];
    [dic setValue:[userEmail stringValue] forKey:USERNAME];
    [dic setValue:[userPassword stringValue] forKey:PASSWORD];
     // Perform Login using the given credentials
    [[BoxNetHandler sharedHandler] performLogin:dic];
    // To prevent text fields from receiving first responder
    [userEmail setEditable:NO];
    [userPassword setEditable:NO]; */

    [window makeFirstResponder:nil];
}

- (void) doLogout:(id)sender
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
        [PreferenceManager savePreference];
    });
    
//    [BoxUser removeSavedUser];
    [BoxNetUser removeSavedUser];
    PostNotificationWithObject(UPDATE_STATUS_ITEM, @"LOGOUT");
    PostNotificationWithObject(QUEUE_PROGRESS, @"LOGOUT");
    PostNotificationWithObject(REQUEST_PROGRESS, @"LOGOUT");
    
    [[HTTPRequestHandler sharedHandler] deallocSharedHandler];
    [[HTTPRequestHandler uploadHandler] deallocUploadHandler];

	[[OAuth2Client sharedInstance] reset];
	[[OAuth2Client sharedInstance] deallocSharedInstance];
	
	[[Mixpanel sharedInstance] deallocSharedInstance];
	
    [[BoxNetHandler sharedHandler] deallocSharedHandler];
    
    // Clear Credentials
    //    [userEmail setStringValue:@""];
    //    [userPassword setStringValue:@""];
    
    [[BoxSimpleShareAppDelegate sharedDelegate] createMenu];
    //[[BoxSimpleShareAppDelegate sharedDelegate] showLoginView];
}

- (void) switchView:(id)sender
{
    NSPanel *panel = [[BoxSimpleShareAppDelegate sharedDelegate] uploadStatusPanel];
    
    NSRect frame = NSZeroRect; 
    
    if (![requestsTable isHidden])
    {
        [requestsTable setHidden:YES];
        
        frame = [panel frame]; 
        
        NSMutableDictionary *dic1 = [NSMutableDictionary dictionary];
        [dic1 setObject:panel forKey:NSViewAnimationTargetKey];
        [dic1 setObject:[NSValue valueWithRect:NSMakeRect(NSMinX(frame), NSMinY(frame) + 163, NSWidth(frame), NSHeight(frame) - 163)] forKey:NSViewAnimationEndFrameKey];
        
        frame = [switchViews frame]; 
        
        NSMutableDictionary *dic2 = [NSMutableDictionary dictionary];
        [dic2 setObject:switchViews forKey:NSViewAnimationTargetKey];
        [dic2 setObject:[NSValue valueWithRect:NSMakeRect(NSMinX(frame), NSMinY(frame), NSWidth(frame), NSHeight(frame))] forKey:NSViewAnimationEndFrameKey];
        
        frame = [msg frame]; 
        
        NSMutableDictionary *dic3 = [NSMutableDictionary dictionary];
        [dic3 setObject:msg forKey:NSViewAnimationTargetKey];
        [dic3 setObject:[NSValue valueWithRect:NSMakeRect(NSMinX(frame), NSMinY(frame), NSWidth(frame), NSHeight(frame))] forKey:NSViewAnimationEndFrameKey];
        
        //frame = [requestsTableScrollView frame]; 
        
        NSMutableDictionary *dic4 = [NSMutableDictionary dictionary];
        [dic4 setObject:requestsTableScrollView forKey:NSViewAnimationTargetKey];
        [dic4 setObject:NSViewAnimationFadeOutEffect forKey:NSViewAnimationEffectKey];
        
        NSViewAnimation *theAnim = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray
                                                                   arrayWithObjects:dic1, dic2, dic3, dic4, nil]];
        [theAnim setDuration:0.3];
        [theAnim setAnimationCurve:NSAnimationEaseIn];
    
        [theAnim startAnimation];
        [theAnim release];
    }
    else
    {
        [requestsTable setHidden:NO];
        [requestsTableScrollView setHidden:NO];
        
        frame = [panel frame]; 
        
        NSMutableDictionary *dic1 = [NSMutableDictionary dictionary];
        [dic1 setObject:panel forKey:NSViewAnimationTargetKey];
        [dic1 setObject:[NSValue valueWithRect:NSMakeRect(NSMinX(frame), NSMinY(frame) - 163, NSWidth(frame), NSHeight(frame) + 163)] forKey:NSViewAnimationEndFrameKey];
        
        frame = [switchViews frame]; 
        
        NSMutableDictionary *dic2 = [NSMutableDictionary dictionary];
        [dic2 setObject:switchViews forKey:NSViewAnimationTargetKey];
        [dic2 setObject:[NSValue valueWithRect:NSMakeRect(NSMinX(frame), NSMinY(frame), NSWidth(frame), NSHeight(frame))] forKey:NSViewAnimationEndFrameKey];
        
        frame = [msg frame]; 
        
        NSMutableDictionary *dic3 = [NSMutableDictionary dictionary];
        [dic3 setObject:msg forKey:NSViewAnimationTargetKey];
        [dic3 setObject:[NSValue valueWithRect:NSMakeRect(NSMinX(frame), NSMinY(frame), NSWidth(frame), NSHeight(frame))] forKey:NSViewAnimationEndFrameKey];
        
        //frame = [requestsTableScrollView frame]; 
        
        NSMutableDictionary *dic4 = [NSMutableDictionary dictionary];
        [dic4 setObject:requestsTableScrollView forKey:NSViewAnimationTargetKey];
        [dic4 setObject:NSViewAnimationFadeInEffect forKey:NSViewAnimationEffectKey];
        
        NSViewAnimation *theAnim = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray
                                                                                    arrayWithObjects:dic1, dic2, dic3, dic4, nil]];
        
        [theAnim setDuration:0.3];
        [theAnim setAnimationCurve:NSAnimationEaseIn];
        
        [theAnim startAnimation];
        [theAnim release];
    }
}

#pragma mark
#pragma mark Response Handlers

- (void) loginResponse:(NSNotification *)response {
    PostNotification(@"HIDE_LOADING");
    
    if ([[response object] isKindOfClass:[NSError class]]) {
        NSString* errorDomain = [(NSError*)[response object] domain];
        if ([errorDomain isEqualToString:@"net.box.loginerror"]) {
            NSBeginAlertSheet(InterfaceString(@"LoginFailed"), 
                              InterfaceString(@"OK"), 
                              nil, 
                              nil, 
                              self.window,
                              nil, 
                              nil, 
                              nil, 
                              nil, 
                              @"");
        }
        DbgLog(@"ERROR!");
    } else {
    
		[[NSFileManager defaultManager] changeCurrentDirectoryPath:TMP_PATH];
		
        [self updateMainViewContent:nil];
        [[BoxSimpleShareAppDelegate sharedDelegate] selectToolBarItem:0];
        //[[BoxSimpleShareAppDelegate sharedDelegate] performSelectorOnMainThread:@selector(showMainView) withObject:nil waitUntilDone:YES];

        [PreferenceManager loadPreference];
		[[Mixpanel sharedInstance] identify:[response object]];
        [[BoxSimpleShareAppDelegate sharedDelegate] createMenu];
        
        //init preference controller
        
        [[BoxSimpleShareAppDelegate sharedDelegate] loadPreferencesController];
        
        
        
        DbgLog(@"Login Successfull!");
        
        
        PostNotification(@"SuccessfulLoginNotification"); //see processShowSuccessfulLoginMessageNotification of AttachedWindowController
        
        PostNotification(@"LoadAccountInfoNotification"); //see processLoadAccountInfoNotification of Preferences Controller
        
    }

    [loginBtn setEnabled:YES];
    [userEmail setEditable:YES];
    [userPassword setEditable:YES];
}

- (void)loginFailed:(NSNotification*)notification {
    
}

- (void) updateMainViewContent:(id)notification
{
    BoxNetUser *user = [[BoxNetHandler sharedHandler] boxNetUser];
    
    if ([user login]) {
        [loginName setStringValue:[user login]];
    }
//    [loginID setStringValue:[user userID]];
    
    if ([user userAvatar])
    {
        // Load Image
        [loginUserImage setImageURL:[user userAvatar]];
    }

    CGFloat percentUsed = ((float)[user spaceUsed] / (float)[user spaceAmount]) * 100;
    //CGFloat usedInGB = (float)[user spaceUsed] / 1024 / 1024 / 1024;
    //CGFloat maxInGB = (float)[user spaceAmount] / 1024 / 1024 / 1024;
    //[storageStatus setStringValue:[NSString stringWithFormat:@"%.2f%% (%.1f GB) of %.1f GB used", percentUsed, usedInGB, maxInGB]];
    
    [storageStatus setStringValue:[NSString stringWithFormat:@"%.2f%% (%@) of %@ used", percentUsed, returnValueWithAccurateAnnotation([user spaceUsed]), returnValueWithAccurateAnnotation([user spaceAmount])]];
    
    user = nil;
}

- (void) updateQueueProgress:(NSNotification *)notification
{
    if ([[notification object] isKindOfClass:[HTTPRequestHandler class]])
    {
        HTTPRequestHandler *handler = [notification object];
    
        if ([handler isUploadQueue])
        {
            if ([[handler queue] requestsCount] > 0)
            {   
                [progressIndicator setDoubleValue:[handler progress]];
                [progressIndicator setMaxValue:[handler maxValue]];
                
                [statusLine1 setStringValue:[NSString stringWithFormat:@"Uploaded %@ of %@", returnValueWithAccurateAnnotation([handler bytesUploadedSoFar]), returnValueWithAccurateAnnotation([handler totalBytesToUpload])]];
                
                [[[BoxSimpleShareAppDelegate sharedDelegate] progressItem] setTitle:[NSString stringWithFormat:@"Uploaded %@ of %@", returnValueWithAccurateAnnotation([handler bytesUploadedSoFar]), returnValueWithAccurateAnnotation([handler totalBytesToUpload])]];
                
                [statusLine2 setStringValue:[NSString stringWithFormat:@"Request(s) in Queue: %i", [[[BoxNetHandler sharedHandler] operationQueue] operationCount]]];
            }
            else
            {
                [statusLine1 setStringValue:@"Idle"];
                [statusLine2 setStringValue:[NSString stringWithFormat:@"Request(s) in Queue: %i", [[[BoxNetHandler sharedHandler] operationQueue] operationCount]]];
                
                [[[BoxSimpleShareAppDelegate sharedDelegate] progressItem] setTitle:PROGRESS_MENU_ITEM_TEXT];
            }
        }
    
        handler = nil;
    }
    else
        if ([[notification object] isKindOfClass:[UploadOperation class]])
        {
            [statusLine2 setStringValue:[NSString stringWithFormat:@"Request(s) in Queue: %i", [[[BoxNetHandler sharedHandler] operationQueue] operationCount]]];
        }
    else
        if ([[notification object] isKindOfClass:[NSString class]])
        {
            if ([[notification object] isEqualToString:UPLOAD_FINISHED] || [[notification object] isEqualToString:@"LOGOUT"])
            {
                [statusLine1 setStringValue:@"Idle"];
                
                [statusLine2 setStringValue:[NSString stringWithFormat:@"Request(s) in Queue: %i", 0]];
                [progressIndicator setDoubleValue:0.0];
                
                [[[BoxSimpleShareAppDelegate sharedDelegate] progressItem] setTitle:PROGRESS_MENU_ITEM_TEXT];
            }
        }
}

- (void) updateRequestProgress:(NSNotification *)notification
{
//    if (notification && [notification object])
//    {
//        //DbgLog(@"updateProgress: %@", [notification object]);
//        
//        UploadOperation *opt = [notification object];
//        
//        [statusLine2 setStringValue:[NSString stringWithFormat:@"REQUEST >> Uploaded %@ of %@", returnValueWithAccurateAnnotation([opt bytesUploadedSoFar]), returnValueWithAccurateAnnotation([opt totalBytesToUpload])]];
//    }
    
    //DbgLog(@"updateProgress");
    
    [[[[BoxSimpleShareAppDelegate sharedDelegate] mainController] requestsTable] reloadData];
}

//-----
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    int i =  [[[BoxNetHandler sharedHandler] operationQueue] operationCount];
    return i;
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    if (![[[BoxNetHandler sharedHandler] operationQueue] operationCount] > 0)
        return nil;
    
    UploadOperation *opt = [[[[BoxNetHandler sharedHandler] operationQueue] operations] objectAtIndex:row]; 
    
    if ([[tableColumn identifier] isEqualToString:@"net.column.filename"])
    {
        return [opt FileName];
    }
    else
        if ([[tableColumn identifier] isEqualToString:@"net.column.size"])
        {
            return ([opt totalBytesToUpload] > 0 ? returnValueWithAccurateAnnotation([opt totalBytesToUpload]) : @"Calculating");
        }
        else
            if ([[tableColumn identifier] isEqualToString:@"net.column.status"])
            {
                return [opt getCurrentStatus];
            }
    
    opt = nil;
    
    return nil;
} 

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    safe_release(screen_cast_hot_key);
    safe_release(upload_hot_key);
    
    [prefSyncTimer invalidate];
    prefSyncTimer = nil;
        
//    [loginHelper release];

    
    [super dealloc];
}

#pragma mark -

- (void)setUser:(BoxNetUser*)user {
    [[BoxNetHandler sharedHandler] setUser:user token:user.token];
    
    [self updateMainViewContent:nil];
    [[BoxSimpleShareAppDelegate sharedDelegate] selectToolBarItem:0];
	[[BoxSimpleShareAppDelegate sharedDelegate] performSelectorOnMainThread:@selector(showMainView) withObject:nil waitUntilDone:YES];
    
    [PreferenceManager loadPreference];
    [[BoxSimpleShareAppDelegate sharedDelegate] createMenu];
}

/*- (void)setBoxUser:(BoxUser *)user 
{
    // Due to changes from direct login to secure login
    // we need to conform BoxUser model (from Box sdk) to BoxNetUser (custom model class)
    
    BoxNetUser* boxNetUser = [[BoxNetUser alloc] init];
    
    [boxNetUser setEmail:user.email];
    [boxNetUser setLogin:user.email];
    [boxNetUser setAccessID:[user.accessId stringValue]];
    [boxNetUser setIsAuthenticated:YES];
    [boxNetUser setUserID:[user.userId stringValue]];
    [boxNetUser setSpaceAmount:[user.storageQuota unsignedLongLongValue]];
    [boxNetUser setSpaceUsed:[user.storageUsed unsignedLongLongValue]];
    [boxNetUser setMaxUploadSize:[user.maxUploadSize unsignedLongLongValue]];
    
    
    [[BoxNetHandler sharedHandler] setUser:boxNetUser token:user.authToken];
    [boxNetUser release];
    
    [self updateMainViewContent:nil];
    [[BoxSimpleShareAppDelegate sharedDelegate] selectToolBarItem:0];
	[[BoxSimpleShareAppDelegate sharedDelegate] performSelectorOnMainThread:@selector(showMainView) withObject:nil waitUntilDone:YES];
    
    [PreferenceManager loadPreference];
    [[BoxSimpleShareAppDelegate sharedDelegate] createMenu];
    
}*/

/*- (void)loginCompletedWithUser:(BoxUser *)user 
{
    [self setBoxUser:user];
    // Save user to disc, to avoid login on next launch
    [user save];
    DbgLog(@"Login Successfull!");
}

- (void)loginFailedWithError:(NSError *)error 
{
    
}*/

-(IBAction)onVideoHostSelected:(id)sender
{
    self.upload_video_host_index = (int)[self.videoHost indexOfSelectedItem];
     prefChanged = YES;
    
    if (self.upload_video_host_index) {
//        if(![BoxNetUser youTubeUser])
//        {
            [self doYoutubeLogin:self];
//        }
       // [[YoutubeUploadWindowController sharedWindowController] prepareData];
    }
}
- (void)toggleUploadsEnabled {
    [self willChangeValueForKey:@"disable_automatic_upload_check"];
    disable_automatic_upload_check = !disable_automatic_upload_check;
    [self didChangeValueForKey:@"disable_automatic_upload_check"];
}

-(void)youTubeLoginSuccessed:(NSNotification*) notification
{
    NSDictionary* dict = notification.object;
    [BoxNetUser saveYoutubeUser:dict];
    
    [[YoutubeAuthenticateManager shareManager] updateAuthenticateToken:dict];
    
    self.isYouTubeLogin = TRUE;
    
}

-(void)doYoutubeLogout:(id)sender{
    
    [BoxNetUser removeYoubeUser];
    
    self.isYouTubeLogin = FALSE;
    
    
}




-(void)doYoutubeLogin:(id)sender
{
    
    [youtubeWindowController reset];
    [youtubeWindowController showWindow:nil];
//    if([BoxNetUser youTubeUser])
//    {
//        NSString* token = [[BoxNetUser youTubeUser] objectForKey:@"access_token"];
//        [BoxNetUser removeYoubeUser];
//        [self setYoutubState:YES];
//        [youtubeWindowController logout:token];
//
//    }
//    else
//    {
//        [youtubeWindowController reset];
//        [youtubeWindowController showWindow:nil];
//        
//    }
}

#pragma itswitch 

-(IBAction)onSwitchChange:(id)sender
{
    if (sender == shorten_links) {
        self.shorten_links_check = shorten_links.isOn;
    }
    self.shorten_links_check = shorten_links.isOn;
    self.delete_all_after_upload_check = delete_all_after_upload.isOn;
    self.delete_screenshot_after_upload_check = delete_screenshot_after_upload.isOn;
    self.copy_url_to_clipboard_check = copy_url_to_clipboard.isOn;
    self.launch_at_startup_check = launchAtLogin.isOn;
    
    [self preferencePageValueChangeNotification:nil];
}

-(void)updateSwitchControl
{
    shorten_links.isOn = self.shorten_links_check ;
    delete_all_after_upload.isOn = self.delete_all_after_upload_check ;
    delete_screenshot_after_upload.isOn = self.delete_screenshot_after_upload_check ;
    copy_url_to_clipboard.isOn = self.copy_url_to_clipboard_check ;
   launchAtLogin.isOn = self.launch_at_startup_check ;

}
@end
