//
//  NSImageVIewDragAnDrop.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 5/12/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "NSImageVIewDragAnDrop.h"

@implementation NSImageVIewDragAnDrop


-(void)awakeFromNib {
    
    [self registerForDraggedTypes:[self acceptableTypes]];
    
}
- (NSArray *) acceptableTypes
{
    return [NSArray arrayWithObjects:NSStringPboardType, NSFilenamesPboardType, nil];
}

- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender
{
    NSPasteboard *pboard = [sender draggingPasteboard];
    NSString *type = [pboard availableTypeFromArray:[self acceptableTypes]];
    
    if (type)
    {
        type = nil;
        return NSDragOperationCopy;
    }
    return NSDragOperationNone;
}

- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender
{
    return YES;
}

- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender
{
    //return [[BoxSimpleShareAppDelegate sharedDelegate] prepareForDragOperation:sender];
    NSPasteboard* pboard = [sender draggingPasteboard];
	NSString *type = [pboard availableTypeFromArray:[self acceptableTypes]];
	BOOL loaded = NO;
    
    if (type)
    {
		if ([type isEqualToString:NSFilenamesPboardType])
        {
			NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
            
            if ([files count] == 0)
            {
                files = nil;
                goto exit;
            }
            
            loaded = YES;
            [self.window.windowController performSelector:@selector(uploadFiles:) withObject:files];
			//[contentTitle setStringValue:[files objectAtIndex:0]];
            // send files to upload queue
            //[[BoxNetHandler sharedHandler] uploadFiles:files withProperties:nil];
		}
    }
    
exit:
    
    pboard = nil;
    type = nil;
    
    return loaded;
    
}

-(void)mouseDown:(NSEvent *)theEvent
{
    [self.window.windowController performSelector:@selector(uploadFiles:) withObject:nil];
}

@end
