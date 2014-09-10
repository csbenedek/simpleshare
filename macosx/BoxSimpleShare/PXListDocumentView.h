

#import <Cocoa/Cocoa.h>
#import "PXListViewDropHighlight.h"


@class PXListView;

@interface PXListDocumentView : NSView
{
	PXListView				*_listView;
	PXListViewDropHighlight	_dropHighlight;
}

@property (assign) PXListView				*listView;
@property (nonatomic, assign) PXListViewDropHighlight	dropHighlight;

-(void)	setDropHighlight: (PXListViewDropHighlight)inState;

@end
