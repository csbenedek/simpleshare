//
//  ItemCell.h
//  LiquidFiles
//
//  Created by Cuongnq on 11/24/13.
//  Copyright (c) 2013 Cuongnq. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PXListViewCell.h"

@class BoxFile;
@class KBButton;
@class LBProgressBar;
@interface ItemCell : PXListViewCell
{
    
    BOOL isActive;
    IBOutlet NSImageView* image;
    IBOutlet NSTextField* titleDesc;
    IBOutlet NSTextField* timeStr;
    
    
}

@property (nonatomic, unsafe_unretained) BoxFile* boxFile;
@property (unsafe_unretained) id delegate;

-(void) loadData:(BoxFile *)file;

@end
