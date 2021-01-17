//
//  OGVPlayerView.h
//  OGVKit
//
//  Created by Brion on 2/8/15.
//  Copyright (c) 2015 Brion Vibber. All rights reserved.
//

#import <UIKit/UIKit.h>

@class OGVPlayerView;

/**
 * Delegate methods are all called on the main thread.
 */
@protocol OGVPlayerDelegate <NSObject>

@optional
-(void)ogvPlayerDidLoadMetadata:(OGVPlayerView *)sender;

@optional
-(void)ogvPlayerDidPlay:(OGVPlayerView *)sender;

@optional
-(void)ogvPlayerDidPause:(OGVPlayerView *)sender;

@optional
-(void)ogvPlayerDidEnd:(OGVPlayerView *)sender;

@optional
-(void)ogvPlayerDidSeek:(OGVPlayerView *)sender;

@optional
-(void)ogvPlayerControlsWillHide:(OGVPlayerView *)sender;

@optional
-(void)ogvPlayerControlsWillShow:(OGVPlayerView *)sender;

@optional
-(void)ogvPlayer:(OGVPlayerView *)sender customizeURLRequest:(NSMutableURLRequest *)request;

@end


@interface OGVPlayerView : UIView <OGVPlayerStateDelegate>

@property (weak) id<OGVPlayerDelegate> delegate;

@property (weak, nonatomic) IBOutlet UIView *controlBar;
@property (weak, nonatomic) IBOutlet UIButton *pausePlayButton;
@property (weak, nonatomic) IBOutlet UILabel *timeLabel;
@property (weak, nonatomic) IBOutlet UILabel *timeRemainingLabel;
@property (weak, nonatomic) IBOutlet UISlider *progressSlider;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicator;

@property (nonatomic) NSURL *sourceURL;
@property (nonatomic) OGVInputStream *inputStream;

@property (readonly) BOOL paused;
@property (readonly) float playbackPosition;

-(void)play;
-(void)pause;
-(void)seek:(float)seconds;

@end
