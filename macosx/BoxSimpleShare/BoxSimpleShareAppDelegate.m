//
//  BoxSimpleShareAppDelegate.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "BoxSimpleShareAppDelegate.h"
#import <CoreServices/CoreServices.h>

#import "AppConstants.h"
#import "MainController.h"
#import "PreferenceManager.h"
#import "BoxNetHandler.h"
#import "UKKQueue.h"
#import "Mixpanel.h"
#import "Extended.h"
#import "Utilities.h"

#import "OAuth2Client.h"

#import "LoadingView.h"

#import "BoxFile.h"
#import "BoxNetUser.h"

#import "Finder.h"

const int MaxHistoryItemCount = 5;

static const int MaxHistoryItemLength = 22;

static const int UploadHotKeyEventId = 11;
static const int VideoCaptureHotKeyEventId = 111;

static OSStatus HotKeyHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData);

@interface BoxSimpleShareAppDelegate (Private)

- (void)setupUploadHotKey;
- (void)setupVideoCaptureHotKey;

@end

@implementation BoxSimpleShareAppDelegate

@synthesize window;
@synthesize tableView;
@synthesize mainController;
@synthesize toolBar;
@synthesize mainView;
@synthesize preferenceView;
@synthesize loginView;
@synthesize uploadStatusPanel;
@synthesize statusBarImages;
@synthesize progressItem;
@synthesize filesUploadedInSession;

@synthesize uploadFileHotkeyRef;
@synthesize videoCaptureHotkeyRef;


- (void) applicationWillTerminate:(NSNotification *)notification;
{
    [PreferenceManager savePreference];
}

