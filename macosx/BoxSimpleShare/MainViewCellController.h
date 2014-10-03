//
//  MainViewCellController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 12/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//
@class BoxNetFileInfoLoader;
@class BoxFile;
@class YouTubeThumbnailLoader;
@class ImgurThumbnailLoader;




#import <Cocoa/Cocoa.h>

@interface MainViewCellController : NSViewController

@property(nonatomic, retain) IBOutlet NSTextField *fileName;

@property(nonatomic, retain) IBOutlet NSTextField *uploadDateTextField;

@property(nonatomic, retain) IBOutlet NSImageView *imageView;

@property(nonatomic , retain) IBOutlet NSProgressIndicator *spinner;

@property(nonatomic, retain) BoxNetFileInfoLoader *fileInfoLoader;

@property(nonatomic, retain) YouTubeThumbnailLoader *youTubeThumbnailLoader;

@property(nonatomic, retain) ImgurThumbnailLoader *imgurThumbnailLoader;


@property(nonatomic, retain) BoxFile *boxFile;








-(void)processThumbnailInfoLoadedNotification:(NSNotification *)notification;

-(void)processYouTubeThumbnailLoadedNotification:(NSNotification *)notification;

-(void)processImgurThumbnailLoadedNotification:(NSNotification *)notification;



-(void)openSharedLink;





@end
