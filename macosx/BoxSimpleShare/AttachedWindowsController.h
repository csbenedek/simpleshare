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



@interface AttachedWindowsController : NSObject <NSWindowDelegate>



//currently displayed attached window

@property (nonatomic, assign) MAAttachedWindow *currentAttachedWindow;

@property (nonatomic, retain) MAAttachedWindow *textMessageWindow;

@property (nonatomic, retain) MAAttachedWindow *loginWindow;

@property (nonatomic, retain) MAAttachedWindow *mainWindow;

@property (nonatomic, retain) MAAttachedWindow *videoCaptureWindow;







//attached windows controllers

@property (nonatomic, retain) mainView* mainViewController;

@property (nonatomic, retain) VideoCaptureController *videoCaptureController;

@property (nonatomic, retain) TextMessageViewController *textMessageController;










//-(void)displayMessageWithText:(NSString *)text andWidth:(NSInteger) width;
-(void)hideTextMessageWindow;


-(void)processShowTextMessageNotification:(NSNotification *)notification;

-(void)processShowStartMessageNotification:(NSNotification *)notification;

-(void)processShowSuccessfulLoginMessageNotification:(NSNotification *)notification;

-(void)processNewHistoryElementNotification:(NSNotification *)notification;


-(void)displayMainWindow;
-(void)displayLoginWindow;
-(void)processShowLoginWindowNotification:(NSNotification *)notification;
-(void)displayVideoCaptureWindow;




-(void)processStatusItemClickedNotification;


//helpers

-(NSPoint)getLocationOfStatusItem;



-(void)hideAttachedWindow:(MAAttachedWindow *)attachedWindow;

-(void)showAttachedWindow:(MAAttachedWindow *)attachedWindow;

-(void)switchAttachedWindow:(MAAttachedWindow *)attachedWindow;

-(void)configureAttachedWindow:(MAAttachedWindow *)attachedWindow;





@end
