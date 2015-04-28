//
//  ItemCell.m
//  LiquidFiles
//
//  Created by Cuongnq on 11/24/13.
//  Copyright (c) 2013 Cuongnq. All rights reserved.
//

#import "ItemCell.h"
#import "PXListView.h"
#import "PXListView+Private.h"
#import <iso646.h>
#import "UploadOperation.h"
#import "GDataXMLNode.h"
#import "BoxFile.h"
#import "BoxSimpleShareAppDelegate.h"
@implementation ItemCell


- (void)prepareForReuse
{
	[titleDesc setStringValue:@""];
    [timeStr setStringValue:@""];
    [image setImage:nil];
   // self.operation.delegate = nil;
    
}

#pragma mark -

-(void)awakeFromNib {
    
    [self createTrackingArea];
    
    [self setTitleWidth:YES];

}

- (void) createTrackingArea
{
    int opts = (NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved|NSTrackingActiveAlways);
   NSTrackingArea* trackingArea = [ [NSTrackingArea alloc] initWithRect:[self bounds]
                                                 options:opts
                                                   owner:self
                                                userInfo:nil];
    [self addTrackingArea:trackingArea];
}

#pragma mark Drawing

- (void)drawRect:(NSRect)dirtyRect
{
    NSRect rect = [self bounds];

	if(isActive) {

        [[NSColor
         colorWithCalibratedRed:239.0 / 0xff
         green:(CGFloat)246.0 / 0xff
         blue:(CGFloat)252.0 / 0xff
         alpha:1.0] set];
	}
    else {
        [[NSColor highlightColor] set];
    }
	NSRectFill(rect);
    
    if(self.row < [ self.listView  numberOfRows] -1){
        NSBezierPath *path = [NSBezierPath bezierPath];
        
        [path moveToPoint:NSMakePoint(NSMinX(rect) + 1, NSMinY(rect))];
        [path lineToPoint:NSMakePoint(NSMaxX(rect) -1, NSMinY(rect))];
        [path closePath];
        
        [[NSColor secondarySelectedControlColor] set];
        [path stroke];
        
    }
        
    
}

-(void) loadData:(BoxFile *)file{
    

    NSString* itemTitle = [file fileName];
    
    [titleDesc setStringValue:itemTitle];
    
//    NSMenuItem *tmp = [[NSMenuItem alloc] initWithTitle:itemTitle action:@selector(openBoxNetFile:) keyEquivalent:@""];
    [image setImage:[NSImage imageNamed:[[BoxSimpleShareAppDelegate sharedDelegate] getIconForType :[file fileName]]]];
//    [tmp setTag:i];
//    [popUpMenu addItem:tmp];
//    [tmp release];
//    ++i;
//    if (i == MaxHistoryItemCount) {
//        break;
//    }

}




//-(IBAction)copyFileLink:(id)sender {
//    if (self.operation.fileLink)
//    {
//
//        [[NSPasteboard generalPasteboard] clearContents];
//        [[NSPasteboard generalPasteboard] setString:self.operation.fileLink forType:NSStringPboardType];
//
//    }
//    else if (self.operation.operationStatus == FAILED)
//    {
//        [self.delegate performSelector:@selector(removeItem:) withObject:[ NSNumber numberWithInteger:self.row]];
//    }
//}

-(void)mouseEntered:(NSEvent *)theEvent {
    
//    isActive = YES;
//    if (self.operation.operationStatus == COMPLETED) {
//        [actionButton setHidden:NO];
//        [actionButton setTitle:@"Copy link"];
//        [fileLinkActionButton setHidden:NO];
//    }else  if (self.operation.operationStatus == FAILED || self.operation.operationStatus == CANCELED) {
//        
//        [actionButton setHidden:NO];
//        [fileLinkActionButton setHidden:YES];
//        [actionButton setTitle:@"Remove"];
//
//    }
//    
//    [self setTitleWidth:NO];
//
//    [self setNeedsDisplay:YES];
}

-(void) mouseExited:(NSEvent *)theEvent {
    
//    isActive = NO;
//     [actionButton setHidden:YES];
//    [fileLinkActionButton setHidden:YES];
//    
//    [self setTitleWidth:YES];
//    
//    [self setNeedsDisplay:YES];
    
}


- (void)viewDidMoveToWindow
{
	[super viewDidMoveToWindow];
	
	[self addToolTipRect:[self bounds] owner:self userData:NULL];
    
}

//- (NSString *)view:(NSView *)view stringForToolTip:(NSToolTipTag)tag point:(NSPoint)point userData:(void *)userData
//{
//	NSLog( @"provide tip %@", NSStringFromPoint( point ) );
//    return self.operation.uploadFileName;
//}


-(void)setTitleWidth:(BOOL)isLongTitle
{
    int width = isLongTitle ? 260 : 175;
    NSRect rect = titleDesc.frame;
    rect.size.width = width;
    [titleDesc setFrame:rect];

}


@end
