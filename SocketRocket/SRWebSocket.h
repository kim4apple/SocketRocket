//
//   Copyright 2012 Square Inc.
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

#import <Foundation/Foundation.h>
#import <Security/SecCertificate.h>

typedef enum {
    XSR_CONNECTING   = 0,
    XSR_OPEN         = 1,
    XSR_CLOSING      = 2,
    XSR_CLOSED       = 3,
} XSRReadyState;

typedef enum XSRStatusCode : NSInteger {
    XSRStatusCodeNormal = 1000,
    XSRStatusCodeGoingAway = 1001,
    XSRStatusCodeProtocolError = 1002,
    XSRStatusCodeUnhandledType = 1003,
    // 1004 reserved.
    XSRStatusNoStatusReceived = 1005,
    // 1004-1006 reserved.
    XSRStatusCodeInvalidUTF8 = 1007,
    XSRStatusCodePolicyViolated = 1008,
    XSRStatusCodeMessageTooBig = 1009,
} XSRStatusCode;

@class XSRWebSocket;

extern NSString *const XSRWebSocketErrorDomain;
extern NSString *const XSRHTTPResponseErrorKey;

#pragma mark - XSRWebSocketDelegate

@protocol XSRWebSocketDelegate;

#pragma mark - XSRWebSocket

@interface XSRWebSocket : NSObject <NSStreamDelegate>

@property (nonatomic, weak) id <XSRWebSocketDelegate> delegate;

@property (nonatomic, readonly) XSRReadyState readyState;
@property (nonatomic, readonly, retain) NSURL *url;

// This returns the negotiated protocol.
// It will be nil until after the handshake completes.
@property (nonatomic, readonly, copy) NSString *protocol;

// Protocols should be an array of strings that turn into Sec-WebSocket-Protocol.
- (id)initWithURLRequest:(NSURLRequest *)request protocols:(NSArray *)protocols;
- (id)initWithURLRequest:(NSURLRequest *)request;

// Some helper constructors.
- (id)initWithURL:(NSURL *)url protocols:(NSArray *)protocols;
- (id)initWithURL:(NSURL *)url;

// Delegate queue will be dispatch_main_queue by default.
// You cannot set both OperationQueue and dispatch_queue.
- (void)setDelegateOperationQueue:(NSOperationQueue*) queue;
- (void)setDelegateDispatchQueue:(dispatch_queue_t) queue;

// By default, it will schedule itself on +[NSRunLoop SR_networkRunLoop] using defaultModes.
- (void)scheduleInRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;
- (void)unscheduleFromRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;

// SRWebSockets are intended for one-time-use only.  Open should be called once and only once.
- (void)open;

- (void)close;
- (void)closeWithCode:(NSInteger)code reason:(NSString *)reason;

// Send a UTF8 String or Data.
- (void)send:(id)data;

// Send Data (can be nil) in a ping message.
- (void)sendPing:(NSData *)data;

@end

#pragma mark - XSRWebSocketDelegate

@protocol XSRWebSocketDelegate <NSObject>

// message will either be an NSString if the server is using text
// or NSData if the server is using binary.
- (void)webSocket:(XSRWebSocket *)webSocket didReceiveMessage:(id)message;

@optional

- (void)webSocketDidOpen:(XSRWebSocket *)webSocket;
- (void)webSocket:(XSRWebSocket *)webSocket didFailWithError:(NSError *)error;
- (void)webSocket:(XSRWebSocket *)webSocket didCloseWithCode:(NSInteger)code reason:(NSString *)reason wasClean:(BOOL)wasClean;
- (void)webSocket:(XSRWebSocket *)webSocket didReceivePong:(NSData *)pongPayload;

@end

#pragma mark - NSURLRequest (XCertificateAdditions)

@interface NSURLRequest (XCertificateAdditions)

@property (nonatomic, retain, readonly) NSArray *XSR_SSLPinnedCertificates;

@end

#pragma mark - NSMutableURLRequest (XCertificateAdditions)

@interface NSMutableURLRequest (XCertificateAdditions)

@property (nonatomic, retain) NSArray *XSR_SSLPinnedCertificates;

@end

#pragma mark - NSRunLoop (XSRWebSocket)

@interface NSRunLoop (XSRWebSocket)

+ (NSRunLoop *)XSR_networkRunLoop;

@end
