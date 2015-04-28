//
//  SHTableView.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 27/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "SHTableView.h"

@implementation SHTableView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    
    // Drawing code here.
}


- (void)mouseDown:(NSEvent *)theEvent {
    
    NSPoint globalLocation = [theEvent locationInWindow];
    NSPoint localLocation = [self convertPoint:globalLocation fromView:nil];
    NSInteger clickedRow = [self rowAtPoint:localLocation];
    
    [super mouseDown:theEvent];
    
    if (clickedRow != -1) {
        [self.extendedDelegate tableView:self didClickedRow:clickedRow];
    }
}

@end
