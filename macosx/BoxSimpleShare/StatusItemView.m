//
//  StatusItemView.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/15/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "StatusItemView.h"

#import "BoxSimpleShareAppDelegate.h"
#import "MainController.h"
#import "HTTPRequestHandler.h"
#import "Extended.h"
#import "YoutubeUploadOperation.h"

static const int ShortStatusItemLength = 24;
static const int LongStatusItemLength = 65;

typedef enum {
    
    StatusItemStateIconOnly,
    StatusItemStateIconAndText
    
} StatusItemState;

@interface StatusItemLayerDelegate : NSObject {

    NSImage* image;
}

- (void)setImage:(NSImage*)image;

@end

@interface StatusItemView (Private)

- (void) updateFrame:(int)state;
- (void) loadView;

- (void) setNotificationsObserver;
- (void) removeNotificationObserver;

- (void)installImage;

@end

@implementation StatusItemView

@synthesize statusItem;
@synthesize isHighlighted = _isHighlighted;
@synthesize action = _action;
@synthesize target = _target;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) 
    {
		currentProgressImageIndex = 0;
    }
    
    return self;
}

- (id) initWithStatusItem:(NSStatusItem *)item
{
    if ((self = [super init]))
    {
        statusItem = item;
        [self setNotificationsObserver];
        
        //NSImage *image = [NSImage imageNamed:@"default-icon.png"];
        
        //[item setTitle:@"Test"];
        
        //[item setImage:image];
        
        
        [self loadView];
        [self updateFrame:StatusItemStateIconOnly];
    }
    return self;
}

#pragma mark
#pragma mark Load View

- (void) updateFrame:(StatusItemState)state
{
    switch (state) 
    {
        case StatusItemStateIconOnly:
            [statusItem setLength:ShortStatusItemLength];
            break;
            
        case StatusItemStateIconAndText:
            [statusItem setLength:LongStatusItemLength];
            break;
            
        default:
            break;
    }
}

- (void)setFrame:(NSRect)frameRect {
    [rootLayer setFrame:NSRectToCGRect(frameRect)];
    [rootLayer setNeedsDisplay];
    [super setFrame:frameRect];
}

- (void) loadView
{
    layerDelegate = [[StatusItemLayerDelegate alloc] init];
    
    rootLayer = [[CALayer layer] retain];
    [self setLayer:rootLayer];
    [self setWantsLayer:YES];

    imageLayer = [CALayer layer];
    imageLayer.frame = CGRectMake(4, 3, 16, 16);
    [rootLayer addSublayer:imageLayer];
    [imageLayer setDelegate:layerDelegate];
    [self installImage];
        
    statusText = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 0, 45, 20)];
    [statusText setEditable:NO];
    [statusText setEnabled:YES];
    [statusText setTextColor:[NSColor blackColor]];
    [statusText setBackgroundColor:[NSColor clearColor]];
    [statusText setFont:[NSFont fontWithName:@"Helvetica" size:13]];
    [statusText setBordered:NO];
    [statusText setAlignment:NSCenterTextAlignment];
    
    [[[BoxSimpleShareAppDelegate sharedDelegate] mainController] addObserver:self
                                                                  forKeyPath:@"disable_automatic_upload_check"
                                                                     options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld 
                                                                     context:NULL];
    
    [self addSubview:statusText];
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Drawing code here.
    
    if (mouseDown)
    {
        [[NSColor selectedMenuItemColor] set];
        NSRectFill(dirtyRect);
        
        [statusText setTextColor:[NSColor whiteColor]];
    }
    else
    {
        [statusText setTextColor:[NSColor blackColor]];
    }
    
    [super drawRect:dirtyRect];
}

#pragma mark
#pragma mark Status Update Notfication Handler

- (void) setTitleForStatus:(NSString *)status
{
    [statusText setStringValue:status];
}

- (void) setImageForStatus:(NSImage *)img
{
    [layerDelegate setImage:img];
    [imageLayer setNeedsDisplay];
    
    //[self.statusItem setImage:img];
    
}

- (void)setImageIndex:(int)idx 
{
    
    
    if (currentProgressImageIndex != idx) {
        NSArray *images = [[BoxSimpleShareAppDelegate sharedDelegate] statusBarImages];
        if (currentProgressImageIndex + 1 != idx && idx > currentProgressImageIndex) {
            if (!imageQueue) {
                imageQueue = [[NSMutableArray alloc] init];
            }
            if (![imageQueue containsObject:[NSNumber numberWithInt:idx]]) {
                for (int i = currentProgressImageIndex + 1; i <= idx; i++) {
                    [imageQueue addObject:[NSNumber numberWithInt:i]];
                }
                [self performSelector:@selector(processNextImage) withObject:nil afterDelay:0.05];
            }
        } else {
            [imageQueue removeAllObjects];
            currentProgressImageIndex = idx;
            [self setImageForStatus:[images objectAtIndex:currentProgressImageIndex]];
        }
    }
}

