//
//  YoutubeUploadWindowController.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 4/7/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "YoutubeUploadWindowController.h"

#import "GTLYoutube.h"
#import "GTL/GTLUtilities.h"
#import "GTL/GTMHTTPUploadFetcher.h"
#import "GTL/GTMHTTPFetcherLogging.h"
#import "GTL/GTMOAuth2WindowController.h"
#import "GTLYouTube.h"
#import <GTL/GTMOAuth2WindowController.h>
#import "BoxNetUser.h"
#import "YoutubeAuthenticateManager.h"

@interface YoutubeUploadWindowController ()

@end

@implementation YoutubeUploadWindowController

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}


- (GTLServiceYouTube *)youTubeService {
    static GTLServiceYouTube *service;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        service = [[GTLServiceYouTube alloc] init];
        
        // Have the service object set tickets to fetch consecutive pages
        // of the feed so we do not need to manually fetch them.
        service.shouldFetchNextPages = YES;
        
        // Have the service object set tickets to retry temporary error conditions
        // automatically.
        service.retryEnabled = YES;
    });
    return service;
}

+ (YoutubeUploadWindowController *)sharedWindowController {
    static YoutubeUploadWindowController* gWindowController = nil;
    if (!gWindowController) {
        gWindowController = [[YoutubeUploadWindowController alloc] init];
       /// [gWindowController prepareData];
        
    }
    return gWindowController;
}

- (id)init {
    
    if(self = [super initWithWindowNibName:@"YoutubeUploadWindowController"])
    {
       // [self prepareData];
    }
//    return [self initWithWindowNibName:@"YoutubeUploadWindowController"];
    return self;
}

-(void)sheetWillDisplay
{
    [self._uploadDescriptionField setStringValue:@""];
    [self._uploadTitleField setStringValue:@""];
    [self._uploadTagsField setStringValue:@""];
}

-(void)prepareData
{
   // [self fetchVideoCategories];

}
- (void)fetchVideoCategories {
    // For uploading, we want the category popup to have a list of all categories
    // that may be assigned to a video.
    GTLServiceYouTube *service = self.youTubeService;
    self.youTubeService.authorizer = [YoutubeAuthenticateManager shareManager].authenticate;
    
    GTLQueryYouTube *query = [GTLQueryYouTube queryForVideoCategoriesListWithPart:@"snippet,id"];
    query.regionCode = [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode];
    
    [service executeQuery:query
        completionHandler:^(GTLServiceTicket *ticket,
                            GTLYouTubeVideoCategoryListResponse *categoryList,
                            NSError *error) {
            if (error) {
                NSLog(@"Could not fetch video category list: %@", error);
            } else {
                // We will build a menu with the category names as menu item titles,
                // and category ID strings as the menu item represented
                // objects.
                NSMenu *categoryMenu = [[NSMenu alloc] init];
                for (GTLYouTubeVideoCategory *category in categoryList) {
                    NSString *title = category.snippet.title;
                    NSString *categoryID = category.identifier;
                    NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:title
                                                                  action:NULL
                                                           keyEquivalent:@""];
                    [item setRepresentedObject:categoryID];
                    [categoryMenu addItem:item];
                }
                [self._uploadCategoryPopup setMenu:categoryMenu];
                [self._uploadCategoryPopup setEnabled:YES];
            }
        }];
}

-(IBAction)cancelClick:(id)sender
{
    [self endSheetWithReturnCode:0];
}

-(IBAction)okClick:(id)sender
{
    if (self._uploadTagsField.stringValue.length == 0 || self._uploadDescriptionField.stringValue.length == 0 || self._uploadTitleField.stringValue.length == 0)
    {
        NSBeep();
        return;
    }
    [self endSheetWithReturnCode:1];
}
@end
