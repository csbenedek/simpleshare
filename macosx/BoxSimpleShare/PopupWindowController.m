//
//  PopupWindowController.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 6/30/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "PopupWindowController.h"
#import "BackgroundView.h"
#import "PopupLoginViewController.h"
#import "LoadingView.h"
#import "StatusItemView.h"
#import "MenubarController.h"
#import "PopupMainViewController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "BoxFile.h"
#import "AppConstants.h"

@interface PopupWindowController ()

@end

@implementation PopupWindowController

//@synthesize backgroudView;
@synthesize delegate = _delegate;
- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    [backgroundView setBackgroundColor:[NSColor whiteColor]];
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

-(void)awakeFromNib
{
    NSPanel *panel = (id)[self window];
    [panel setAcceptsMouseMovedEvents:YES];
    [panel setLevel:NSFloatingWindowLevel];
    [panel setOpaque:NO];
    [panel setBackgroundColor:[NSColor clearColor]];
    [backgroundView setBackgroundColor:[NSColor whiteColor]];
    
    
    
    AddNotificationObserver(self, @selector(showLoading:), @"SHOW_LOADING", nil);
    AddNotificationObserver(self, @selector(hideLoading), @"HIDE_LOADING", nil);

}

-(void)showLogin
{
       loginViewController = [PopupLoginViewController shareController];
        NSRect rect = self.window.frame;
    
        CGFloat loginViewHeight = loginViewController.view.bounds.size.height;
    
       float exchange = loginViewHeight - backgroundView.bounds.size.height;
    
        rect.size.height += exchange;
    rect.size.width = loginViewController.view.bounds.size.width;
        rect.origin.y -= exchange;
    [[self.window animator] setFrame:rect display:NO];

       [backgroundView addSubview:loginViewController.view];
}
- (void) showLoading:(NSNotification *)notification
{


    safe_release(loadingView);
    
    NSRect rect = [self.window.contentView frame];
    rect.size.height -=25;
    loadingView = [[LoadingView alloc] initWithFrame:rect];
    
    [loadingView setText:notification.object];
    [backgroundView setBackgroundColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:.7]];
    
    [[[self window] contentView] addSubview:loadingView positioned:NSWindowAbove relativeTo:nil];
}

- (void) hideLoading
{

    
    [loadingView removeFromSuperview];
    [backgroundView setBackgroundColor:[NSColor whiteColor]];
    
    safe_release(loadingView);

}

- (id)initWithDelegate:(id<PopupControllerDelegate>)delegate
{
    self = [self initWithWindowNibName:@"PopupWindowController"];
    if (self != nil)
    {
        _delegate = delegate;
    }
    return self;
}


#pragma mainViewController

-(void)updateWindowSize
{
//    NSInteger itemCount = self.main.itemSource.count > 3 ? 3: self.taskViewController.itemSource.count;
    NSInteger height = [self.mainViewController getSizeOfItem];
 
      float maximumHeight =  [NSScreen mainScreen].frame.size.height / 2;
      if (height > maximumHeight) {
          height = maximumHeight;
      }

    //float height = self.taskViewController.view.frame.size.height;
    float exchange = height - backgroundView.bounds.size.height;
    
    NSRect rect = self.window.frame;
    rect.size.height += exchange;
    rect.size.width = self.mainViewController.view.bounds.size.width;
    rect.origin.y -= exchange;
    [self.window  setFrame:rect display:NO];
    
    if(self.mainViewController.view.superview != backgroundView)
    {
        [backgroundView addSubview:self.mainViewController.view];
    }
    [self.mainViewController reloadData];
    
}

-(PopupMainViewController* )mainViewController
{
    if (_mainViewController == nil) {
        _mainViewController = [[PopupMainViewController alloc] init];
        [_mainViewController loadView];
    }
    
    return _mainViewController;
}
#pragma mark - Public accessors

- (BOOL)hasActivePanel
{
    return _hasActivePanel;
}

- (void)setHasActivePanel:(BOOL)flag
{
    if (_hasActivePanel != flag)
    {
        _hasActivePanel = flag;
        
        if (_hasActivePanel)
        {
            [self openPanel];
        }
        else
        {
            [self closePanel];
        }
    }
}

#pragma mark - NSWindowDelegate

- (void)windowWillClose:(NSNotification *)notification
{
    self.hasActivePanel = NO;
}



