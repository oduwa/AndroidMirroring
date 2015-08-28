//
//  IPInputViewController.m
//  iOSSampleViewer
//
//  Created by Oduwa Edo Osagie on 25/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#import "IPInputViewController.h"
#import "ViewController.h"

@interface IPInputViewController ()

@property (nonatomic, strong) UITextField *inputField;

@end

@implementation IPInputViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    [self initInputField];
}

- (void) initInputField
{
    _inputField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, 0.5*[[UIScreen mainScreen] bounds].size.width, 44)];
    _inputField.center = self.view.center;
    _inputField.placeholder = @"Enter Host IP Address:";
    _inputField.returnKeyType = UIReturnKeyGo;
    [self.view addSubview:_inputField];
    
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(didTapBackground)];
    [self.view addGestureRecognizer:tap];
    
    _inputField.delegate = self;
}

- (void) didTapBackground
{
    [_inputField resignFirstResponder];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [_inputField resignFirstResponder];
    
    ViewController *vc = [self.storyboard instantiateViewControllerWithIdentifier:@"viewController"];
    vc.ip = [textField text];
    [self presentViewController:vc animated:YES completion:nil];
    
    return YES;
}


/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
