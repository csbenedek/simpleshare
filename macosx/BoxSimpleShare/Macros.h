//
//  Macros.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/13/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

@interface Macros : NSObject {
@private

}

extern void (^SSLog)(id, ...);
extern NSString * (^MD5)(NSString *);
extern NSString * (^URLEncode)(NSString *);
extern NSString * (^returnValueWithAccurateAnnotation)(long long);

extern NSString * (^URLShortner)(NSString *);
extern BOOL (^CopyURLToClipBoardForType)(NSString *,  NSString *type, ...) NS_REQUIRES_NIL_TERMINATION;

@end