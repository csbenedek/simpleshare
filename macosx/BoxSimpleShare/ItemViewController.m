
//  ItemViewController.m
//  LiquidFiles
//
//  Created by Cuongnq on 11/24/13.
//  Copyright (c) 2013 Cuongnq. All rights reserved.
//

#import "ItemViewController.h"
#import "PXListView.h"
#import "ItemCell.h"

#define LISTVIEW_CELL_IDENTIFIER		@"ItemCellIdentifier"
#define NUM_EXAMPLE_ITEMS                 10
@interface ItemViewController ()

@end

@implementation ItemViewController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    return self;
}


-(void) awakeFromNib {
    
    
    [listView setCellSpacing:0.0f];
    [listView setAllowsEmptySelection :NO];
    [listView setAllowsMultipleSelection: NO];
    self.itemSource = [[NSMutableArray alloc] init];

}

- (NSUInteger)numberOfRowsInListView: (PXListView*)aListView
{
#pragma unused(aListView)
	return self.itemSource.count;
}

- (PXListViewCell*)listView:(PXListView*)aListView cellForRow:(NSUInteger)row
{
    BoxFile* file = [self.itemSource objectAtIndex:row];
	ItemCell *cell = (ItemCell*)[aListView dequeueCellWithReusableIdentifier:LISTVIEW_CELL_IDENTIFIER];
	
	if(!cell) {
		cell = [ItemCell cellLoadedFromNibNamed:@"ItemCell" reusableIdentifier:LISTVIEW_CELL_IDENTIFIER];
       // [cell setDelegate:self];
	}
	
    [cell setDelegate:self];
    [cell loadData:file];
	return cell;
}

- (CGFloat)listView:(PXListView*)aListView heightOfRow:(NSUInteger)row
{
    return 58.0;
}

- (void)listViewSelectionDidChange:(NSNotification*)aNotification
{
    // NSLog(@"Selection changed");
}

-(NSUInteger) sizeOfItems {
    
    return self.itemSource.count * 58.0;
}

-(void)reloadData
{
    [listView reloadData];
}

-(void)dealloc
{
    [super dealloc];
}
@end
