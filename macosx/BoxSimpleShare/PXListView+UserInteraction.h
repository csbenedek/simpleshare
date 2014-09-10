

#import <Foundation/Foundation.h>

#import "PXListView.h"

@interface PXListView (UserInteraction)

- (void)handleMouseDown:(NSEvent*)theEvent inCell:(PXListViewCell*)theCell;
- (void)handleMouseDownOutsideCells:(NSEvent*)theEvent;

- (BOOL)attemptDragWithMouseDown:(NSEvent*)theEvent inCell:(PXListViewCell*)theCell;
- (NSImage*)dragImageForRowsWithIndexes:(NSIndexSet *)dragRows
                                  event:(NSEvent*)dragEvent clickedCell:(PXListViewCell*)clickedCell
                                 offset:(NSPointPointer)dragImageOffset;
- (void)setShowsDropHighlight:(BOOL)inState;
- (void)setDropRow:(NSUInteger)row dropHighlight:(PXListViewDropHighlight)dropHighlight;

@end
