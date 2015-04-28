//
//  BoxFolder.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/7/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "BoxFolder.h"
#import "Extended.h"

@implementation BoxFolder

@synthesize folderID, folderName, folderTypeID, isShared, publicName, parentFolderID, password;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)dealloc
{
    [folderID release];
    [folderName release];
	[folderTypeID release];
	[publicName release];
	[parentFolderID release];
	[password release];
    
    [super dealloc];
}

- (id)initWithDictionary:(NSDictionary*)dictionary {
    if (self = [self init]) {
        self.folderID = [dictionary objectForKey:@"folderID"];
        self.folderName = [dictionary objectForKey:@"folderName"];
        self.folderTypeID = [dictionary objectForKey:@"folderTypeID"];
        self.publicName = [dictionary objectForKey:@"publicName"];
        self.parentFolderID = [dictionary objectForKey:@"parentFolderID"];
    }
    return self;
}

- (NSDictionary*)dictionaryRepresentation {
    NSMutableDictionary* dict = [NSMutableDictionary dictionary];
    [dict safeSetObject:folderID forKey:@"folderID"];
    [dict safeSetObject:folderName forKey:@"folderName"];
    [dict safeSetObject:folderTypeID forKey:@"folderTypeID"];
    [dict safeSetObject:publicName forKey:@"publicName"];
    [dict safeSetObject:parentFolderID forKey:@"parentFolderID"];

    return dict;
}

@end
