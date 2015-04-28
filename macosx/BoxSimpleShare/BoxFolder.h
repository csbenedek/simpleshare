//
//  BoxFolder.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/7/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface BoxFolder : NSObject {
@private
    
    NSString *folderID;
    NSString *folderName;
    NSString *folderTypeID;
    BOOL isShared;
    NSString *publicName;
    NSString *parentFolderID;
    NSString *password;
    
}

@property (retain, nonatomic) NSString *folderID;
@property (retain, nonatomic) NSString *folderName;
@property (retain, nonatomic) NSString *folderTypeID;
@property (nonatomic) BOOL isShared;
@property (retain, nonatomic) NSString *publicName;
@property (retain, nonatomic) NSString *parentFolderID;
@property (retain, nonatomic) NSString *password;

- (id)initWithDictionary:(NSDictionary*)dictionary;
- (NSDictionary*)dictionaryRepresentation;

@end
