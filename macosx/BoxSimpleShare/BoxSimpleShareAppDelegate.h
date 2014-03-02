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
extern const int MaxHistoryItemCount;

@class MainController;

@interface BoxSimpleShareAppDelegate : NSObject <NSApplicationDelegate, NSTabViewDelegate, NSTableViewDataSource, NSWindowDelegate, NSToolbarDelegate, GrowlApplicationBridgeDelegate, VideoCaptureControllerDelegate,ImageCaptureControllerDelegate> {

@private
    NSWindow *window;
    
    MainController *mainController;
    
    NSToolbar *toolBar;
    
    CustomNSView *loginView;
    NSView *mainView;
    NSView *preferenceView;
    
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
@property (retain, nonatomic) IBOutlet NSPanel *uploadStatusPanel;

@property (retain, nonatomic) NSMutableArray *statusBarImages;
@property (retain, nonatomic) NSMenuItem *progressItem;

@property (retain, nonatomic) NSMutableArray *filesUploadedInSession;

@property (nonatomic, readonly) EventHotKeyRef uploadFileHotkeyRef;
@property (nonatomic, readonly) EventHotKeyRef videoCaptureHotkeyRef;

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
@end
