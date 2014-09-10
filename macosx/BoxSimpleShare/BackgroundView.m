#import "BackgroundView.h"
#import "NSColor+ColorExtensions.h"
#import <Quartz/Quartz.h>


#define FILL_OPACITY .9
#define STROKE_OPACITY .7

#define LINE_THICKNESS 1
#define CORNER_RADIUS 10

#define SEARCH_INSET 10

#pragma mark -

@implementation BackgroundView

@synthesize arrowX = _arrowX;
@synthesize opacity = _opacity;

- (void)setOpacity:(float)opacity
{
    _opacity = opacity;
    [self setNeedsDisplay:YES];
    
}

-(void) setBackgroundColor:(NSColor *)backgroundColor {
    
    [_color release];
    _color = [backgroundColor retain];
    
    [self setNeedsDisplay:YES];
}

-(NSColor*)backgroundColor
{
    return _color;
}
-(float) opacity {
    return _opacity;
}
- (void)awakeFromNib
{
    [super awakeFromNib];
    _opacity = 1.0;
    
}

#pragma mark -


- (void)drawRect:(NSRect)dirtyRect
{
    
    NSRect contentRect = NSInsetRect([self bounds], LINE_THICKNESS, LINE_THICKNESS);
    NSBezierPath *path = [NSBezierPath bezierPath];
    
    NSInteger tempArrow =  0;
    [path moveToPoint:NSMakePoint(_arrowX, NSMaxY(contentRect) - tempArrow)];
    [path lineToPoint:NSMakePoint(_arrowX + ARROW_WIDTH / 2, NSMaxY(contentRect) - ARROW_HEIGHT)];
    [path lineToPoint:NSMakePoint(NSMaxX(contentRect) - CORNER_RADIUS, NSMaxY(contentRect) - ARROW_HEIGHT)];
    
    NSPoint topRightCorner = NSMakePoint(NSMaxX(contentRect), NSMaxY(contentRect) - ARROW_HEIGHT);
    [path curveToPoint:NSMakePoint(NSMaxX(contentRect), NSMaxY(contentRect) - ARROW_HEIGHT - CORNER_RADIUS)
         controlPoint1:topRightCorner controlPoint2:topRightCorner];
    
    [path lineToPoint:NSMakePoint(NSMaxX(contentRect), NSMinY(contentRect) + CORNER_RADIUS)];
    
    NSPoint bottomRightCorner = NSMakePoint(NSMaxX(contentRect), NSMinY(contentRect));
    [path curveToPoint:NSMakePoint(NSMaxX(contentRect) - CORNER_RADIUS, NSMinY(contentRect))
         controlPoint1:bottomRightCorner controlPoint2:bottomRightCorner];
    
    [path lineToPoint:NSMakePoint(NSMinX(contentRect) + CORNER_RADIUS, NSMinY(contentRect))];
    [path curveToPoint:NSMakePoint(NSMinX(contentRect), NSMinY(contentRect) + CORNER_RADIUS)
         controlPoint1:contentRect.origin controlPoint2:contentRect.origin];
    
    [path lineToPoint:NSMakePoint(NSMinX(contentRect), NSMaxY(contentRect) - ARROW_HEIGHT - CORNER_RADIUS)];
    
    NSPoint topLeftCorner = NSMakePoint(NSMinX(contentRect), NSMaxY(contentRect) - ARROW_HEIGHT);
    [path curveToPoint:NSMakePoint(NSMinX(contentRect) + CORNER_RADIUS, NSMaxY(contentRect) - ARROW_HEIGHT)
         controlPoint1:topLeftCorner controlPoint2:topLeftCorner];
    
    
    [path lineToPoint:NSMakePoint(_arrowX - ARROW_WIDTH / 2, NSMaxY(contentRect) - ARROW_HEIGHT)];
    [path closePath];
    
//    [[NSColor whiteColor] setFill];
//    [path fill];
    if(self.backgroundColor)
    {
        [self.backgroundColor setFill];
        [path fill];
        
    }

    
}

#pragma mark -
#pragma mark Public accessors

- (void)setArrowX:(NSInteger)value
{
    _arrowX = value;
    [self setNeedsDisplay:YES];
}



-(BOOL)mouseDownCanMoveWindow {
    return NO;
}
@end
