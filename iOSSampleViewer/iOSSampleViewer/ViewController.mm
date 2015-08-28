//
//  ViewController.m
//  iOSSampleViewer
//
//  Created by Oduwa Edo Osagie on 20/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#import "ViewController.h"
#import "AndroidViewer.h"
#import "zlib.h"

@interface ViewController ()


@end

@implementation ViewController

ViewController *vc;
UIImageView *publicImageView;

class Callback : public AndroidViewer::AndroidViewerCallbackInterface {
public:
    void newFrameAvailable(signed char* data, int size){
        // zlib decompress
//        uLong initialSize = size;
//        uLong resultingSize = 1664*2392*4;
//        char *decompressedData = new char[resultingSize];
//        uncompress((Bytef*)decompressedData, &resultingSize, (Bytef*)data, initialSize);
//        
//        printf("CALLBACK: DECOMPRESSED FROM %d TO %lu \n", size, resultingSize);
        printf("CALLBACK: %d \n", size);
        
        //UIImage *i = [[UIImage alloc] initWithData:[NSData dataWithBytes:decompressedData length:resultingSize]];
        UIImage *i = [[UIImage alloc] initWithData:[NSData dataWithBytes:data length:size]];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            vc.imageView.image = i;
        });
        
        //delete [] decompressedData;
    }
    void debug(){
        NSLog(@"DEBUG CALLBACK");
    }
};
Callback cb;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    vc = self;
    _imageView.contentMode = UIViewContentModeScaleAspectFill;
    publicImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"x.jpg"]];
    _imageView.image = [UIImage imageNamed:@"x.jpg"];
    //[self.view addSubview:publicImageView];
    
    /* Toolbar */
    _toolbar.backgroundColor = [UIColor colorWithRed:100/255.0 green:23/255.0 blue:43/255.0 alpha:1];
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(didTapScreen)];
    [self.view addGestureRecognizer:tap];
    [self.view bringSubviewToFront:_toolbar];
}

- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    [self startViewer];
}

- (void) startViewer
{
    const char *c1 = [_ip UTF8String];
    char *c2 = new char[32];
    sprintf(c2, "%s", c1);
    
    AndroidViewer *viewer = new AndroidViewer(c2, 6880, &cb);
    viewer->connect();
    
    dispatch_queue_t viewingQueue = dispatch_queue_create("Viewing Queue",NULL);
    dispatch_async(viewingQueue, ^{
        viewer->startViewing();
    });
}

- (void) didTapScreen
{
    if(_toolbar.alpha != 0){
        _toolbar.alpha = 0;
    }
    else{
        _toolbar.alpha = 1.0;
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)expandBarButtonPressed:(id)sender
{
    dispatch_async(dispatch_get_main_queue(), ^{
        _imageView.contentMode = UIViewContentModeScaleToFill;
    });
}

- (IBAction)collapseBarButtonPressed:(id)sender
{
    dispatch_async(dispatch_get_main_queue(), ^{
        _imageView.contentMode = UIViewContentModeScaleAspectFit;
    });
}
@end
