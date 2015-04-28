//
//  SHTableView.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 27/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol ExtendedTableViewDelegate <NSObject>

- (void)tableView:(NSTableView *)tableView didClickedRow:(NSInteger)row;

@end

@interface SHTableView : NSTableView

@property (nonatomic, retain) id<ExtendedTableViewDelegate> extendedDelegate;

@end
