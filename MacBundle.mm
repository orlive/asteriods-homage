#include "MacBundle.h"
#import <Foundation/Foundation.h>

MacBundle::MacBundle() { }
MacBundle::~MacBundle() { }

std::string MacBundle::recourcePath() {
    NSString *bundlePath = [[NSBundle mainBundle]resourcePath];
    return std::string([bundlePath UTF8String]);
}

