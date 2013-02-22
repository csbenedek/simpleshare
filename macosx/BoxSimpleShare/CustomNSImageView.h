//
//  CustomNSImageView.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/9/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface CustomNSImageView : NSImageView {
@private
    NSString *_imgURL;
}

@property (retain, nonatomic) NSString *imageURL;

- (void) reloadImage;

@end
