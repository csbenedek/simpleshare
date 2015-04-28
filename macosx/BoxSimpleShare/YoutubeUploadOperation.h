//
//  YoutubeUploadOperation.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 3/23/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UploadOperation.h"

@class ESSYouTube;
@class GTLServiceTicket;
@interface YoutubeUploadOperation : UploadOperation
{
    ESSYouTube *yt;
    
    NSString* _title;
    NSString* _description;
    NSString* _tag;
    NSString* _categoty;
    NSString* _privateStatus;
    GTLServiceTicket *_uploadFileTicket;
    
    long long totalBytesUpload;
    long long totalBytesUploadedSofar;

}

-(void)startUpload;
-(void)onError:(NSString*)description;
-(void)didSendBytes;

- (long long) bytesUploadedSoFar;

- (long long) totalBytesToUpload;
@end