- (void)processNextImage {
    if ([imageQueue count]) {
        NSNumber* imageIndex = [imageQueue objectAtIndex:0];
        currentProgressImageIndex = [imageIndex intValue];
        NSArray *images = [[BoxSimpleShareAppDelegate sharedDelegate] statusBarImages];
        NSImage *image = [images objectAtIndex:currentProgressImageIndex];
        
        [self setImageForStatus:image];
        [imageQueue removeObjectAtIndex:0];
        [self performSelector:@selector(processNextImage) withObject:nil afterDelay:0.05];
    }
}

- (void) setNotificationsObserver
{
    AddNotificationObserver(self, @selector(updateStatusItem:), UPDATE_STATUS_ITEM, nil);
    [self registerForDraggedTypes:[[BoxSimpleShareAppDelegate sharedDelegate] acceptableTypes]];
}

- (void) removeNotificationObserver
{
    RemoveNotificationObserver(self);
    [self unregisterDraggedTypes];
}

- (void) updateStatusItem:(NSNotification *)notification
{
    @try {
        
        if([[notification object] isKindOfClass:[YoutubeUploadOperation class]])
        {
            id handler = notification.object;
            if ([handler bytesUploadedSoFar] > 0 && [handler totalBytesToUpload] > (5 * 1024 * 1024)) // Should be greater than 5 mb to show the percentage in status bar
            {
                double progress = (double)[handler bytesUploadedSoFar] / (double)[handler totalBytesToUpload];
                [self setTitleForStatus:[NSString stringWithFormat:@"%.0f%%", (100 * progress)]];
                [self updateFrame:StatusItemStateIconAndText];
            }
            else
            {
                [self setTitleForStatus:@""];
                [self updateFrame:StatusItemStateIconOnly];
            }
            
            // STATUS IMAGES
            if ([handler totalBytesToUpload] > 0)
            {
                NSArray *images = [[BoxSimpleShareAppDelegate sharedDelegate] statusBarImages];
                
                int index = (int)((float)[handler bytesUploadedSoFar] * ((float)([images count] - 1) / (float)[handler totalBytesToUpload]));
                
                // Have to change active icon to the first in animation set.
                if (index == 0)
                {
                    index = 1;
                }
                else if ([[[BoxSimpleShareAppDelegate sharedDelegate] statusBarImages] count] == index + 1)
                {
                    --index;
                }
                
                if (currentProgressImageIndex > index)
                {
                    index = currentProgressImageIndex;
                }
                
                if (!mouseDown)
                {
                    [self setImageIndex:index];
                    
                }
            }

        }
        
        else if ([[notification object] isKindOfClass:[HTTPRequestHandler class]])
        {
            HTTPRequestHandler *handler = [notification object];
            
            if ([handler isUploadQueue])
            {

                if ([handler bytesUploadedSoFar] > 0 && [handler totalBytesToUpload] > (5 * 1024 * 1024)) // Should be greater than 5 mb to show the percentage in status bar
                {
                    double progress = (double)[handler bytesUploadedSoFar] / (double)[handler totalBytesToUpload];
                    [self setTitleForStatus:[NSString stringWithFormat:@"%.0f%%", (100 * progress)]];
                    [self updateFrame:StatusItemStateIconAndText];
                }
                else
                {
                    [self setTitleForStatus:@""];
                    [self updateFrame:StatusItemStateIconOnly];
                }
                
                // STATUS IMAGES
                if ([handler totalBytesToUpload] > 0)
                {
                    NSArray *images = [[BoxSimpleShareAppDelegate sharedDelegate] statusBarImages];
                    
                    int index = (int)((float)[handler bytesUploadedSoFar] * ((float)([images count] - 1) / (float)[handler totalBytesToUpload]));
					
					// Have to change active icon to the first in animation set.
					if (index == 0)
					{
						index = 1;
					}
					else if ([[[BoxSimpleShareAppDelegate sharedDelegate] statusBarImages] count] == index + 1)
					{
						--index;
					}
					
					if (currentProgressImageIndex > index)
					{
						index = currentProgressImageIndex;
					}
					
					if (!mouseDown)
					{
						[self setImageIndex:index];
					}
                }
            }
        }
        else
        {
            if ([[notification object] isKindOfClass:[NSString class]])
            {
                if ([[notification object] isEqualToString:UPLOAD_FINISHED] || [[notification object] isEqualToString:@"LOGOUT"])
                {
                    NSArray *images = [[BoxSimpleShareAppDelegate sharedDelegate] statusBarImages];
                    [self setTitleForStatus:@""];
					
					if (!mouseDown)
					{
                      
					 	if ([imageQueue count] == 0) {
							[self setImageIndex:0];
                            [self setNeedsDisplay:YES];
						} else {
							[self performSelector:@selector(setupInitialImage) withObject:nil afterDelay:0.1];
						}
						images = nil;
					}
                    
                    [self updateFrame:StatusItemStateIconOnly];

					currentProgressImageIndex = 0;					
                }
            }
        }
    }
    @catch (NSException *e)
    {
        DbgLog(@"CLASS: %@   METHOD: %@   EXCEPTION: %@", NSStringFromClass([self class]), NSStringFromSelector(_cmd), e);
    }
}

