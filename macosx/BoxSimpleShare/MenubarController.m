#import "MenubarController.h"
#import "StatusItemView.h"

@implementation MenubarController

@synthesize statusItemView = _statusItemView;

#pragma mark -

- (id)init
{
    self = [super init];
    if (self != nil)
    {
        // Install status item into the menu bar
        NSStatusItem *statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:STATUS_ITEM_VIEW_WIDTH] retain];
        [statusItem setHighlightMode:YES];
        NSImage* img =  [NSImage imageNamed:@"menubar"];
        [img setSize:NSMakeSize(18, 18)];
        _statusItemView = [[StatusItemView alloc] initWithStatusItem:statusItem];
        [_statusItemView setFrame:NSMakeRect(0, 0, 25, 50)];
        
        [statusItem setView:_statusItemView];
        
        //[view release];
        //_statusItemView.image =  img ;//[NSImage imageNamed:@"menubar"];
       // _statusItemView.alternateImage = img;// [NSImage imageNamed:@"menubar"];
        _statusItemView.action = @selector(togglePanel:);
        [_statusItemView setDelegate:self]; // =@selector(rightClickMenu:);
    }
    return self;
}

- (void)dealloc
{
    [[NSStatusBar systemStatusBar] removeStatusItem:self.statusItem];
}

-(void) showContextMenu :(NSMenu*) menu {
    //[_contextMenu setDelegate:self];
   
    [self.statusItem popUpStatusItemMenu:menu];
}
#pragma mark -
#pragma mark Public accessors

- (NSStatusItem *)statusItem
{
    return self.statusItemView.statusItem;
}

#pragma mark -

- (BOOL)hasActiveIcon
{
    return self.statusItemView.isHighlighted;
}

- (void)setHasActiveIcon:(BOOL)flag
{
    self.statusItemView.isHighlighted = flag;
}

-(void) setStatus:(NSString*)status {
    
//    NSString* image = nil;
//    if([status rangeOfString:@"no_"].location == NSNotFound) {
//      
//        image = [status isEqualToString :@"notification_update" ] ? @"red":
//        [status isEqualToString:@"inbox_update"] ? @"green" :@"blue";
//            
//        _lastStatus = status;
//       
//    }
//    else {
//        NSString* crop = [status stringByReplacingOccurrencesOfString:@"no_" withString:@""];
//        if ([_lastStatus isEqualToString:crop]) {
//            _lastStatus = nil;
//            image = @"black";
//        }
//    }
//    if (image) {
//        self.statusItemView.image = [NSImage imageNamed: image] ;
//        self.statusItemView.alternateImage = [NSImage imageNamed: image];
//    }
}


@end
