
#ifndef __COMPOSITE_VIDEO_H__
#define __COMPOSITE_VIDEO_H__

/*
 *
 *	Initializes composite video output on PE1 (low bit) and PE2 (high bit)
 *	PE1 and PE2 are fed into a voltage divider for the final video signal
 *	TODO: make a circuit diagram of composite output
 *	Return: None
 *
 */
void composite_video_init();

#endif
