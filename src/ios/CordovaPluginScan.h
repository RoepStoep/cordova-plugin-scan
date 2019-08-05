#import <Cordova/CDVPlugin.h>
#import "CordovaPluginScanc.h"

@interface CordovaPluginScan : CDVPlugin

- (void)init:(CDVInvokedUrlCommand*)command;

- (void)cmd:(CDVInvokedUrlCommand*)command;

- (void)output:(CDVInvokedUrlCommand*)command;

- (void)exit:(CDVInvokedUrlCommand*)command;

- (void)onAppTerminate;

@end
