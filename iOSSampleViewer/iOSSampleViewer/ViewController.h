//
//  ViewController.h
//  iOSSampleViewer
//
//  Created by Oduwa Edo Osagie on 20/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController

@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (weak, nonatomic) IBOutlet UIToolbar *toolbar;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *expandBarButton;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *collapseBarButton;

- (IBAction)expandBarButtonPressed:(id)sender;
- (IBAction)collapseBarButtonPressed:(id)sender;


@end

