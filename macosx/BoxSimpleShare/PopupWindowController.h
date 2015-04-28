//
//  PopupWindowController.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 6/30/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class BackgroundView;
@class LoadingView;
@class PopupWindowController;
@class StatusItemView;
@class PopupLoginViewController;
@class PopupMainViewController;
@protocol PopupControllerDelegate <NSObject>

@optional

- (StatusItemView *)statusItemViewForPanelController:(PopupWindowController *)controller;

@end
@interface PopupWindowController : NSWindowController
{
    
    IBOutlet BackgroundView* backgroundView;
    LoadingView* loadingView;
    BOOL _hasActivePanel;
    id<PopupControllerDelegate>_delegate;
    PopupLoginViewController* loginViewController ;
    PopupMainViewController* _mainViewController;
}

@property (nonatomic) BOOL hasActivePanel;
@property (nonatomic, unsafe_unretained, readonly) id<PopupControllerDelegate> delegate;
@property (nonatomic, readonly, getter = mainViewController) PopupMainViewController* mainViewController;
- (id)initWithDelegate:(id<PopupControllerDelegate>)delegate;
-(void) loadUserData;
-(void)showLogin;
@end
