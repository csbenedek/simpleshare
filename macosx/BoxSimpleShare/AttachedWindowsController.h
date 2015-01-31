//
//  AttachedWindowsController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 05/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "MAAttachedWindow.h"

@class mainView;
@class VideoCaptureController;
@class TextMessageViewController;
@class LoginViewController;
@class MAAttachedWindowController;



@interface AttachedWindowsController : NSObject <NSWindowDelegate>


-(id)initWithSettings:(NSDictionary *)settingsDict;




//isFirstLauhcn control variable

@property(nonatomic, assign) BOOL isFirstLaunch;
@property(nonatomic, assign) BOOL isStartUpMessageVisible;
@property(nonatomic, assign) BOOL isSuccessfulLoginMessageVisible;



@property (nonatomic, retain) MAAttachedWindow *videoCaptureWindow;







//attached windows controllers

@property(nonatomic, retain) MAAttachedWindowController *activeController;

@property (nonatomic, retain) mainView* mainViewController;

@property (nonatomic, retain) VideoCaptureController *videoCaptureController;

@property (nonatomic, retain) TextMessageViewController *textMessageController;

@property (nonatomic, retain) LoginViewController *loginWindowController;










//-(void)displayMessageWithText:(NSString *)text andWidth:(NSInteger) width;
-(void)hideTextMessageWindow;



-(void)processShowTextMessageNotification:(NSNotification *)notification;

-(void)processShowStartMessageNotification:(NSNotification *)notification;

-(void)processSuccessfulLoginNotification:(NSNotification *)notification;

-(void)processNewHistoryElementNotification:(NSNotification *)notification;

-(void)processShowLoginWindowNotification:(NSNotification *)notification;

-(void)processStatusItemClickedNotification;


-(void)displayMainWindow;
-(void)displayLoginWindow;

-(void)displayVideoCaptureWindow;







//helpers

-(NSPoint)getLocationOfStatusItem;

-(void)setActiveWindowController:(MAAttachedWindowController *)contrtoller;





-(void)hideAttachedWindow:(MAAttachedWindow *)attachedWindow;

-(void)showAttachedWindow:(MAAttachedWindow *)attachedWindow;

-(void)switchAttachedWindow:(MAAttachedWindow *)attachedWindow;

-(void)configureAttachedWindow:(MAAttachedWindow *)attachedWindow;





@end
