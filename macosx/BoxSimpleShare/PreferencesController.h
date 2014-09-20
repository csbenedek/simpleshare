//
//  PreferencesController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 19/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class MainPreferencesViewController;
@class AccountViewController;
@class ShortcutsViewController;


@interface PreferencesController : NSWindowController

//toolbar
@property(nonatomic, retain) IBOutlet  NSToolbar *toolbar;



//view controllers;

@property (nonatomic, retain) MainPreferencesViewController *mainPreferencesController;

@property (nonatomic, retain) AccountViewController *accountViewController;

@property (nonatomic, retain) ShortcutsViewController *shortcutsViewController;





//currently displayed view

@property (nonatomic ,retain) NSView *activeView;



//toolbar items actions

-(IBAction)mainPreferencesToolbarItemAction:(id)sender;

-(IBAction)accountToolbarItemAction:(id)sender;

-(IBAction)shortcutsToolbarItemAction:(id)sender;

-(void)displayView:(NSView *) view;


-(void)processShowPreferencesWindowNotification;


@end
