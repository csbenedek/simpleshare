//
//  FooterView.m
//  LiquidFiles
//
//  Created by Cuongnq on 11/22/13.
//  Copyright (c) 2013 Cuongnq. All rights reserved.
//

#import "FooterView.h"
#import <QuartzCore/QuartzCore.h>

@implementation FooterView


- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization codegdataxmlnode.m
        startingColor = [NSColor colorWithCalibratedWhite:0.95 alpha:1.0];
        endingColor = [NSColor colorWithCalibratedWhite:0.8 alpha:1.0];
        //        [self setAngle:270];
        
    }
    return self;
}
-(void)drawRect:(NSRect)dirtyRect {
    
    NSRect rect = [self bounds];
    //            if(self.row < [ self.listView  numberOfRows] -1){
    
     NSBezierPath *borderPath = [NSBezierPath bezierPath];
    [borderPath moveToPoint:NSMakePoint(NSMinX(rect) , NSMaxY(rect))];
    [borderPath lineToPoint:NSMakePoint(NSMaxX(rect) , NSMaxY(rect))];
     [borderPath lineToPoint:NSMakePoint(NSMaxX(rect) , NSMinY(rect) + 15)];
   
    NSPoint bottomRightCorner = NSMakePoint(NSMaxX(rect), NSMinY(rect));
    [borderPath curveToPoint:NSMakePoint(NSMaxX(rect) - 15, NSMinY(rect))
         controlPoint1:bottomRightCorner controlPoint2:bottomRightCorner];
    
    [borderPath lineToPoint:NSMakePoint(NSMinX(rect) + 15, NSMinY(rect))];
    [borderPath curveToPoint:NSMakePoint(NSMinX(rect), NSMinY(rect) + 15)
         controlPoint1:rect.origin controlPoint2:rect.origin];
    [borderPath closePath];
//    
//    NSGradient* aGradient = [[NSGradient alloc]
//                             initWithStartingColor:startingColor
//                             endingColor:endingColor];
//    [aGradient drawInBezierPath:borderPath angle:270];
    
    
    NSBezierPath *path = [NSBezierPath bezierPath];
    [path moveToPoint:NSMakePoint(NSMinX(rect) , NSMaxY(rect))];
    [path lineToPoint:NSMakePoint(NSMaxX(rect) , NSMaxY(rect))];
    [path closePath];
    
    [[NSColor secondarySelectedControlColor] set];
    [path stroke];
    [super drawRect:dirtyRect];
    
}


-(void) startAnimation {
    
    
    CALayer *myLayer = [self.image layer];
    CABasicAnimation* rotationAnimation = [CABasicAnimation animationWithKeyPath:@"transform.rotation.z"];
    rotationAnimation.toValue = [NSNumber numberWithFloat: (-2* M_PI) * 1];
    rotationAnimation.duration = 2.0f;
    rotationAnimation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionLinear];
    
    
    
    CGPoint anchorPoint = CGPointMake(0.487, 0.5);
    CGPoint newPoint = CGPointMake(self.image.bounds.size.width * anchorPoint.x,
                                   self.image.bounds.size.height * anchorPoint.y);
    CGPoint oldPoint = CGPointMake(self.image.bounds.size.width * myLayer.anchorPoint.x,
                                   self.image.bounds.size.height * myLayer.anchorPoint.y);
    
    newPoint = CGPointApplyAffineTransform(newPoint,
                                           [self.image.layer affineTransform]);
    oldPoint = CGPointApplyAffineTransform(oldPoint,
                                           [self.image.layer affineTransform]);
    
    CGPoint position = self.image.layer.position;
    
    position.x -= oldPoint.x;
    position.x += newPoint.x;
    
    position.y -= oldPoint.y;
    position.y += newPoint.y;
    
    myLayer.position = position;
    myLayer.anchorPoint = anchorPoint;
    //  myLayer.position = anchorPoint;
    rotationAnimation.cumulative = YES;
    rotationAnimation.repeatDuration = DBL_MAX;
    
    [myLayer addAnimation:rotationAnimation forKey:@"transform.rotation"];
    
}

@end
