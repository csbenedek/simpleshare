//
//  NSString+HTMLTags.h
//  Links
//
//  Created by Sergey Plotnikov on 23/07/14.
//  Copyright (c) 2014 Sergey Plotnikov. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (HTMLTags)

-(NSString *)searchForValueForTag:(NSString *)tag;
-(NSString *)searchForFaviconURL;
-(NSString *)getHREFValue;



-(BOOL)containsString:(NSString *)string;




@end
