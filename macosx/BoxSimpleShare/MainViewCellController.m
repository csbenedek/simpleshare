//
//  MainViewCellController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 12/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "MainViewCellController.h"
#import "BoxNetFileInfoLoader.h"
#import "BoxFile.h"

@interface MainViewCellController ()

@end

@implementation MainViewCellController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
        self.fileInfoLoader = [[BoxNetFileInfoLoader alloc] init];
        
        //register self for notifications from fileInfoLoader
        
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(processThumbnailInfoLoadedNotification:) name:@"ThumbnailLoadedNotification" object:self.fileInfoLoader];
        
        
        
    }
    return self;
}


-(void)processThumbnailInfoLoadedNotification:(NSNotification *) notification{
    
    NSLog(@"ProcessThumbnailInfoLoadedNotification");
    
    //thumbnail image loaded
    
    
    if (self.fileInfoLoader.thumbnailImage) {
        
        self.imageView.image = self.fileInfoLoader.thumbnailImage;
        
        self.boxFile.thumbnailImage = self.fileInfoLoader.thumbnailImage;
        
    }
    
    
    //update view
    [self.view display];
    
}


@end
