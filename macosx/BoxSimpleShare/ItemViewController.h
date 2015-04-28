//
//  ItemViewController.h
//  LiquidFiles
//
//  Created by Cuongnq on 11/24/13.
//  Copyright (c) 2013 Cuongnq. All rights reserved.
//



#import <Cocoa/Cocoa.h>
#import "PXListView.h"


@class OperationQueue;
@class PanelController;
@interface ItemViewController : NSViewController<PXListViewDelegate>
{
    IBOutlet PXListView* listView;
}

@property (nonatomic, strong) NSArray* itemSource;
@property (nonatomic, unsafe_unretained) PanelController* delegate;
-(NSUInteger)sizeOfItems;
-(void)reloadData;
@end
