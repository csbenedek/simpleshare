//
//  PopupMainViewController.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 8/27/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "PopupMainViewController.h"
#import "ItemViewController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "BoxFile.h"
#import "HeaderView.h"
#import "FooterView.h"
#import "MainController.h"
@interface PopupMainViewController ()

@end

@implementation PopupMainViewController

@synthesize itemViewController = _itemViewController;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    return self;
}


-(id)init {
    
    if(self = [super initWithNibName:NSStringFromClass([self class]) bundle:nil])
    {
       
    }
    return self;
}

-(void)awakeFromNib
{
    self.itemViewController = [[ItemViewController alloc] initWithNibName:@"ItemViewController" bundle:nil];
    [self.itemViewController loadView];
    [self prepareData];
}

-(void) loadNoItemView
{
    NSRect rect     = self.view.bounds;
    rect.size.height = headerView.bounds.size.height + footerView.bounds.size.height + noItemView.bounds.size.height;
    [self.view setFrame:rect];
    
    if (noItemView.superview != contentView ) {
        
        [contentView addSubview:noItemView];

    }
    [_itemViewController.view setHidden:YES];
    [noItemView setHidden:NO];
}

-(void) loadItemView {
    
    
    
    NSUInteger itemHeight = [_itemViewController sizeOfItems] ;
    
    float maximumHeight =  [NSScreen mainScreen].frame.size.height / 2;
    if (itemHeight > maximumHeight) {
        itemHeight = maximumHeight;
    }


    NSRect rect = self.view.frame;
    rect.size.height = headerView.bounds.size.height + footerView.bounds.size.height + itemHeight;
    [self.view setFrame:rect ];
    if (_itemViewController.view.superview != contentView) {
        
        NSRect itemRect =  _itemViewController.view.bounds;
        itemRect.size.width = contentView.bounds.size.width;
        itemRect.size.height = contentView.bounds.size.height;
        
        [_itemViewController.view setFrame:itemRect];
        [contentView addSubview:_itemViewController.view];
    }

    
}

- (void)updateHistoryMenuImpl
{
    //int i = 0;
    NSMutableArray* filesUploadedInSession = [[BoxSimpleShareAppDelegate sharedDelegate] filesUploadedInSession];
    
    self.itemViewController.itemSource = filesUploadedInSession;
}

-(CGFloat)getSizeOfItem
{
    
    NSUInteger itemHeight = [_itemViewController sizeOfItems] ;

    return  headerView.bounds.size.height + footerView.bounds.size.height + itemHeight;
}
-(void)prepareData
{
    [self updateHistoryMenuImpl];
    
    if (self.itemViewController.itemSource.count == 0) {
        
        [self loadNoItemView];
        
    }
    else
    {
        [self loadItemView];
        
    }
    
}

-(void)reloadData
{
    [self.itemViewController reloadData];
}

-(IBAction)quitSimpleShare:(id)sender
{
    [NSApp terminate:self];
}
-(IBAction)showPreferences:(id)sender
{
    [[BoxSimpleShareAppDelegate sharedDelegate].window makeKeyAndOrderFront:nil];

}

@end
