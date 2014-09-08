//
//  BoxFile.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/17/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "BoxFile.h"


@implementation BoxFile

@synthesize uploadStatus, fileName, fileID, isShared, publicName, folderID, shortURL, isImgur;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (id) initWithCoder:(NSCoder *)aDecoder
{
    if ((self = [super init]))
    {
        uploadStatus = [[aDecoder decodeObjectForKey:@"uploadStatus"] retain];
        fileName = [[aDecoder decodeObjectForKey:@"fileName"] retain];
        fileID = [[aDecoder decodeObjectForKey:@"fileID"] retain];
        publicName = [[aDecoder decodeObjectForKey:@"publicName"] retain];
        folderID = [[aDecoder decodeObjectForKey:@"folderID"] retain];
        isShared = [aDecoder decodeBoolForKey:@"isShared"];
        isImgur = [aDecoder decodeBoolForKey:@"isImgur"];
        if ([aDecoder decodeObjectForKey:@"shortURL"])
            shortURL = [[aDecoder decodeObjectForKey:@"shortURL"] retain];
    }
    return self;
}

- (void) encodeWithCoder:(NSCoder *)aCoder
{
    [aCoder encodeObject:uploadStatus forKey:@"uploadStatus"];
    [aCoder encodeObject:fileName forKey:@"fileName"];
    [aCoder encodeObject:fileID forKey:@"fileID"];
    [aCoder encodeObject:publicName forKey:@"publicName"];
    [aCoder encodeObject:folderID forKey:@"folderID"];
    [aCoder encodeBool:isShared forKey:@"isShared"];
    [aCoder encodeBool:isImgur forKey:@"isImgur"];
    if (shortURL)
        [aCoder encodeObject:shortURL forKey:@"shortURL"];
}

- (void)dealloc
{
    safe_release(uploadStatus);
    safe_release(fileName);
    safe_release(fileID);
    safe_release(publicName);
    safe_release(folderID);
    safe_release(shortURL);
    
    [super dealloc];
}

@end