- (void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Check if we have plist file from previous version which contains user password in open form - delete password
    
    NSString* plistPath = [BASE_PATH stringByAppendingPathComponent:@"net.box.simpleshare.plist"];
    if ([[NSFileManager defaultManager] fileExistsAtPath:plistPath]) {
        NSMutableDictionary* dictionary = [[NSMutableDictionary alloc] initWithContentsOfFile:plistPath];
        [dictionary removeObjectForKey:@"password"];
        [dictionary removeObjectForKey:@"auth_token"];
        [[NSFileManager defaultManager] removeItemAtPath:plistPath error:NULL];
        [dictionary writeToFile:plistPath atomically:YES];
        [dictionary release];
    }
    
    SUPPORTED_FORMATS = [NSArray arrayWithObjects:@"flv", /* @"mp4",*/ @"avi", @"mov", nil];
    
    AddNotificationObserver(self, @selector(showLoading:), @"SHOW_LOADING", nil);
    AddNotificationObserver(self, @selector(hideLoading), @"HIDE_LOADING", nil);
    
    [mainController setAppVersion];
    [mainController wireAllButtons];
    [mainController addObserver:self
                     forKeyPath:@"disable_automatic_upload_check"
                        options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld 
                        context:NULL];
    [mainController addObserver:self
                     forKeyPath:@"upload_hot_key"
                        options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld 
                        context:NULL];
    [mainController addObserver:self
                     forKeyPath:@"screen_cast_hot_key"
                        options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld 
                        context:NULL];
    [mainController addObserver:self
                     forKeyPath:@"launch_at_startup_check"
                        options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld 
                        context:NULL];
        
    if (![[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated]) 
    {
        [self addAndResizeWindowForView:loginView];
    }/* else {
//        [self showMainView];
        [self addAndResizeWindowForView:mainView];
    }*/
    
    [[self window] setDelegate:self];
    [[self window] makeKeyWindow];
    [[self window] makeMainWindow];
    
    statusBarImages = [NSMutableArray new];
	
	[statusBarImages addObject:[NSImage imageNamed:@"default-icon.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-1.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-2.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-3.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-4.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-5.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-6.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-7.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-8.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-9.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-10.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-11.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-12.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-13.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-14.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-15.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-16.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-17.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-18.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"icon-animation-19.png"]];
	[statusBarImages addObject:[NSImage imageNamed:@"active-icon.png"]];
    
	
    [self createMenu];
    [statusBarItem setHighlightMode:YES];
    [statusBarItem setEnabled:YES];
    
    [uploadStatusPanel setDelegate:self];
    [uploadStatusPanel registerForDraggedTypes:[self acceptableTypes]];
        
    [GrowlApplicationBridge setGrowlDelegate:self];
    [GrowlApplicationBridge setWillRegisterWhenGrowlIsReady:YES]; 
    
    UKKQueue *kqueue = [UKKQueue sharedFileWatcher];
    NSString *path = [NSHomeDirectory() stringByAppendingPathComponent:@"Desktop"];
    [kqueue addPathToQueue:path];
    NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
    NSNotificationCenter *notificationCenter = [workspace notificationCenter];
    [notificationCenter addObserver:self 
                           selector:@selector(fileSystemNotificationsHandler:) 
                               name:UKFileWatcherWriteNotification 
                             object:nil];

    
    //
    filesAddedToQueue = [NSMutableSet new];
    [self setupUploadHotKey];
    [self setupVideoCaptureHotKey];
	
	if ([[OAuth2Client sharedInstance] isAuthorized])
	{
		PostNotificationWithObject(@"SHOW_LOADING", [NSString stringWithUTF8String:"Logging In ..."]);
		[[OAuth2Client sharedInstance] authorize];
	}
}

#pragma mark Hotkeys

- (void)installHotKey:(NSString*)hotKeyString eventId:(int)eventId eventHotKeyRef:(EventHotKeyRef*)hotKeyRef {
    static BOOL handlerInstalled = NO;
    if (!handlerInstalled) {
        EventHandlerUPP myUPP = NewEventHandlerUPP(HotKeyHandler); 
        EventTypeSpec event;
        event.eventClass = kEventClassKeyboard; 
        event.eventKind = kEventHotKeyPressed; 
        InstallApplicationEventHandler(myUPP, 1, &event, self, NULL);
        handlerInstalled = YES;
    }
      
    if ([hotKeyString length] > 0) {
        NSArray* keyCodes = [hotKeyString keyCodes];
        for (NSNumber* code in keyCodes) {
            int keyCode = [code intValue];
            unichar keyChar = [hotKeyString characterAtIndex:0];
            EventHotKeyID uploadFileHotkeyId = { keyChar, eventId };
            RegisterEventHotKey(keyCode,
                                shiftKey | cmdKey, 
                                uploadFileHotkeyId, 
                                GetApplicationEventTarget(), 
                                0,
                                hotKeyRef);
        }
    } else {
        *hotKeyRef = NULL;
    }
}

- (void)uninstallHotKey:(EventHotKeyRef)hotKeyRef {
    if (hotKeyRef) {
        UnregisterEventHotKey(hotKeyRef);  
    }
}

- (void)setupUploadHotKey {
    NSString* hotKeyString = [[mainController upload_hot_key] lowercaseString];
    [self installHotKey:hotKeyString eventId:UploadHotKeyEventId eventHotKeyRef:&uploadFileHotkeyRef];
}

- (void)uninstallUploadHotKey {
    [self uninstallHotKey:uploadFileHotkeyRef];
}

- (void)setupVideoCaptureHotKey {
    NSString* hotKeyString = [[mainController screen_cast_hot_key] lowercaseString];
    [self installHotKey:hotKeyString eventId:VideoCaptureHotKeyEventId eventHotKeyRef:&videoCaptureHotkeyRef];
}

- (void)uninstallVideoCaptureHotKey {
    [self uninstallHotKey:videoCaptureHotkeyRef];
}

#pragma mark Histrory

- (void)updateHistoryMenuImpl 
{
    int i = 0;
    for (BoxFile *file in [filesUploadedInSession reverseObjectEnumerator])
    {
        NSString* itemTitle = [file fileName];
        if ([itemTitle length] > MaxHistoryItemLength) {
            itemTitle = [itemTitle substringToIndex:MaxHistoryItemLength];
            itemTitle = [itemTitle stringByAppendingFormat:@"..."];
        }
        NSMenuItem *tmp = [[NSMenuItem alloc] initWithTitle:itemTitle action:@selector(openBoxNetFile:) keyEquivalent:@""];
        [tmp setImage:[NSImage imageNamed:[self getIconForType:[file fileName]]]];
        [tmp setTag:i];
        [popUpMenu addItem:tmp];
        [tmp release];
        ++i;
        if (i == MaxHistoryItemCount) {
            break;
        }
    }
}

- (void)updateHistoryMenu
{       
    [self createMenu]; 
}

#pragma mark -

- (void) openBoxNetFile:(id)sender
{
    NSUInteger idx = [filesUploadedInSession count] - [sender tag] - 1;
    BoxFile *file = [filesUploadedInSession objectAtIndex:idx];
    
    if (file)
    {
		// open the url using the file link
		NSString *url = nil;
		
		if ([file shortURL])
			url = [file shortURL];
		else
			//url = [NSString stringWithFormat:@"http://www.box.com/shared/%@", [file publicName]];
			url = [file publicName];
		
		if (mainController.shorten_links_check)
		{
			url = URLShortner( url );
		}
		
		[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:url]];
		url = nil;
    }
    
    file = nil;
}

- (void) createMenu
{
    if (!statusBarItem) {
        statusBarItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
        
        StatusItemView *view = [[StatusItemView alloc] initWithStatusItem:statusBarItem];
        [view setFrame:NSMakeRect(0, 0, 25, 50)];
        
        [statusBarItem setView:view];
        
        [view release];
        
        popUpMenu = [[NSMenu alloc] initWithTitle:@"MainMenu"];
        [statusBarItem setMenu:popUpMenu];
    }
    
    [popUpMenu removeAllItems];

    if (!account)
    {
        account = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"Account") action:nil keyEquivalent:@""];
    }

    if (!preference)
    {
        preference = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"Preferences") action:@selector(showPreferenceView) keyEquivalent:@""];
    }

    if (!login)
    {
        login = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"Login") action:@selector(showLoginView) keyEquivalent:@""];
    }

    NSMenuItem *history = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"History") action:nil keyEquivalent:@""];
    [history setEnabled:NO];
    
    NSMenuItem *quit = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"Quit") action:@selector(quitApplication) keyEquivalent:@""];
    
    NSMenuItem *capture = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"Capture") action:nil keyEquivalent:@""];

    if (!videoCaptureMenuItem) {
        videoCaptureMenuItem = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"VideoCapture") action:@selector(startVideoCapture:) keyEquivalent:@"5"];
        [videoCaptureMenuItem setKeyEquivalentModifierMask:NSCommandKeyMask | NSShiftKeyMask];
    }
    
    NSMenuItem *screenshot = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"CaptureRegion") action:@selector(triggerRegionCapture:) keyEquivalent:@"4"];
    [screenshot setKeyEquivalentModifierMask:NSCommandKeyMask | NSShiftKeyMask];

    NSMenuItem *screenshot_full = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"CaptureFullscreen") action:@selector(triggerFullScreenCapture:) keyEquivalent:@"3"];
    [screenshot_full setKeyEquivalentModifierMask:NSCommandKeyMask | NSShiftKeyMask];

    NSMenuItem *uploadfile = [[NSMenuItem alloc] initWithTitle:InterfaceString(@"Upload") action:@selector(selectAndUploadFile:) keyEquivalent:@"U"];
    [uploadfile setKeyEquivalentModifierMask:NSCommandKeyMask | NSShiftKeyMask];

    NSMenuItem *item0 = [NSMenuItem separatorItem];
        
    NSString *toggleTitle = mainController.disable_automatic_upload_check ? InterfaceString(@"EnableUploads") : InterfaceString(@"DisableUploads");
    NSMenuItem *toggleUploadsEnabled = [[NSMenuItem alloc] initWithTitle:toggleTitle action:@selector(toggleUploadsEnabled:) keyEquivalent:@""];    

    if (!progressItem)
    {
        progressItem = [[NSMenuItem alloc] initWithTitle:PROGRESS_MENU_ITEM_TEXT action:@selector(showUploadingStatusWindow) keyEquivalent:@""];
        [progressItem setImage:[NSImage imageNamed:@"icon_account_small.png"]];
    }

    if ([[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated])
    {
        [popUpMenu addItem:history];
        [self updateHistoryMenuImpl];
        [popUpMenu addItem:[[item0 copy] autorelease]]; // Sep
        [popUpMenu addItem:capture];
        [popUpMenu addItem:screenshot];
        [popUpMenu addItem:screenshot_full];
        [popUpMenu addItem:videoCaptureMenuItem];
        [popUpMenu addItem:uploadfile];
        [popUpMenu addItem:[[item0 copy] autorelease]]; // Sep
//        [popUpMenu addItem:account];
        [popUpMenu addItem:toggleUploadsEnabled];
        [popUpMenu addItem:preference];
    }
    else
    {
        [popUpMenu addItem:login];
    }

    [popUpMenu addItem:quit];

    safe_release(history);
    safe_release(quit);
    safe_release(capture);
    safe_release(screenshot);
    safe_release(screenshot_full);
    safe_release(uploadfile);
    safe_release(toggleUploadsEnabled);
            

    [[self window] registerForDraggedTypes:[self acceptableTypes]];

    [NSApp setDelegate:self];
}