- (void)windowDidResize:(NSNotification *)notification
{
    
    NSWindow *panel = [self window];
    NSRect statusRect = [self statusRectForWindow:panel];
    NSRect panelRect = [panel frame];
    
    CGFloat statusX = roundf(NSMidX(statusRect));
    CGFloat panelX = statusX - NSMinX(panelRect);
    
    backgroundView.arrowX = panelX;
    
    
}



#pragma mark - Keyboard

- (void)cancelOperation:(id)sender
{
    self.hasActivePanel = NO;
}



#pragma mark - Public methods

- (NSRect)statusRectForWindow:(NSWindow *)window
{
    NSRect screenRect = [[[NSScreen screens] objectAtIndex:0] frame];
    NSRect statusRect = NSZeroRect;
    
    StatusItemView *statusItemView = nil;
    if ([self.delegate respondsToSelector:@selector(statusItemViewForPanelController:)])
    {
        statusItemView = [self.delegate statusItemViewForPanelController:self];
    }
    
    if (statusItemView)
    {
        statusRect = statusItemView.globalRect;
        statusRect.origin.y = NSMinY(statusRect) - NSHeight(statusRect);
    }
    else
    {
        statusRect.size = NSMakeSize(STATUS_ITEM_VIEW_WIDTH, [[NSStatusBar systemStatusBar] thickness]);
        statusRect.origin.x = roundf((NSWidth(screenRect) - NSWidth(statusRect)) / 2);
        statusRect.origin.y = NSHeight(screenRect) - NSHeight(statusRect) * 2;
    }
    return statusRect;
}

- (void)openPanel
{
    NSWindow *panel = [self window];
    [panel setLevel:NSFloatingWindowLevel];
    NSRect panelRect = [panel frame];
        NSRect screenRect = [[[NSScreen screens] objectAtIndex:0] frame];
        NSRect statusRect = [self statusRectForWindow:panel];
        
        
        panelRect.origin.x = roundf(NSMidX(statusRect) - NSWidth(panelRect) / 2);
        panelRect.origin.y = NSMaxY(statusRect) - NSHeight(panelRect);
        
        if (NSMaxX(panelRect) > (NSMaxX(screenRect) - ARROW_HEIGHT))
            panelRect.origin.x -= NSMaxX(panelRect) - (NSMaxX(screenRect) - ARROW_HEIGHT);
        
        
//    }
    [NSApp activateIgnoringOtherApps:NO];
    [panel setAlphaValue:0];
    [panel setFrame:panelRect display:YES];
    [self windowDidResize: nil];
    [panel setAlphaValue:1];
    [panel makeKeyAndOrderFront:nil];
    
    
}

- (void)closePanel
{
    [self.window orderOut:nil];
}

-(void)loadMainView
{
    NSRect rect = self.mainViewController.view.bounds;
    NSRect windowFrame = self.window.frame;
    windowFrame.size.width = rect.size.width;
    windowFrame.size.height = rect.size.height;
    
    [self.window setFrame:windowFrame display:NO] ;
    [self windowDidResize:nil];
    
    
}

- (void)updateHistoryMenuImpl
{
    int i = 0;
    NSMutableArray* filesUploadedInSession = [[BoxSimpleShareAppDelegate sharedDelegate] filesUploadedInSession];
    
    NSArray* itemList =  [[[filesUploadedInSession reverseObjectEnumerator] allObjects] subarrayWithRange:NSMakeRange(0, MAX_HISTORY_ITEM_COUNT)];

    
//    for (BoxFile *file in [filesUploadedInSession reverseObjectEnumerator])
//    {
//        NSString* itemTitle = [file fileName];
//        //        if ([itemTitle length] > MaxHistoryItemLength) {
//        //            itemTitle = [itemTitle substringToIndex:MaxHistoryItemLength];
//        //            itemTitle = [itemTitle stringByAppendingFormat:@"..."];
//        //        }
//        NSMenuItem *tmp = [[NSMenuItem alloc] initWithTitle:itemTitle action:@selector(openBoxNetFile:) keyEquivalent:@""];
//        [tmp setImage:[NSImage imageNamed:[self getIconForType:[file fileName]]]];
//        [tmp setTag:i];
//        [popUpMenu addItem:tmp];
//        [tmp release];
//        ++i;
//        if (i == MaxHistoryItemCount) {
//            break;
//        }
//    }
}

-(void)loadUserData
{
    
    [loginViewController.view removeFromSuperview];
    //[self.mainViewController prepareData];
    [self updateWindowSize];

}

@end
