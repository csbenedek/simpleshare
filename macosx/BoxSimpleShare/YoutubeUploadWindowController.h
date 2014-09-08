//
//  YoutubeUploadWindowController.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 4/7/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "JCSSheetController.h"

@interface YoutubeUploadWindowController : JCSSheetController
{
    
//    IBOutlet NSPopUpButton *_uploadPrivacyPopup;
//    IBOutlet NSPopUpButton *_uploadCategoryPopup;
//    IBOutlet NSTextField *_uploadTitleField;
//    IBOutlet NSTextField *_uploadDescriptionField;
//    IBOutlet NSTextField *_uploadTagsField;
}

@property (nonatomic, strong) IBOutlet NSPopUpButton *_uploadPrivacyPopup;
@property (nonatomic, strong) IBOutlet NSPopUpButton *_uploadCategoryPopup;
@property (nonatomic, strong) IBOutlet NSTextField *_uploadTitleField;
@property (nonatomic, strong) IBOutlet NSTextField *_uploadDescriptionField;
@property (nonatomic, strong) IBOutlet NSTextField *_uploadTagsField;


+ (YoutubeUploadWindowController *)sharedWindowController;
-(void)prepareData;

-(IBAction)cancelClick:(id)sender;
-(IBAction)okClick:(id)sender;

@end
