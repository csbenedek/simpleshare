//
//  mainView.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 12/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "mainView.h"
#import "MainViewCellController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "BoxFile.h"
#import "AttachedWindowsController.h"


@interface mainView ()

@end

@implementation mainView

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
    
        
        
        
    }
    return self;
}

#pragma mark - NSTableViewDataSource methods


-(NSView *) tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row{
    
    //return cell from cells array
    
    MainViewCellController *cellController = [self.cells objectAtIndex:row];
    
    return cellController.view;
    
    
}




-(NSInteger)numberOfRowsInTableView:(NSTableView *)tableView{
    
    
    
    return [self.cells count];
    
   
    
}


-(void)awakeFromNib{
    
    NSLog(@"Main VIew awake from nib!");
    
    //init cells array
    
    self.cells = [[NSMutableArray alloc] init];
    
    
    
    //initial population of history elements array
    [self updateHistoryElements];
    
    //register nib for MainViewCell identifier
    NSNib *nib = [[NSNib alloc] initWithNibNamed:@"MainViewCellController" bundle:nil];
    
    [self.tableView registerNib:nib forIdentifier:@"MainViewCell"];
    

    
}

-(void)processNewHistoryElementNotification:(NSNotification *)notification{
    
    
    [self updateHistoryElements];
    
    
    
}


-(IBAction)quitApp:(id)sender{
    
    BoxSimpleShareAppDelegate *delegate =(BoxSimpleShareAppDelegate *)[[NSApplication sharedApplication] delegate];
    
    
    [delegate quitApplication];
    
    
}

-(void)showCaptureVideoView:(id)sender{
    
    //make attachedWindowsCOntroller display capture video window
    [[self rootController] displayVideoCaptureWindow];
    
    
    
}



-(void)updateHistoryElements{
    
   //clear cells array
    
    [self.cells removeAllObjects];
    
    //get array of uploaded items
        
    BoxSimpleShareAppDelegate *appDelegate = (BoxSimpleShareAppDelegate *)[[NSApplication sharedApplication] delegate];
    
    //go through list of saved files in reverse order
        
    for (BoxFile *file in [appDelegate.filesUploadedInSession reverseObjectEnumerator]) {
        
        //make cell for each file
        
        //controller to load nib and set properties
        MainViewCellController *controller = [[MainViewCellController alloc] initWithNibName:@"MainViewCellController" bundle:nil];
        
        //just triggering lazy loading
        NSView *tmpView = controller.view;
        

        
        //set properties
        
        [controller.fileName setTitleWithMnemonic:file.fileName];
        
        
        //store controller
        
        [self.cells addObject:controller];
        
        
        
    }
    
    
    //reload table view
    [self.tableView reloadData];
    
    

    
}







@end