- (void)setupInitialImage {
    if ([imageQueue count] == 0) {
        [self setImageIndex:0];
    } else {
        [self performSelector:@selector(setupInitialImage) withObject:nil afterDelay:0.1];
    }
}

#pragma mark -

- (void)installImage {
    BOOL enabled = ![[[BoxSimpleShareAppDelegate sharedDelegate] mainController] disable_automatic_upload_check];
    if (enabled) {
        [self setImageForStatus:[NSImage imageNamed:@"default-icon"]];
    } else {
		[self setImageForStatus:[NSImage imageNamed:@"nointernet-icon"]];
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
    if ([keyPath isEqualToString:@"disable_automatic_upload_check"]) {
        [self installImage];
    }
}

#pragma mark
#pragma mark Drag n Drop Handlers

- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender
{
    return [[BoxSimpleShareAppDelegate sharedDelegate] draggingEntered:sender];
}

- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender
{
    return [[BoxSimpleShareAppDelegate sharedDelegate] performDragOperation:sender];
}

- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender
{
    return [[BoxSimpleShareAppDelegate sharedDelegate] prepareForDragOperation:sender];
}



#pragma mark -

- (void)setMouseDown:(BOOL)down {
    if (mouseDown != down) {
        mouseDown = down;
        [self setNeedsDisplay:YES];
        rootLayer.backgroundColor = mouseDown ? [[NSColor blueColor] CGColor] : [[NSColor clearColor] CGColor];
        [rootLayer setNeedsDisplay];
        statusText.textColor = mouseDown ? [NSColor whiteColor] : [NSColor blackColor];

		if (down)
		{
			[self setImageForStatus:[NSImage imageNamed:@"active-icon"]];
		}
		else
		{
//			[self setImageForStatus:[NSImage imageNamed:@"default-icon.png"]];

			NSArray *images = [[BoxSimpleShareAppDelegate sharedDelegate] statusBarImages];
            if(currentProgressImageIndex > images.count -1)
            {
                currentProgressImageIndex = images.count -1;
            }
			[self setImageForStatus:[images objectAtIndex:currentProgressImageIndex]];
		}
	}
}

#pragma mark
#pragma mark Mouse Events

- (void) mouseDown:(NSEvent *)theEvent
{
    [self setMouseDown:YES];
    
    [self postStatusItemClickedNotification];
    
    
     //[NSApp sendAction:self.action to:self.target from:self];
//    [[statusItem menu] setDelegate:self];
//    [statusItem popUpStatusItemMenu:[statusItem menu]];
}

- (void) mouseUp:(NSEvent *)event
{
    [self setMouseDown:NO];
}

- (void)rightMouseDown:(NSEvent *)event
{
	//[self mouseDown:event];
    
    NSLog(@"Right mouse down");
    
    [[statusItem menu] setDelegate:self];
    [statusItem popUpStatusItemMenu:[statusItem menu]];
}

- (void)rightMouseUp:(NSEvent *)event
{
	//[self mouseUp:event];
}

//- (void) menuDidClose:(NSMenu *)menu
//{
//    [self setMouseDown:NO];
//    [menu setDelegate:nil];
//}
//
//- (void) menuWillOpen:(NSMenu *)menu
//{
//    [self setNeedsDisplay:YES];
//}
#pragma mark Accessors

-(BOOL)isHighlighted {
    return _isHighlighted;
}
- (void)setHighlighted:(BOOL)newFlag
{
    //if (mouseDown == newFlag) return;
    mouseDown = newFlag;
    _isHighlighted = newFlag;
    [self setNeedsDisplay:YES];
}
#pragma mark 
- (NSRect)globalRect
{
    NSRect frame = [self frame];
    frame.origin = [self.window convertBaseToScreen:frame.origin];
    return frame;
}

-(void)postStatusItemClickedNotification{
    
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"StatusItemClickedNotification" object:self];
    
    //see processStatusItemClickedNotificetion of AttachedWindowsController
    
}


#pragma mark Memory Management

- (void) dealloc
{
    [self removeNotificationObserver];
    statusItem = nil;
    [layerDelegate release];
    [imageQueue release];
    [super dealloc];
}

@end


@implementation StatusItemLayerDelegate

- (void)setImage:(NSImage*)newImage {
    if (image != newImage) {
        [image release];
        image = [newImage retain];
    }
}

- (void)drawLayer:(CALayer*)layer inContext:(CGContextRef)ctx {
    NSGraphicsContext* nsGraphicsContext = [NSGraphicsContext
                                            graphicsContextWithGraphicsPort:ctx
                                            flipped:NO];
    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:nsGraphicsContext];
    
    [image drawAtPoint:NSZeroPoint 
              fromRect:NSMakeRect(0, 0, image.size.width, image.size.height) 
             operation:NSCompositeSourceOver 
              fraction:1.0];
    
    [NSGraphicsContext restoreGraphicsState];
}
#pragma mark -



@end
