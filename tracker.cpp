#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <dirent.h>
using namespace cv;
using namespace std;
 
int main()
{
  //Reading each file from the folder images.
  vector<String> filein;
  glob("images/*.png", filein);
  //Creating output stream to store the output in intercepts.csv file
  ofstream fileout;
  fileout.open("intercepts.csv",ios::out);
//For each file read using glob we perform tracking
double xprevlow=1100,xprevhigh=2400, mreferl=0.6;
double xprevlowr=-200,xprevhighr=500, mreferr=-0.6;
int count=0, countr=0,countl=0;
//for(int w=601;w<642; w++)
for(int w=0;w<filein.size(); w++)
{
  if(count==3)// If we dont find coordinates for more than 3 frames then reset 
  {
  xprevlow=1500,xprevhigh=3000, mreferl= 0.7;
  xprevlowr=-1000,xprevhighr=400, mreferr= -0.5;
  count=0;
}
if(countr==3)// If we dont find coordinate Right for more than 3 frames: Reset
{xprevlowr=-1000,xprevhighr=400, mreferr=-0.7;countr=0;}
if(countl==3)// If we dont find coordinate Right for more than 3 frames: Reset
{xprevlow=1000,xprevhigh=1800, mreferl=0.7;countl=0;}
  cout<<"Image:"<<filein[w]<<endl;
  //Initializing mat files for storing images.
  Mat im;
  Mat image;
  // Creating 2 mat to hold images.
  im = imread( filein[w], 1 );
  Size s= im.size();// Size of the image
  int rows= s.height;
  int cols= s.width;
  cout<< rows<< "\t"<< cols<<endl;
  if (im.empty())
  {
    cout << "Cannot open image!" << endl;
    return -1;
  }
  //Region of interest:
  //From the dataset, it is recognized that only bottom half of the image has majority information
  //Which is relevant.
  Rect regionOfInterest= Rect(0, 600, 1600, 600);
  Mat image2= im(regionOfInterest);
  if (image2.empty())
  {
    cout << "Cannot open image!" << endl;
    return -1;
  }
  //Applying gaussian blur: to reduce noise
  Mat im_gaus;
  GaussianBlur(image2,im_gaus,Size(7,7),0,0);
  //Applying Canny edge detector: to detect edge.
  //These edes would be used in determining lines.
  Mat cannyedge, edge;
  Canny(im_gaus, cannyedge, 70, 100, 3);
  cannyedge.convertTo(edge, CV_8U);
  //imshow("Canny Edge", edge);
  waitKey(1000);
  if (edge.empty())
  {
    cout << "Cannot open image!" << endl;
    return -1;
  }
  //Applying Hough probablistic trasform on each image whose edges are detected.
  Mat hough(image2.size(),CV_8U,Scalar(0));
  image2.copyTo(hough); 
  vector<Vec4i> lines;//Vector of lines
  HoughLinesP(edge, lines, 1, CV_PI/180, 50, 50, 5 );//This implements Hough Trasform.
  //Lines extracted from hough are stored in lines vector in the above line of code.
  double m, b, xtop, ytop=0, xbottom, ybottom=(599), ydiff, xdiff, referr=xprevlowr,referl=xprevhigh;
  double  yprev, slopel,sloper;
  double xl1, xl2=-4000,yl1,yl2,xr1, xr2=-4000,yr1,yr2;
  bool flagr=false, flagl=false;
 
  //For each line detected by hough P Transform we do the following:
 
  for( size_t i = 0; i < lines.size(); i++ )
  {
    //1. Describe the line obtained in 4 points x1,y1,x2,y2 in vector l.
    Vec4i l = lines[i];
    //2.Calculate Slope of the line
    ydiff= (l[3]-l[1]);
    xdiff= (l[2]-l[0]);
    m= ydiff/xdiff;//Slope
    //3.Intercept of the line
    

    b= l[1]- m*l[0];
    //Calculate xtop coordinate on the line when y=0
    ytop=0;
    xtop= -(b)/m;// Equation of line y=mx+b.Rearranging that for y=0 gives this
    //Calculate xbottom coordinate when y=bottom i.e. height-1
    xbottom= (ybottom-b)/m;
    
     //Find right lane marker
    if(xbottom>=xprevlowr && xbottom<xprevhighr)
    {
      cout<<"xprevlowr:"<<xprevlowr<<endl;
      cout<<"xprevhighr:"<<xprevhighr<<endl;
      cout<<"xbottom:"<<xbottom<<endl;
      cout<<"sloper:"<<m<<endl;
      cout<<"referr:"<<referr<<endl;
      cout<<"mreferr:"<<mreferr<<endl;
      if(xbottom>referr && m < mreferr+0.1 && m > mreferr-0.25)
      {  
        referr= xbottom;
        xr1=xtop;
        yr1=ytop;
        xr2=xbottom;
        yr2=ybottom;
        flagr=true;
        sloper=m;
      }
      
    }
    //Find left lane marker
    
    if(xbottom>=xprevlow && xbottom<=xprevhigh)
    {// Filter few lines
     
      if(xbottom<referl && m< mreferl+0.25 && m> mreferl-0.1)
      {  // Filter lines as per slope
        referl= xbottom;
        xl1=xtop;
        yl1=ytop;
        xl2=xbottom;
        yl2=ybottom;

        flagl=true;
        slopel=m;
        cout<<"slopel:"<<m<<endl;
      }

      
    } 
  }
  //If we succeed in finding right or left lane marker, we draw it. 
  if(flagr==true)
      {line( im, Point(xr1,yr1+600 ), Point(xr2,yr2+600), Scalar(0,255,255),  2, CV_AA);
        
        xprevlowr=xr2-100;
        xprevhighr=xr2+200;
        mreferr=sloper;
      }

  if(flagl==true)
      {
     //If line found for left lane
        line( im, Point(xl1,yl1+600 ), Point(xl2,yl2+600), Scalar(0,255,255),  2, CV_AA);
        //Draw line and update reference variables
        xprevlow=xl2-100;
        xprevhigh=xl2+200;
        mreferl=slopel;
      }
      // Updating intercept.csv file.
      if(xr2!=-4000 && xl2!=-4000)
        fileout<<filein[w]<<","<<xr2<<","<<xl2<<endl;
      else if(xr2==-4000 && xl2==-4000)
        {
          fileout<<filein[w]<<","<<"NONE"<<","<<"NONE"<<endl;
          count++;
        }
      else if(xr2!=-4000)
        {
          fileout<<filein[w]<<","<<xr2<<","<<"NONE"<<endl;
          countl++;
    }
      else if(xr1!=-4000)
        {fileout<<filein[w]<<","<<"NONE"<<","<<xl2<<endl;
      countr++;
    }
     
  if (im.empty())
  {
    cout << "Cannot open image hough!" << endl;
    return -1;
  }
//Display images with right and left lane markers in yellow.

Size size(800,600);//the dst image size,e.g.100x100
Mat dst;
// Reducing size of the image for better visibility.
resize(im,dst,size);//resize image
imshow("detected lines", dst);
waitKey(2000);
 //}
}
fileout.close();
waitKey(0);
  return 0;
}
