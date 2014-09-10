//
//  PopupMainViewController.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 8/27/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ItemViewController;
@class HeaderView;
@class FooterView;
@interface PopupMainViewController : NSViewController
{

    IBOutlet NSView* noItemView;
    IBOutlet NSView* contentView;
    IBOutlet HeaderView* headerView;
    IBOutlet FooterView* footerView;
    ItemViewController* _itemViewController;
}

@property(nonatomic, retain) ItemViewController* itemViewController;
-(void)prepareData;
-(CGFloat)getSizeOfItem;
-(void)reloadData;

-(IBAction)quitSimpleShare:(id)sender;
-(IBAction)showPreferences:(id)sender;
@end
