
// FingerSkel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <GL/glut.h>

#include "iostream"

#include "util_render.h"
#include "util_pipeline.h"
#include <windows.h>
#include <math.h>
#include <wchar.h>
#include <list>
#include <vector>
#include <conio.h>
#include "pxcsession.h"
#include "pxcsmartptr.h"
#include "pxccapture.h"
#include "util_render.h"
#include "util_capture_file.h"
#include "util_cmdline.h"
#include "pxcgesture.h"
//#include "gesture_render.h"
#include "util_pipeline.h"
#include "iostream"
#define PI 3.141592653589
#define size_smoother 8
#include <fstream>
#include <ctime>


using namespace std;

bool initBool=FALSE,stop_control=FALSE,start_control=TRUE;;
int x,y,x_N,y_N,lx,ly,lz,x_world,y_world,z_world,tx,ty,tz,pinx,piny,pinz,tempX,tempY;
int z_N,avgX [size_smoother], avgY[size_smoother],counter=0;
double M_X,M_Y, a,b,c;
int w=0,i=0,count=0 ;
unsigned int index=0,tstampTemp=0,tstamp[1];
float averageX=0,averageY=0,tempavgX,tempavgY;
int width=0,height=0;



PXCGesture::GeoNode thumb_data,Index_data,lower_data,pinky_data,ring_data,hand_data[5];
bool jointOne=FALSE,jointTwo=FALSE,rotX=FALSE,rotY=FALSE,rotZ=FALSE,intiBool=FALSE,start=TRUE;
int tracking =0;
float x1,y3,z1,x2,y2,z2,m;
ofstream outputFile;
time_t currenttime,previoustime=0;

void GetDesktopResolution(int& horizontal, int& vertical)
	{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
	}





void LeftClick ( )
	{  
	INPUT    Input={0};
	// left down 
	Input.type      = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1,&Input,sizeof(INPUT));

	// left up
	::ZeroMemory(&Input,sizeof(INPUT));
	Input.type      = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
	::SendInput(1,&Input,sizeof(INPUT));
	}

void RightClick ( )
	{  
	INPUT    Input={0};
	// Right down 
	Input.type      = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1,&Input,sizeof(INPUT));

	// Right up
	::ZeroMemory(&Input,sizeof(INPUT));
	Input.type      = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_RIGHTUP;
	::SendInput(1,&Input,sizeof(INPUT));
	}

class Mypipeline : public UtilPipeline
	{


	virtual void PXCAPI OnGesture(PXCGesture::Gesture *data) {
		if ((data->label&PXCGesture::Gesture::LABEL_MASK_SET)<PXCGesture::Gesture::LABEL_SET_CUSTOMIZED) {
			switch (data->label) {
			case PXCGesture::Gesture::LABEL_POSE_THUMB_DOWN:
				RightClick();
				break;
			case PXCGesture::Gesture::LABEL_POSE_THUMB_UP:     
				LeftClick();
				break;
			case PXCGesture::Gesture::LABEL_POSE_BIG5:     
				stop_control=FALSE;
				start_control=TRUE;
				break;

			case PXCGesture::Gesture::LABEL_POSE_PEACE:     
				stop_control=TRUE;
				start_control=FALSE;
				break;

				}
			}
		}



	virtual void PXCAPI OnAlert(PXCGesture::Alert *alert)
		{
		if (alert->label&PXCGesture::Alert::LABEL_FOV_LEFT)
			{
			cout<<"OUT of FOV : LEFT";

			}

		if (alert->label&PXCGesture::Alert::LABEL_FOV_RIGHT)
			{
			cout<<"OUT of FOV : Right";
			}
		if (alert->label&PXCGesture::Alert::LABEL_FOV_TOP)		
			{
			cout<<"OUT of FOV : TOP";
			}
		if (alert->label&PXCGesture::Alert::LABEL_FOV_BOTTOM)	
			{
			cout<<"OUT of FOV : BOTTOM";
			}
		if (alert->label&PXCGesture::Alert::LABEL_FOV_BLOCKED)	  
			{
			cout<<"OUT of FOV : BLOCKED";
			}
		if (alert->label&PXCGesture::Alert::LABEL_GEONODE_ACTIVE)	  
			{
			cout<<"OUT of FOV : BLOCKED";
			tracking=1;
			}
		if (alert->label&PXCGesture::Alert::LABEL_GEONODE_INACTIVE)	  
			{
			cout<<"OUT of FOV : BLOCKED";
			tracking=2;
			}
		}



	}ppp;
void fillzero ()
	{
	int i=0;
	for (i=0;i<(size_smoother+1);i++)
		{

		avgX[i]=0;
		avgY[i]=0;

		}
	}

