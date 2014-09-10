#define ARROW_WIDTH 20
#define ARROW_HEIGHT 10

@interface BackgroundView : NSView
{
    NSInteger _arrowX;
    float _opacity;
    NSColor* _color;
      BOOL _diconnectMode;
}

@property (nonatomic, assign) NSInteger arrowX;
@property (nonatomic, assign) float opacity;
@property (nonatomic, strong) NSColor* backgroundColor;

@end