- (NSArray *) acceptableTypes
{
    return [NSArray arrayWithObjects:NSStringPboardType, NSFilenamesPboardType, nil];
}

- (BOOL) application:(NSApplication *)sender openFile:(NSString *)filename
{
    if (filename && [[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated])
    {
        // send files to upload queue
        [[BoxNetHandler sharedHandler] uploadFiles:filename withProperties:nil];
        
        return YES;
    }
    return NO;
}

- (void) application:(NSApplication *)sender openFiles:(NSArray *)filenames
{
    if ([[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated])
    {
        // send files to upload queue
        [[BoxNetHandler sharedHandler] uploadFiles:filenames withProperties:nil];
    }
    else
    {
        // Please Authenticate
    }
}

#pragma mark D'n'd

- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender
{
    if ([[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated] && ![mainController disable_automatic_upload_check])
    {
		NSPasteboard *pboard = [sender draggingPasteboard];
		NSString *type = [pboard availableTypeFromArray:[self acceptableTypes]];
        
        if (type)
        {
            type = nil;
            return NSDragOperationCopy;
        }
    }
    
    return NSDragOperationNone;
}

- (void) draggingEnded:(id<NSDraggingInfo>)sender
{
    
}

- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender
{
    return YES;
}

- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender
{
    NSPasteboard* pboard = [sender draggingPasteboard];
	NSString *type = [pboard availableTypeFromArray:[self acceptableTypes]];
	BOOL loaded = NO;
    
    if (type) 
    {
		if ([type isEqualToString:NSFilenamesPboardType]) 
        {
			NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];

            if ([files count] == 0)
            {
                files = nil;
                goto exit;
            }
            
            loaded = YES;
			
            // send files to upload queue
            [[BoxNetHandler sharedHandler] uploadFiles:files withProperties:nil];
		}
    }
    
exit:
    
    pboard = nil;
    type = nil;
    
    return loaded;
}

#pragma mark File system notifications handler

- (void) fileSystemNotificationsHandler:(NSNotification *)notification
{
    if ([mainController disable_automatic_upload_check])  {
        return;
    }
    
    SSLog(@"UserInfo %@", [notification userInfo]);
    SSLog(@"Object %@", [[notification object] class]);
    
    // check if upload isn't disable
    // add code
    // if (![[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated]) return; // NO
    
   
    // Workaround for Lion. Screenshot preferences are stored in com.apple.screencapture.plist on pre Lion OS (Snow Leo, Leo...) 
    // and in SystemUIServer.app/Contents/Resources/English.lproj/ScreenCapture.strings on Lion
    BOOL isLionAndUpper = [Utilities isRunningOnLion];
        
	NSString *scprefspath = [NSHomeDirectory() stringByAppendingPathComponent:@"Library/Preferences/com.apple.screencapture.plist"];
    if (isLionAndUpper) {
        scprefspath = @"/System/Library/CoreServices/SystemUIServer.app/Contents/Resources/English.lproj/ScreenCapture.strings";
    }
    
	NSDictionary *scdict = [NSDictionary dictionaryWithContentsOfFile:scprefspath];
	
	// Get prefix
    
	NSString *prefix = @"Screen Shot";
    
    if (isLionAndUpper) {
        if (scdict && [scdict objectForKey:@"Screen Shot"]){
            prefix = [scdict objectForKey:@"Screen Shot"];
        }
    } else {
        if (scdict && [scdict objectForKey:@"name"]){
            prefix = [scdict objectForKey:@"name"];
        }        
    }
    prefix = [prefix lowercaseString];

	// Get path
	NSString *basepath = [NSHomeDirectory() stringByAppendingPathComponent:@"Desktop"];
	if (scdict && [scdict objectForKey:@"location"]) basepath = [scdict objectForKey:@"location"];
        
	// Iterate through all files at that path
	NSArray *files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:basepath error:nil];
	
	NSDateFormatter* dateFormatter = [[[NSDateFormatter  alloc] initWithDateFormat:@"%z" allowNaturalLanguage:NO] autorelease];
	NSString* tzString=[dateFormatter stringFromDate:[NSDate date]];
	SSLog(@"Time Zone is %@",tzString);
	
	for (NSString *fileName in files)
	{
		// if there's a file that matches the prefix
		if ([[fileName lowercaseString] hasPrefix:prefix])
		{
            NSString* originalFileName = [[fileName copy] autorelease];
            fileName = [fileName lowercaseString];
			// Extract the date string and use natural language matching to get its date
            NSString *datestring = [[fileName stringByDeletingPathExtension] stringByReplacingOccurrencesOfString:prefix withString:@""];
			datestring = [[[datestring stringByReplacingOccurrencesOfString:@" at" withString:@""] stringByReplacingOccurrencesOfString:@"." withString:@":"] stringByAppendingString:[NSString stringWithFormat:@" %@",tzString]];
			NSDate *picDate = [NSDate dateWithNaturalLanguageString:datestring];
			SSLog(@"Picture Date %@",picDate);
			// Determine the length of time since the screen was shot.
			NSTimeInterval t = [[NSDate date] timeIntervalSinceDate:picDate];
			
			// Proceed if the time interval lies within 10 seconds
			// New extra time is to allow for better screen shot layout
			NSLog(@"Time diff = %f",t);
			if (t < 10.0f)
			{
				// Get the full path and the actual image
				NSString *path = [basepath stringByAppendingPathComponent:originalFileName];
				SSLog(@"PATH >> %@",path);
				
                if ([filesAddedToQueue containsObject:path])
                    continue;
                
                if (![[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated])
                {
//                    if(![filenameSet containsObject:path])
//                    {
                        NSPasteboard *pBoard= [NSPasteboard generalPasteboard];
                        [pBoard clearContents];
                        NSData *data = [NSData dataWithContentsOfFile:path];
                        NSImage *image = [[[NSImage alloc] initWithData:data] autorelease];
                        
                        if (image)
                        {
                            BOOL ok = [pBoard writeObjects:[NSArray arrayWithObject:image]];
                            
                            if (ok)
                            {
                                [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Notification" withDescription:@"Screenshot copied to clipboard."];
                            }
                        }
//                    }
                }
                else
                {
                    NSLog(@"UPLOADING!!!!!!!");
                    [filesAddedToQueue addObject:path];
                    [[BoxNetHandler sharedHandler] uploadFiles:[NSArray arrayWithObject:path] withProperties:[NSDictionary dictionaryWithObject:@"SCREEN_SHOT" forKey:@"SCREEN_SHOT"]];
                }
            }
        }
	}
}

- (void) quitApplication
{
    [NSApp terminate:self];
}

- (void) showUploadingStatusWindow
{
    [[mainController progressIndicator] startAnimation:nil];
    [uploadStatusPanel setBecomesKeyOnlyIfNeeded:YES];
    [uploadStatusPanel setFloatingPanel:NO];
    [uploadStatusPanel makeKeyAndOrderFront:nil];
    
    NSScreen *mainScreen = [NSScreen mainScreen];
    
    [uploadStatusPanel setFrameOrigin:NSMakePoint(NSWidth(mainScreen.frame) - NSWidth(uploadStatusPanel.frame) - 6, NSHeight(mainScreen.frame) - NSHeight(uploadStatusPanel.frame) - 28)];
    
    mainScreen = nil;
    
    [NSApp activateIgnoringOtherApps:YES];
}

- (void) showMainView
{
    if ([[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated])
    {   
        [account setTarget:self];
        [account setAction:@selector(showMainView)];

        [preference setTarget:self];
        [preference setAction:@selector(showPreferenceView)];
        
        NSToolbarItem *item = [[toolBar items] objectAtIndex:1];
        
        [item setTarget:self];
        [item setAction:@selector(showMainView)];
        
        item = nil;
        item = [[toolBar items] objectAtIndex:2];
        
        [item setTarget:self];
        [item setAction:@selector(showPreferenceView)];
        
        item = nil;
    }
    
    [self selectToolBarItem:0];
    [self addAndResizeWindowForView:mainView];
}

- (void) showLoginView
{
    [self addAndResizeWindowForView:loginView];
}

- (void) showPreferenceView
{    
    [self selectToolBarItem:1];
    [self addAndResizeWindowForView:preferenceView];
}

- (void) addAndResizeWindowForView:(NSView *)view
{
    if ([[[[self window] contentView] subviews] count] > 0)
        [[[[[self window] contentView] subviews] objectAtIndex:0] removeFromSuperview];
    
    NSRect frame = [[self window] frame];
    frame.size.width = view.frame.size.width;
    frame.size.height = view.frame.size.height + 75;
    frame.origin.y = NSMaxY([[self window] frame]) - view.frame.size.height - 75;
    
    [[self window] setFrame:frame display:YES animate:YES];
    
    [self.window.contentView setFrame:[view frame]];
    [self.window.contentView addSubview:view];
    
    if (![[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated])
    {
        [self disableToolBarItems];
    }
    
    currentView = (CustomNSView *)view;
    
    if (![[self window] isVisible]) {
        [[self window] makeKeyAndOrderFront:nil];
    }
    [NSApp activateIgnoringOtherApps:YES];
}

- (void) showLoading:(NSNotification *)notification
{
    safe_release(loadingView);
    
    loadingView = [[LoadingView alloc] initWithFrame:[self.window.contentView frame]];
    
    [loadingView setText:notification.object];
    
    if (currentView) {
        currentView->enabled = NO;
    }
    
    [[[self window] contentView] addSubview:loadingView positioned:NSWindowAbove relativeTo:nil];
}

- (void) hideLoading
{
    if (currentView)
        currentView->enabled = YES;
    
    [loadingView removeFromSuperview];
    safe_release(loadingView);
}

- (void) selectToolBarItem:(int)index
{
    if (index == 0)
    {
        [toolBar setSelectedItemIdentifier:@"net.box.mainView"];
    }
    else
        if (index == 1)
        {
            [toolBar setSelectedItemIdentifier:@"net.box.preferenceView"];
        }
}

- (void) disableToolBarItems
{
    [toolBar setSelectedItemIdentifier:nil];
    for (id item in [toolBar items])
    {
        if ([item isKindOfClass:[NSToolbarItem class]])
        {
            [item setTarget:nil];
            [item setAction:nil];
        }
    }       
    
    [account setTarget:nil];
    [account setAction:nil];
    
//    [preference setTarget:nil];
//    [preference setAction:nil];
}
        
+ (BoxSimpleShareAppDelegate *) sharedDelegate
{
    return (BoxSimpleShareAppDelegate *)[NSApp delegate];
}

- (id) getIconForType:(NSString *)name
{
    name = [name pathExtension];
    
    if (!icons) 
    {
        icons = [NSMutableDictionary new];
        
        // ILLUSTRATOR
        [icons setValue:@"illustrator.png" forKey:@"ai"];
        // FLASH
        [icons setValue:@"flash.png" forKey:@"fla"];
        // PHOTOSHOP
        [icons setValue:@"photoshop.png" forKey:@"psd"];
        // WORD
        [icons setValue:@"word.png" forKey:@"doc"];
        [icons setValue:@"word.png" forKey:@"docx"];
        // POWERPOINT
        [icons setValue:@"powerpoint.png" forKey:@"ppt"];
        [icons setValue:@"powerpoint.png" forKey:@"pptx"];
        [icons setValue:@"powerpoint.png" forKey:@"pps"];
        // EXCEL
        [icons setValue:@"excel.png" forKey:@"xls"];
        [icons setValue:@"excel.png" forKey:@"csv"];
        [icons setValue:@"excel.png" forKey:@"xlsx"];
        // VIDEO
        [icons setValue:@"video.png" forKey:@"mov"];
        [icons setValue:@"video.png" forKey:@"m4v"];
        [icons setValue:@"video.png" forKey:@"mp4"];
        [icons setValue:@"video.png" forKey:@"flv"];
        [icons setValue:@"video.png" forKey:@"divx"];
        [icons setValue:@"video.png" forKey:@"avi"];
        [icons setValue:@"video.png" forKey:@"mpg"];
        [icons setValue:@"video.png" forKey:@"rm"];
        [icons setValue:@"video.png" forKey:@"swf"];
        [icons setValue:@"video.png" forKey:@"vob"];
        [icons setValue:@"video.png" forKey:@"wmv"];
        // AUDIO
        [icons setValue:@"audio.png" forKey:@"mp3"];
        [icons setValue:@"audio.png" forKey:@"m4a"];
        [icons setValue:@"audio.png" forKey:@"ape"];
        [icons setValue:@"audio.png" forKey:@"flac"];
        [icons setValue:@"audio.png" forKey:@"wav"];
        [icons setValue:@"audio.png" forKey:@"aac"];
        [icons setValue:@"audio.png" forKey:@"aif"];
        [icons setValue:@"audio.png" forKey:@"m3u"];
        [icons setValue:@"audio.png" forKey:@"mid"];
        [icons setValue:@"audio.png" forKey:@"mpa"];
        [icons setValue:@"audio.png" forKey:@"ra"];
        [icons setValue:@"audio.png" forKey:@"wma"];
        // WEB
        [icons setValue:@"web.png" forKey:@"php"];
        [icons setValue:@"web.png" forKey:@"html"];
        // COMPRESS
        [icons setValue:@"compress.png" forKey:@"zip"];
        [icons setValue:@"compress.png" forKey:@"tar"];
        [icons setValue:@"compress.png" forKey:@"rar"];
        [icons setValue:@"compress.png" forKey:@"dmg"];
        [icons setValue:@"compress.png" forKey:@"iso"];
        [icons setValue:@"compress.png" forKey:@"msi"];
        // PDF
        [icons setValue:@"pdf.png" forKey:@"pdf"];
        // IMAGE
        [icons setValue:@"image.png" forKey:@"png"];
        [icons setValue:@"image.png" forKey:@"gif"];
        [icons setValue:@"image.png" forKey:@"bmp"];
        [icons setValue:@"image.png" forKey:@"tiff"];
        [icons setValue:@"image.png" forKey:@"jpg"];
    }
    return ([icons containsKey:[name lowercaseString]] ? [icons valueForKey:[name lowercaseString]] : [NSString stringWithString:@"default.png"]);
}

#pragma mark Growl notifications

- (NSDictionary *) registrationDictionaryForGrowl
{
	return [NSDictionary dictionaryWithObjectsAndKeys:
            [NSArray arrayWithObjects:@"TestNotification", nil], GROWL_NOTIFICATIONS_ALL,
            [NSArray arrayWithObjects:@"TestNotification", nil], GROWL_NOTIFICATIONS_DEFAULT,
            nil];
}

// Growl Notification
+ (void) showNotificationWithTitle:(NSString *)title withDescription:(NSString *)description 
{    
    [GrowlApplicationBridge  notifyWithTitle:title
                                 description:description
                            notificationName:@"TestNotification"
                                    iconData:nil
                                    priority:1
                                    isSticky:NO
                                clickContext:nil];
}

- (IBAction) selectAndUploadFile:(id)sender
{
	int result;
    
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	[openPanel setTitle:@"Choose A File(s) To Upload"];
	[openPanel setCanChooseFiles:YES];
	[openPanel setCanChooseDirectories:NO];
	[openPanel setAllowsMultipleSelection:YES];
	[openPanel setResolvesAliases:YES];
	result = (int)[openPanel runModal];
    
	if (result == NSOKButton && [[openPanel filenames] count] > 0)
    {
		[[Mixpanel sharedInstance] trackUploadFileEvent];
        [[BoxNetHandler sharedHandler] uploadFiles:[openPanel filenames] withProperties:nil];
	}
}

- (IBAction)startVideoCapture:(id)sender {
    if ([videoCaptureController isCapturing]) {
        [videoCaptureController stopVideoRecording];
        [videoCaptureController hideAllWindows];
        [videoCaptureController clear];
        [videoCaptureMenuItem setTitle:InterfaceString(@"VideoCapture")];
    } else {
        if (!videoCaptureController) {
            videoCaptureController = [[StartVideoCaptureController alloc] init];
            videoCaptureController.delegate = self;
        }
        if ([[self window] isVisible]) {
            [[self window] close];
        }
//        NSWindow* win = [videoCaptureController window];
//        
//        [NSApp arrangeInFront:self];
//        [win makeKeyAndOrderFront:nil];
//        [win setLevel:NSScreenSaverWindowLevel];
//        [NSApp activateIgnoringOtherApps:YES];
        
        [videoCaptureController showWindows];
        
        [[NSCursor crosshairCursor] set];
        shouldSetCaptureCursor = YES;
        [videoCaptureMenuItem setTitle:InterfaceString(@"StopVideoCapture")];
    }
}

- (void)applicationWillBecomeActive:(NSNotification*)notification {
    if (shouldSetCaptureCursor) {
        [[NSCursor crosshairCursor] performSelectorOnMainThread:@selector(set) withObject:nil waitUntilDone:NO];
        shouldSetCaptureCursor = NO;
    }
}

- (void) modifierDepressMagic
{
	// for some unknown reason after keyboard event with modifiers flags is sent
	// it is required to unpress all modifiers key AFTER DELAY
	CGEventRef commandKey = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)kVK_Command, false);
	CGEventPost(kCGSessionEventTap, commandKey);
	CFRelease(commandKey);
	
	CGEventRef shiftKey = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)kVK_Shift, false);
	CGEventPost(kCGSessionEventTap, shiftKey);
	CFRelease(shiftKey);
}

- (IBAction) triggerRegionCapture:(id)sender
{
    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Capture Region" withDescription:@"Hold and drag to take screenshot"];

	[[Mixpanel sharedInstance] trackCaptureRegionEvent];
    
	// simulate cmd+shift+4 keypress
	// press
	CGEventRef xTheCommand = CGEventCreateKeyboardEvent(NULL, kVK_ANSI_Keypad4, true);
    CGEventSetFlags(xTheCommand, kCGEventFlagMaskShift|kCGEventFlagMaskCommand);
    CGEventPost(kCGSessionEventTap, xTheCommand);
    CFRelease(xTheCommand);
	// release
	xTheCommand = CGEventCreateKeyboardEvent(NULL, kVK_ANSI_Keypad4, false);
    CGEventSetFlags(xTheCommand, kCGEventFlagMaskShift|kCGEventFlagMaskCommand);
    CGEventPost(kCGSessionEventTap, xTheCommand);
    CFRelease(xTheCommand);
    
	// release modifiers
	[self performSelector:@selector(modifierDepressMagic) withObject:nil afterDelay:0.5f];
}

- (IBAction) triggerFullScreenCapture:(id)sender
{
    //[BoxSimpleShareAppDelegate showNotificationWithTitle:@"Capture Fullscreen" withDescription:@"Taken fullscreen screenshot"];

	[[Mixpanel sharedInstance] trackCaptureFullScreenEvent];
    
	// press
    CGEventRef xTheCommand = CGEventCreateKeyboardEvent(NULL, kVK_ANSI_Keypad3, true);
    CGEventSetFlags(xTheCommand, kCGEventFlagMaskShift|kCGEventFlagMaskCommand);
    CGEventPost(kCGHIDEventTap, xTheCommand);
    CFRelease(xTheCommand);
	// release
	xTheCommand = CGEventCreateKeyboardEvent(NULL, kVK_ANSI_Keypad3, false);
    CGEventSetFlags(xTheCommand, kCGEventFlagMaskShift|kCGEventFlagMaskCommand);
    CGEventPost(kCGHIDEventTap, xTheCommand);
    CFRelease(xTheCommand);
    
	// release modifiers
	[self performSelector:@selector(modifierDepressMagic) withObject:nil afterDelay:0.5f];
}

- (void)toggleUploadsEnabled:(id)sender {
    [mainController toggleUploadsEnabled];
}

- (void) dealloc
{
    safe_release(statusBarItem);
    safe_release(statusBarImages);
    
    [popUpMenu removeAllItems];
    safe_release(popUpMenu);
    
    safe_release(progressItem);
    safe_release(account);
    safe_release(preference);
    safe_release(videoCaptureMenuItem)
    
    
    safe_release(filesUploadedInSession);
    
    safe_release(filesAddedToQueue);
    
    [icons release];
    
    [[self window] unregisterDraggedTypes];
    
    RemoveNotificationObserver(self);
    
    [super dealloc];
}

#pragma mark -

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
    if ([keyPath isEqualToString:@"disable_automatic_upload_check"]) {
        [self createMenu];
        
        BOOL enabled = ![mainController disable_automatic_upload_check];
        if (enabled) {
            
        } else {
            
        }
    } else if ([keyPath isEqualToString:@"upload_hot_key"]) {
        [self uninstallUploadHotKey];
        [self setupUploadHotKey];
    } else if ([keyPath isEqualToString:@"screen_cast_hot_key"]) {
        [self uninstallVideoCaptureHotKey];
        [self setupVideoCaptureHotKey];
    } else if ([keyPath isEqualToString:@"launch_at_startup_check"]) {
        BOOL launchOnStart = [mainController launch_at_startup_check];
        if (launchOnStart) {
            [Utilities addAppAsStartupItem]; 
        } else {
            [Utilities deleteAppFromLoginItem];
        }
    }
}

