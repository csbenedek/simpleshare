//
//  BoxFile.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/17/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface BoxFile : NSObject <NSCoding> {
@private
    
    NSString *uploadStatus;
    NSString *fileName;
    NSString *fileID;
    BOOL isShared;
    BOOL isImgur;
    NSString *publicName;
    NSString *folderID;
    NSString *shortURL;
}

@property (retain, nonatomic) NSString *uploadStatus;
@property (retain, nonatomic) NSString *fileName;
@property (retain, nonatomic) NSString *fileID;
@property (nonatomic) BOOL isShared;
@property (retain, nonatomic) NSString *publicName;
@property (retain, nonatomic) NSString *folderID;
@property (retain, nonatomic) NSString *shortURL;
@property (nonatomic) BOOL isImgur;

@end
