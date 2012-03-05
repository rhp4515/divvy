//
//  DivvyImageController.h
//  Divvy
//
//  Created by Joshua Lewis on 8/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface DivvyImageController : NSViewController

@property (retain) IBOutlet NSArrayController *imageHeights;
@property (retain) IBOutlet NSObjectController *imageHeight;

- (IBAction) imageHeightSelect:(id)sender;
- (IBAction) resample:(id)sender;

@end