void init (void)
	/* this function sets the initial state */
	{
	/* select clearing (background) color to white */
	glClearColor (0.0, 0.0, 0.0, 0.0);
	/* initialize viewing values */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 320, 0, 240, -1.0, 1.0);


	}

void mousemover()

	{
	
	
	if(!initBool){

		ppp.EnableGesture();
		ppp.Init();	
		//fillzero();
		}
	initBool=TRUE;
	ppp.AcquireFrame(true);


	GetDesktopResolution(width,height);

	ppp.QueryGesture()->QueryNodeData(0,PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY|PXCGesture::GeoNode::LABEL_FINGER_INDEX, &Index_data);
	x_N=Index_data.positionImage.x;
	y_N=Index_data.positionImage.y;
	currenttime=time(NULL);
	if(x_N>290)
		{
		x_N=290;
		}


	if(y_N>210)
		{
		y_N=210;
		}

	if(x_N<10)
		{
		x_N=0;
		}


	if(y_N<10)
		{
		y_N=0;
		}


	if(x_N!=0 && y_N!=0)
		if(index>=size_smoother)
			{
			for(int j=0;j<index-1;j++)
				{
				avgX[j]=avgX[j+1];
				avgY[j]=avgY[j+1];
				}
			avgX[index-1]=x_N;
			avgY[index-1]=y_N;
			cout<<"avgY- "<<avgY[index-1]<<" "<<index-1<<"\n";
			}
		else{
			avgX[index]=x_N;
			avgY[index]=y_N;
			cout<<avgX[index]<<"\n";
			index++;
			}


		for(int j=0;j<index;j++)
			cout<<avgX[j]<<" ";
		cout<<endl;
		for(i=0;i<index;i++)

			{
			averageX=(averageX+avgX[i]);

			averageY=(averageY+avgY[i]);

			}
		if(index>0){
			averageX = averageX/index;
			cout <<"averageX" <<averageX <<"\n";
			averageY = averageY/(index);
			cout <<"averageY" <<averageY <<"\n";		
			}
	

		if(!stop_control)
			{

			if(difftime(currenttime,previoustime)>2)
				{
				if((averageX-tempavgX)<4 &&(averageY-tempavgY)<4)
					{
					previoustime=currenttime;

					SetCursorPos( (int) (1+width/290)*(290-(tempavgX)),(int) (height/210)*(tempavgY));
					cout << "I AM HERE" << endl;
					//Sleep(200);
					}

		

				else {
					SetCursorPos( (int) (1+width/290)*(290-(averageX)),(int) (height/210)*(averageY));
					}

				}

			else {
				SetCursorPos( (int) (1+width/290)*(290-(averageX)),(int) (height/210)*(averageY));
				}	
			}
	
		
		tempavgX=averageX;
		tempavgY=averageY;
		averageX=averageY=0;
	counter++;
	cout<<counter<<endl;
	
	}
	

