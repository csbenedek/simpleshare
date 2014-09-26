//
//  MainViewCellController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 12/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//
@class BoxNetFileInfoLoader;


#import <Cocoa/Cocoa.h>

@interface MainViewCellController : NSViewController

@property(nonatomic, retain) IBOutlet NSTextField *fileName;

@property(nonatomic, retain) IBOutlet NSTextField *shareDate;

@property(nonatomic, retain) IBOutlet NSImageView *imageView;

@property(nonatomic, retain) BoxNetFileInfoLoader *fileInfoLoader;






-(void)processThumbnailInfoLoadedNotification:(NSNotification *)notification;




@end
