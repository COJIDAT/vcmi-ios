//
//  OGVPlayerState.h
//  OGVKit
//
//  Created by Brion on 6/13/15.
//  Copyright (c) 2015 Brion Vibber. All rights reserved.
//
//

@class OGVPlayerState;


/**
 * Delegate messages are sent on the main thread dispatch queue, unless
 * a different queue is specified in the init call. If NULL is specified
 * as the dispatch queue, delegate calls will be on the decode thread.
 */
@protocol OGVPlayerStateDelegate<NSObject>

-(void)ogvPlayerState:(OGVPlayerState *)state drawFrame:(OGVVideoBuffer *)buffer;

@optional
-(void)ogvPlayerStateDidLoadMetadata:(OGVPlayerState *)state;

@optional
-(void)ogvPlayerStateDidPlay:(OGVPlayerState *)state;

@optional
-(void)ogvPlayerStateDidPause:(OGVPlayerState *)sender;

@optional
-(void)ogvPlayerStateDidEnd:(OGVPlayerState *)state;

@optional
-(void)ogvPlayerStateDidSeek:(OGVPlayerState *)state;

@optional
-(void)ogvPlayerState:(OGVPlayerState *)state customizeURLRequest:(NSMutableURLRequest *)request;

@end


@interface OGVPlayerState : NSObject <OGVInputStreamDelegate>

-(instancetype)initWithInputStream:(OGVInputStream *)inputStream
                          delegate:(id<OGVPlayerStateDelegate>)delegate;

-(instancetype)initWithInputStream:(OGVInputStream *)inputStream
                          delegate:(id<OGVPlayerStateDelegate>)delegate
                     delegateQueue:(dispatch_queue_t)delegateQueue;

-(instancetype)initWithURL:(NSURL *)URL delegate:(id<OGVPlayerStateDelegate>)delegate;

-(void)play;
-(void)pause;
-(void)cancel;
-(void)seek:(float)time;

@property (readonly) BOOL paused;
@property (readonly) float playbackPosition;
@property (readonly) float duration;
@property (readonly) BOOL seekable;

@end
