//
//  HeaderView.h
//  LiquidFiles
//
//  Created by Cuongnq on 11/21/13.
//  Copyright (c) 2013 Cuongnq. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HeaderView : NSView
{
    IBOutlet NSTextField* serverUrl;
    IBOutlet NSImageView* icon;
    
    NSString* _url;
 
}



@end
