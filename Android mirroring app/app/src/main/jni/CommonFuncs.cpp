//
//  CommonFuncs.cpp
//  HelloWorld
//
//  Created by Oduwa Edo Osagie on 13/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "CommonFuncs.h"

void CommonFuncs::error(const char *msg)
{
    perror(msg);
    exit(0);
}