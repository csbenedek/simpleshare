//
//  FolderUtility.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 1/10/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FolderUtility : NSObject
{
    
}

-(BOOL)checkPermision ;
-(BOOL) getPathToDesktopFolder;
-(BOOL) tellUserTrustDesktopFolder;
@end