#pragma mark -

- (void)videoCaptureController:(id)controller didCaptureVideoToPath:(NSString*)path {
    if (controller == videoCaptureController && path) {
		[[Mixpanel sharedInstance] trackVideoCaptureEvent];
        [[BoxNetHandler sharedHandler] uploadFiles:[NSArray arrayWithObject:path] withProperties:nil];
    }
}

- (void)videoCaptureControllerDidFailedStartVideoCapture:(StartVideoCaptureController*)controller {
    if (controller == videoCaptureController) {
        [videoCaptureMenuItem setTitle:InterfaceString(@"VideoCapture")];
    }
}

- (void)videoCaptureControllerDidStartVideoCapture:(StartVideoCaptureController*)controller {
    if (controller == videoCaptureController) {
        [BoxSimpleShareAppDelegate showNotificationWithTitle:InterfaceString(@"StartVideoCaptureTitle") 
                                             withDescription:[NSString stringWithFormat:InterfaceString(@"StartVideoCaptureDescription"), [mainController screen_cast_hot_key]]];
    }
}

- (void)videoCapturControlllerDidPressStopCapture:(StartVideoCaptureController*)controller {
    if (controller == videoCaptureController) {
        [self startVideoCapture:nil];
    }
}

@end

// Handler for global hot key events
static OSStatus HotKeyHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData) {
    BoxSimpleShareAppDelegate* appDelegate = (BoxSimpleShareAppDelegate*)inUserData;
    
    EventHotKeyID hotKeyId;
    GetEventParameter(inEvent, kEventParamDirectObject, typeEventHotKeyID, NULL, sizeof(hotKeyId), NULL, &hotKeyId);
    
    if (hotKeyId.id == VideoCaptureHotKeyEventId) {
        [appDelegate startVideoCapture:nil];
    } else if (hotKeyId.id == UploadHotKeyEventId) {
        if ([[appDelegate mainController] disable_automatic_upload_check]) {
            return noErr;
        }
        BOOL showOpenWindow = NO;
        FinderApplication *theFinder = [SBApplication applicationWithBundleIdentifier: @"com.apple.finder"];
        if ([theFinder frontmost]) {
//            NSLog(@"FINDER ON TOP - TRY TO PROCESS SELECTION");
            
            SBObject* obj = [theFinder selection];
            SBElementArray* selectedObjects = [obj get];
            if ([selectedObjects count]) {
                NSMutableArray* uploadPaths = [NSMutableArray array];
                for (FinderItem* item in selectedObjects) {
                    NSString* fileId = [item URL]; // Acrually returns file id int form file:///.file/id=6571367.8124611
                    NSURL* url = [NSURL URLWithString:fileId]; // convert it to absoulute path
                    [uploadPaths addObject:[url path]];
                }
                [[BoxNetHandler sharedHandler] uploadFiles:uploadPaths withProperties:nil];
            } else {
                showOpenWindow = YES;
            }
            
        } else {
            showOpenWindow = YES;
        }
        
        if (showOpenWindow) {
             BoxSimpleShareAppDelegate* delegate = (BoxSimpleShareAppDelegate*)[NSApp delegate];
             [delegate selectAndUploadFile:nil];
        }
    }
    return noErr;
}


