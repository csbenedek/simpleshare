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
#import "YouTubeThumbnailLoader.h"
#import "ImgurThumbnailLoader.h"

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
        
        self.youTubeThumbnailLoader = [[YouTubeThumbnailLoader alloc] init];
        
        self.imgurThumbnailLoader = [[ImgurThumbnailLoader alloc] init];
        
        
        //register self for notifications from loaders
        
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(processThumbnailInfoLoadedNotification:) name:@"ThumbnailLoadedNotification" object:self.fileInfoLoader];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(processYouTubeThumbnailLoadedNotification:) name:@"YouTubeThumbnailLoadedNotification" object:self.youTubeThumbnailLoader];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(processImgurThumbnailLoadedNotification:) name:@"ImgurThumbnailLoadedNotification" object:self.imgurThumbnailLoader];
        
        
        
    }
    return self;
}


-(void)processImgurThumbnailLoadedNotification:(NSNotification *)notification{
    
    NSLog(@"ImgurThumbnail");
    
    if (self.imgurThumbnailLoader.thumbnail) {
        
        self.imageView.image = self.imgurThumbnailLoader.thumbnail;
        
        self.boxFile.thumbnailImage = self.imgurThumbnailLoader.thumbnail;
        
        //hide spinner and stop animation
        
        [self.spinner stopAnimation:self];
        
        [self.spinner setHidden:TRUE];
        
        
    }
    
    
    [self.view display];
    
    
}

-(void)processYouTubeThumbnailLoadedNotification:(NSNotification *)notification{
    
    NSLog(@"YouTubeThumbnailLoadedNotification");
    
    if (self.youTubeThumbnailLoader.thumbnail) {
        
        self.imageView.image = self.youTubeThumbnailLoader.thumbnail;
        
        self.boxFile.thumbnailImage = self.youTubeThumbnailLoader.thumbnail;
        
        //hide spinner and stop animation
        
        [self.spinner stopAnimation:self];
        
        [self.spinner setHidden:TRUE];
        
        
    }
    
    
    [self.view display];
    
    
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
