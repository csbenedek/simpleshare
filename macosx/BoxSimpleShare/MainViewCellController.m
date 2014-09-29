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
#import "BoxSimpleShareAppDelegate.h"
#import "MainController.h"
#import <QuartzCore/QuartzCore.h>

@interface MainViewCellController ()

@end

@implementation MainViewCellController


-(void)openSharedLink{
    
    MainController *mainController = (MainController *)[[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    BoxFile *file = self.boxFile;
    
    
    if (file)
    {
		// open the url using the file link
		NSString *url = nil;
		
		if ([file shortURL])
			url = [file shortURL];
		else
			//url = [NSString stringWithFormat:@"http://www.box.com/shared/%@", [file publicName]];
			url = [file publicName];
		
		if (mainController.shorten_links_check && !file.isImgur)
		{
			url = URLShortner( url );
		}
		
		[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:url]];
		url = nil;
    }

    
    
}


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
        
        //hide spinner and stop animation
        
        [self.spinner stopAnimation:self];
        
        [self.spinner setHidden:TRUE];
        
        
        
    }
    
    
    //update view
    [self.view display];
    
}


@end
