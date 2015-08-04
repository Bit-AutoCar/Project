#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"


clock_t start_point, end_point;
 
 
struct termios orig_termios;



void ColorDetectPi();


//-- 키보드관련 함수 
//
void KeyBD_Reset_TerminalMode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void KeyBD_Set_ConioTerminalMode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(KeyBD_Reset_TerminalMode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int KeyBD_IsPressed()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}


int KeyBD_Getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}


//--------------------------------------------------------------------------
//	Name : main
//
//--------------------------------------------------------------------------
int main(int argc,char** argv)
{
	KeyBD_Set_ConioTerminalMode();
	
	ColorDetectPi();
	
	
    return 0;
}


#define USE_FOR_MAC		false


void ColorDetectPi()
{
	char 	ch;
	double 	process_time;
//	int 	process_rate;

	clock_t start_point;
	clock_t end_point;
		
	// 프레임 포인터
	IplImage *frame;
	IplImage *image;
	IplImage *img_bin;	
	IplImage *img_red;
	IplImage *img_green;
	IplImage *img_blue;
	IplImage *img_dst;
	
	int threadhold_red_value;
	int threadhold_bin_value;
	
	// Create capture device ready
	// here 0 indicates that we want to use camera at 0th index
	CvCapture *capture = cvCaptureFromCAM(0);
	
	// 캡쳐 이미지 사이즈 설정
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH	, 320);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT	, 240);
	
#if USE_FOR_MAC == true
	cvNamedWindow("Cam",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Red",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Out",CV_WINDOW_AUTOSIZE);
#endif
	
	frame = cvQueryFrame(capture);
	
	image = cvCreateImage(cvSize(frame->width,frame->height), 8, 3);

	img_bin 	= cvCreateImage(cvSize(frame->width,frame->height), 8, 1);	
	img_red 	= cvCreateImage(cvSize(frame->width,frame->height), 8, 1);
	img_green 	= cvCreateImage(cvSize(frame->width,frame->height), 8, 1);
	img_blue 	= cvCreateImage(cvSize(frame->width,frame->height), 8, 1);
	
	img_dst		= cvCreateImage(cvSize(frame->width,frame->height), 8, 1);
	
#if USE_FOR_MAC == true	
	threadhold_red_value = 120;
	threadhold_bin_value = 150;
#else
	threadhold_red_value = 120;
	threadhold_bin_value = 220;
#endif
		
	while(1)
	{
		start_point = clock();
		
		// 카메라로 부터 1프레임 이미지를 읽어온다.
		frame = cvQueryFrame(capture);
		
		// YCrCb 형태로 변경
		cvCvtColor(frame, image, CV_RGB2YCrCb);
		
		// 채널별로 분리
		cvSplit(image, img_blue, img_green, img_red, NULL);
		cvMaxS(img_red, threadhold_red_value, img_red);
		
#if USE_FOR_MAC == true
		cvShowImage("Red", img_red);
#endif
		
		// 이진화
		cvThreshold(img_red, img_bin, threadhold_bin_value, 255, CV_THRESH_BINARY);
		
		// 팽창
		cvDilate(img_bin, img_dst, NULL, 4);
		
		// 침식
		cvErode(img_dst, img_dst, NULL, 4);
		
		// 모멘트 구하기
		CvMoments moments;
		
		int binary = 1;
		
		cvMoments(img_dst, &moments, binary);
		
		
		
		double M;
		int x_order;
		int y_order;
		double cX, cY;
		double m00;
		
		for( y_order=0; y_order<=3; y_order++ )
		{
			for( x_order=0; x_order<=3; x_order++ )
			{
				if( x_order+y_order > 3 )
				{
					continue;
				}
				
				M = cvGetSpatialMoment( &moments, x_order, y_order );
				
				if( x_order == 0 && y_order == 0 )
				{
					m00 = M;
				}
				else if( x_order == 1 && y_order == 0 )
				{
					cX = M;
				}
				else if( x_order == 0 && y_order == 1 )
				{
					cY = M;
				}
			}
		}
		
		cX = cX / m00;
		cY = cY / m00;
		
		cvCircle(frame, cvPoint(cvRound(cX), cvRound(cY)), 20, CV_RGB(0, 0, 255), 2);
		
#if USE_FOR_MAC == true
		cvShowImage("Out", img_dst);
		cvShowImage("Cam", frame);
#endif
		
		
		// Labeling
		// 구형 예정
		
		
#if USE_FOR_MAC == true
		ch =  cvWaitKey(25);  // Wait for 25 ms for user to hit any key
		if(ch==27) break;  // If Escape Key was hit just exit the loop
#else
		ch = 0;
		
		if( KeyBD_IsPressed() )
		{
			ch = KeyBD_Getch();		
		}		
		
		if( ch == 'q' )
		{
			break;
		}
		
		//if( ch == 's' )
		{
			cvSaveImage("/srv/cam.jpg",frame);
			cvSaveImage("/srv/http/out.jpg",img_dst);
			//cvSaveImage("/srv/http/out.jpg",img_red);
		}
		
		end_point = clock();
		process_time = ((double)(end_point - start_point)/(CLOCKS_PER_SEC/1000));
		printf("Exe time : %04f msec  %03.1f frames\r\n", process_time, 1000./process_time ); 
						
#endif
	
	
		
	}
	
	// Release All Images and Windows
	cvReleaseImage(&frame);
	cvReleaseImage(&image);
	cvReleaseImage(&img_bin);
	cvReleaseImage(&img_red);
	cvReleaseImage(&img_green);
	cvReleaseImage(&img_blue);
	cvReleaseImage(&img_dst);
	
#if USE_FOR_MAC == true
	cvDestroyWindow("Cam");
	cvDestroyWindow("Out");
#endif
}
