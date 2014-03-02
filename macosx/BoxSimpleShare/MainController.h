//
//  MainController.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/8/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CustomNSImageView.h"

@interface MainController : NSObject <NSTableViewDelegate, NSTableViewDataSource> {
    
@private
    
    NSWindow *window;
    
    // Main View
    
    NSTextField *loginName;
    NSTextField *loginID;
    CustomNSImageView *loginUserImage;
    NSButton *logoutBtn;
    NSButton *goToUploadFolderBtn;
    NSTextField *verField1;
    NSTextField *storageStatus;
    
    IBOutlet NSTextField *registerForFreeLabel;
    IBOutlet NSTextField *learnMoreLabel;
    
    // Login View
    
    NSTextField *userEmail;
    NSTextField *userPassword;
    NSTextField *errorMsg;
    NSButton *loginBtn;
    NSTextField *verField2;
    
    // Status Window
    
    NSProgressIndicator *progressIndicator;
    
    NSTextField *statusLine1;
    NSTextField *statusLine2;

    NSTextField *msg;
    
    NSButton *switchViews;
    
    NSTableView *requestsTable;
    NSScrollView *requestsTableScrollView;
    
    // Preference Window
    
    BOOL delete_screenshot_after_upload_check;
    BOOL delete_all_after_upload_check;
    BOOL disable_automatic_upload_check;
    BOOL copy_url_to_clipboard_check;
    BOOL shorten_links_check;
    BOOL launch_at_startup_check;
    BOOL mute_audio_check;

    int screencast_format_index;
    int uploadHost_index;
    NSString *screen_cast_hot_key;
    NSString *upload_hot_key;
    
    NSComboBox *screencast_format;
    
    NSTimer *prefSyncTimer;
    BOOL prefChanged;
    NSComboBox *imageHost;
    //
    
//    LoginHelper *loginHelper;
}

@property (retain, nonatomic) IBOutlet NSWindow *window;

// MAIN VIEW

@property (retain, nonatomic) IBOutlet NSTextField *loginName;
@property (retain, nonatomic) IBOutlet NSTextField *loginID;
@property (retain, nonatomic) IBOutlet CustomNSImageView *loginUserImage;
@property (retain, nonatomic) IBOutlet NSButton *logoutBtn;
@property (retain, nonatomic) IBOutlet NSButton *goToUploadFolderBtn;
@property (retain, nonatomic) IBOutlet NSTextField *verField1;
@property (retain, nonatomic) IBOutlet NSTextField *storageStatus;

// LOGIN VIEW

@property (retain, nonatomic) IBOutlet NSButton *loginBtn;
@property (retain, nonatomic) IBOutlet NSTextField *verField2;

// STATUS WINDOW

@property (retain, nonatomic) IBOutlet NSProgressIndicator *progressIndicator;

@property (retain, nonatomic) IBOutlet NSTextField *statusLine1;
@property (retain, nonatomic) IBOutlet NSTextField *statusLine2;

@property (retain, nonatomic) IBOutlet NSTextField *msg;

@property (retain, nonatomic) IBOutlet NSButton *switchViews;

@property (retain, nonatomic) IBOutlet NSTableView *requestsTable;
@property (retain, nonatomic) IBOutlet NSScrollView *requestsTableScrollView;

// PREFERENCE VIEW

@property (nonatomic) BOOL delete_screenshot_after_upload_check;
@property (nonatomic) BOOL delete_all_after_upload_check;
@property (nonatomic) BOOL disable_automatic_upload_check;
@property (nonatomic) BOOL copy_url_to_clipboard_check;
@property (nonatomic) BOOL shorten_links_check;
@property (nonatomic) BOOL launch_at_startup_check;
@property (nonatomic) BOOL mute_audio_check;

@property (nonatomic) int uploadhost_index;

@property (nonatomic) int screencast_format_index;
@property (nonatomic, retain) NSString *screen_cast_hot_key;
@property (nonatomic, retain) NSString *upload_hot_key;

@property (nonatomic, retain) IBOutlet NSComboBox *screencast_format;
@property (nonatomic, retain) IBOutlet NSComboBox *imageHost;

// OTHER

- (NSString*)videoFormatExtension;

- (void) setAppVersion;
- (void) wireAllButtons;
- (void) updateMainViewContent:(id)notification;

- (void)toggleUploadsEnabled;

- (IBAction) preferencePageValueChangeNotification:(id)sender;

@end
