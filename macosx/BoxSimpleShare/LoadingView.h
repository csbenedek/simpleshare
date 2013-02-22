//
//  LoadingView.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/9/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LoadingView : NSView {
    
@public
    
    BOOL enabled;
    
@private
    
    NSProgressIndicator *indicator;
    NSTextField *text;
}

- (void) setText:(NSString *)string;

@end
