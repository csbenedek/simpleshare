//
//  BoxSimpleShareAppDelegate.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <Carbon/Carbon.h>
#import "CustomNSView.h"
#import "StatusItemView.h"
#import "LoadingView.h"
#import "StartVideoCaptureController.h"
#import "ImageCaptureController.h"
#import "Growl.h"
#import "FeedbackWindowController.h"

#import "MAAttachedWindow.h"


extern const int MaxHistoryItemCount;

@class MainController;
@class PopupWindowController;
@class MenubarController;

@class AttachedWindowsController;
@class PreferencesController;


@interface BoxSimpleShareAppDelegate : NSObject <NSApplicationDelegate, NSTabViewDelegate, NSTableViewDataSource, NSWindowDelegate, NSToolbarDelegate, GrowlApplicationBridgeDelegate, VideoCaptureControllerDelegate,ImageCaptureControllerDelegate> {

@private
    NSWindow *window;
    
    MainController *mainController;
    
    NSToolbar *toolBar;
    
    CustomNSView *loginView;
    NSView *mainView;
    NSView *preferenceView;
    NSView* shorcutView;
    LoadingView *loadingView;
    
    CustomNSView *currentView;
    
    NSPanel *uploadStatusPanel;
    
    // STATUS BAR
    
    NSStatusItem *statusBarItem;
    NSMutableArray *statusBarImages;
    NSMenu *popUpMenu;
    
    NSMenuItem *progressItem;
    NSMenuItem *account;
    NSMenuItem *preference;
    NSMenuItem *login;
    NSMenuItem *videoCaptureMenuItem;
    
    // OTHER
    
    NSMutableArray *filesUploadedInSession; // loads 10 recent files from last session (if they exists) and will hold all files from current one
    
    NSMutableSet *filesAddedToQueue;
    NSMutableDictionary *icons;
    
    EventHotKeyRef uploadFileHotkeyRef;
    EventHotKeyRef videoCaptureHotkeyRef;
    
    // video capture
    
    StartVideoCaptureController* videoCaptureController;
    ImageCaptureController     * imageCaptureController;
    BOOL shouldSetCaptureCursor;
    
    FeedbackWindowController* feedbackWindowController;
    
    PopupWindowController* _popupWindowController;
    
    MenubarController* _menubarController;
@public
    NSTabView *tableView;
    
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSTabView *tableView;

@property (retain, nonatomic) IBOutlet MainController *mainController;

@property (retain, nonatomic) IBOutlet NSToolbar *toolBar;
@property (retain, nonatomic) IBOutlet CustomNSView *loginView;
@property (retain, nonatomic) IBOutlet NSView *mainView;
@property (retain, nonatomic) IBOutlet NSView *preferenceView;
@property (retain, nonatomic) IBOutlet NSView *shortcutView;

@property (retain, nonatomic) IBOutlet NSPanel *uploadStatusPanel;

@property (retain, nonatomic) NSMutableArray *statusBarImages;
@property (retain, nonatomic) NSMenuItem *progressItem;

@property (retain, nonatomic) NSMutableArray *filesUploadedInSession;

@property (nonatomic, readonly) EventHotKeyRef uploadFileHotkeyRef;
@property (nonatomic, readonly) EventHotKeyRef videoCaptureHotkeyRef;

@property (nonatomic, retain) PopupWindowController* popupWindowController;
@property (nonatomic, strong) MenubarController *menubarController;

//a controller to manage attached window

@property (nonatomic, retain) AttachedWindowsController *attachedWindowsController;

//preferences controller
@property (nonatomic, retain) PreferencesController* preferencesController;





//new helpers

-(NSRect)getStatusItemRectInMainScreen;

-(void)loadPreferencesController;


+ (BoxSimpleShareAppDelegate *) sharedDelegate;
- (void) addAndResizeWindowForView:(NSView *)view;

- (void) showLoading:(NSNotification *)notification;
- (void) hideLoading;

- (void) showMainView;
- (void) showLoginView;

- (void) selectToolBarItem:(int)index;
- (void) disableToolBarItems;

- (void) updateHistoryMenu;
- (void) createMenu;
- (id) getIconForType:(NSString *)name;

- (void) showUploadingStatusWindow;

- (NSArray *) acceptableTypes;

+ (void) showNotificationWithTitle:(NSString *)title withDescription:(NSString *)description;

- (IBAction) selectAndUploadFile:(id)sender;

-(IBAction)startImageCapture:(id)sender;

-(NSWindow*)statusBarWindow;

-(void)quitApplication;

-(IBAction)startVideoCapture:(id)sender;

- (void)showPreferenceView;


-(void)openBoxNetFile:(id)sender;

-(void)sendFeedback:(id)sender;


@end
