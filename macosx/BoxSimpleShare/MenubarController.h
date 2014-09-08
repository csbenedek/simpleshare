#define STATUS_ITEM_VIEW_WIDTH 24.0

#pragma mark -


#import "StatusItemView.h"

@interface MenubarController : NSObject {
@private
    StatusItemView *_statusItemView;
    
    NSString* _lastStatus;
}

@property (nonatomic) BOOL hasActiveIcon;
@property (nonatomic, strong, readonly) NSStatusItem *statusItem;
@property (nonatomic, strong, readonly) StatusItemView *statusItemView;

-(void) showContextMenu :(NSMenu*) menu;
-(void) setStatus:(NSString*) satus;
@end
