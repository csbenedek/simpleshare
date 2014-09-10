


enum
{
	PXIsDragStartMouseReleased = 0,
	PXIsDragStartTimedOut,
	PXIsDragStartMouseMovedHorizontally,
	PXIsDragStartMouseMovedVertically
};
typedef NSInteger PXIsDragStartResult;


@interface PXListView ()

- (NSRect)contentViewRect;

- (void)cacheCellLayout;
- (void)layoutCells;
- (void)layoutCell:(PXListViewCell*)cell atRow:(NSUInteger)row;

- (void)addCellsFromVisibleRange;
- (PXListViewCell*)visibleCellForRow:(NSUInteger)row;
- (NSArray*)visibleCellsForRowIndexes:(NSIndexSet*)rows;

- (NSUInteger)numberOfRows;
- (void)deselectRowIndexes:(NSIndexSet*)rows;
- (void)postSelectionDidChangeNotification;

- (void)updateCells;
- (void)enqueueCell:(PXListViewCell*)cell;

- (void)contentViewBoundsDidChange:(NSNotification*)notification;
-(void)layoutCellsForResizeEvent;

@end