void display() {


	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	

	//if(!initBool){

	//	ppp.EnableGesture();
	//	ppp.Init();	
	//	//fillzero();
	//	}

	//initBool=TRUE;
	//ppp.AcquireFrame(true);


	//GetDesktopResolution(width,height);

	//ppp.QueryGesture()->QueryNodeData(0,PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY|PXCGesture::GeoNode::LABEL_FINGER_INDEX, &Index_data);
	//x_N=Index_data.positionImage.x;
	//y_N=Index_data.positionImage.y;
	//currenttime=time(NULL);
	//if(x_N>290)
	//	{
	//	x_N=290;
	//	}


	//if(y_N>210)
	//	{
	//	y_N=210;
	//	}

	//if(x_N<10)
	//	{
	//	x_N=0;
	//	}


	//if(y_N<10)
	//	{
	//	y_N=0;
	//	}


	//if(x_N!=0 && y_N!=0)
	//	if(index>=size_smoother)
	//		{
	//		for(int j=0;j<index-1;j++)
	//			{
	//			avgX[j]=avgX[j+1];
	//			avgY[j]=avgY[j+1];
	//			}
	//		avgX[index-1]=x_N;
	//		avgY[index-1]=y_N;
	//		cout<<"avgY- "<<avgY[index-1]<<" "<<index-1<<"\n";
	//		}
	//	else{
	//		avgX[index]=x_N;
	//		avgY[index]=y_N;
	//		cout<<avgX[index]<<"\n";
	//		index++;
	//		}


	//	for(int j=0;j<index;j++)
	//		cout<<avgX[j]<<" ";
	//	cout<<endl;
	//	for(i=0;i<index;i++)

	//		{
	//		averageX=(averageX+avgX[i]);

	//		averageY=(averageY+avgY[i]);

	//		}
	//	if(index>0){
	//		averageX = averageX/index;
	//		cout <<"averageX" <<averageX <<"\n";
	//		averageY = averageY/(index);
	//		cout <<"averageY" <<averageY <<"\n";		
	//		}
	//

	//	if(!stop_control)
	//		{

	//		if(difftime(currenttime,previoustime)>2)
	//			{
	//			if((averageX-tempavgX)<4 &&(averageY-tempavgY)<4)
	//				{
	//				previoustime=currenttime;

	//				SetCursorPos( (int) (1+width/290)*(290-(tempavgX)),(int) (height/210)*(tempavgY));
	//				cout << "I AM HERE" << endl;
	//				//Sleep(200);
	//				}

	//			//if(difftime(currenttime,previoustime)>6)
	//			//	if((averageX-tempavgX)<3)
	//			//	if((averageY-tempavgY)<3)
	//			//		{
	//			//		stop_control=TRUE;
	//			//		previoustime=currenttime;
	//			//		
	//			//		}

	//			else {
	//				SetCursorPos( (int) (1+width/290)*(290-(averageX)),(int) (height/210)*(averageY));
	//				}

	//			}

	//		else {
	//			SetCursorPos( (int) (1+width/290)*(290-(averageX)),(int) (height/210)*(averageY));
	//			}	
	//		}
	//
	//	
	//	tempavgX=averageX;
	//	tempavgY=averageY;
	//	averageX=averageY=0;


		ppp.QueryGesture()->QueryNodeData(0,PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY|PXCGesture::GeoNode::LABEL_FINGER_THUMB, 5, hand_data);
		ppp.QueryGesture()->QueryNodeData(0,PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, &thumb_data);
		x2=320-thumb_data.positionImage.x;
		y2=240-thumb_data.positionImage.y;
		ppp.ReleaseFrame();
		glLineWidth(3);
		glEnable(GL_LINE_SMOOTH);
		glColor3f (.9, 0.1, 0);
		glBegin(GL_LINES);
		for(int i=0;i<5;i++)
			{
			x1=320- hand_data[i].positionImage.x;
			y3=240- hand_data[i].positionImage.y;
		

			if((hand_data[i].positionImage.x !=0 && hand_data[i].positionImage.y !=0) && (x2!=320 && y2!=240) && tracking==1)
				{
				glVertex3f (x2,y2 , 0.0);
				glVertex3f (x1, y3, 0.0);
				}

			}
		glEnd();
		glFlush ();

		ppp.QueryGesture()->QueryNodeData(0,PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY|PXCGesture::GeoNode::LABEL_FINGER_THUMB, 5, hand_data);
		ppp.QueryGesture()->QueryNodeData(0,PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY, &thumb_data);
		x2=320-thumb_data.positionImage.x;
		y2=240-thumb_data.positionImage.y;
		ppp.ReleaseFrame();


		if(tracking==0)
			{
			glPointSize( 70.0 );
			glColor3f( .6, .6, 0 );

			glBegin( GL_POINTS );
			glVertex2f( 0,235 );

			glEnd();
			glFlush ();

			glutSwapBuffers();

			}



		if(tracking==1)
			{
			glPointSize( 70.0 );
			glColor3f( 0, 1, 0 );

			glBegin( GL_POINTS );
			glVertex2f( 0,235 );

			glEnd();
			glFlush ();

			glutSwapBuffers();

			}
		if(tracking==2)
			{
			glPointSize( 70.0 );
			glColor3f( 1, 0, 0 );

			glBegin( GL_POINTS );
			glVertex2f( 0,235 );

			glEnd();
			glFlush ();

			glutSwapBuffers();

			}
		glColor3f (.5, 0.5, .2);
		glBegin(GL_LINES);
		for(int i=0;i<5;i++)
			{
			x1=320- hand_data[i].positionImage.x;
			y3=240- hand_data[i].positionImage.y;
		

			if((hand_data[i].positionImage.x !=0 && hand_data[i].positionImage.y !=0) && (x2!=320 && y2!=240) && tracking==1)

				{
				glVertex3f (x2,y2 , 0.0);
				glVertex3f (x1, y3, 0.0);
				}

			}
		glEnd();
		glFlush ();
		//outputFile.close();
		glutSwapBuffers();
		glutPostRedisplay();

	}



int main(int argc, char** argv)

	{
	glutInit(&argc, argv);
	
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (300, 300);
	glutInitWindowPosition (380, 380);
	glutCreateWindow ("HandSkeleton");
	init ();
	outputFile.open("FingerCoords.txt");
	glutDisplayFunc(display);
	glutIdleFunc(mousemover);
	glutMainLoop();
	//outputFile.close();
	}

































