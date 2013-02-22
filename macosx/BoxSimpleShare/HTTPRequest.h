//
//  HTTPRequest.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "ASIHTTPRequest.h"


@interface HTTPRequest : ASIHTTPRequest {
 
    NSMutableArray *multicastDelegate;
    
}

- (void) addMulticastDelegate:(id)obj;
- (void) removeMulticastDelegate:(id)obj;

@end
