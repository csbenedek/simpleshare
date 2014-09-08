//
//  FooterView.h
//  LiquidFiles
//
//  Created by Cuongnq on 11/22/13.
//  Copyright (c) 2013 Cuongnq. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FooterView : NSView
{
    
    NSColor* startingColor;
    NSColor* endingColor;
}

@property (strong) IBOutlet NSImageView* image;
-(void) startAnimation ;
@end
