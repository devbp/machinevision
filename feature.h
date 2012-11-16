#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
//#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/legacy.hpp> //(BruteForceMatcher is defined here
#include <opencv2/nonfree/features2d.hpp>
#include<getopt.h>

using namespace cv;
using namespace std;

extern char *optarg;
extern int optind;


void usage(char *prog)
{
#ifndef _MSC_VER
  const string bold("\033[1m");
  const string normal("\033[m");
#else
  const string bold("");
  const string normal("");
#endif
  cout << endl
    << bold << "USAGE " << normal << endl
    << "   " << prog << " [options] queryimage sourceimage" << endl << endl;
  cout << bold << "OPTIONS" << normal << endl

    << bold << "  -f" << normal << " NR, " << bold << "--featuremethod=" << normal << "NR   [default: 1]" << endl
    << "         selects the feature extraction method for the images" << endl
    << "           1 = SIFT features" << endl
    << "           2 = SURF features. " << endl
    << bold << "  -m" << normal << " NR, " << bold << "--matchmethod=" << normal << "NR   [default: 1]" << endl
    << "         selects the feature extraction method for the images" << endl
    << "           1 = FLANN BASED MATCH" << endl
    << "           2 = FLANN BASED KNN MATCH METHOD"<<endl
    << "           3 = FLANN BASED FIXED RADIUS" << endl
    << "           4 = BRUTE FORCE METHOD"<<endl
    << bold << "  -r" << normal << " NR, " << bold << "--radius=" << normal << "NR   [default: 20]" << endl
    << "         radius value for FIXED RADIUS METHOD" << endl
     << bold << "  -k" << normal << " NR, " << bold << "--knn=" << normal << "NR   [default: 20]" << endl
    << "         k represents number of nearest neighbours for KNN MATCH METHOD" << endl<<endl

    <<"Example :  ./featurematching -f 1 -m 4 queryimage.jpg sourceimage.jpg"<<endl
    <<"Example :  ./featurematching -f 1 -m 1 queryimage.jpg sourceimage.jpg"<<endl
    <<"Example :  ./featurematching -f 2 -m 2 queryimage.jpg sourceimage.jpg"<<endl;

}  //end of usage function


int parseArgs(int argc, char **argv, int &featuremethod, int &matchmethod, int &radius, int &knn, Mat &img1, Mat &img2)
{

static struct option longopts[]={
                                  
{ "featuremethod",          required_argument,   0,  'f' },
{ "matchmethod",            required_argument,   0,  'm' },
{ "radius",                 required_argument,   0,  'r' },
{ "knn",                    required_argument,   0,  'k' },
{ 0,           0,   0,   0} 
                                 };
int c;

while ((c = getopt_long(argc, argv, "f:m:r:k", longopts, NULL)) != -1)
 {
     switch(c)
       { case 'c': 
          featuremethod=atoi(optarg);
            break;

         case 'm':
          matchmethod =atoi(optarg);
            break;

         case 'r':
         radius=atoi(optarg);
           break;


          case 'k':
          knn=atoi(optarg);
           break;
         }//end of choice

  }//end of while

/*if (optind != 3) {
    cerr << "\n*** Images missing ***" << endl;
    usage(argv[0]);
  }*/

img1=imread(argv[optind],0);//query image in img1 0 for grayscale
img2=imread(argv[optind+1],0); 

return 0;
} //end of parsearg function


void findfeatures(Mat &img, vector<KeyPoint> &key_points,Mat &descriptors,int featuremethod)
{
if(featuremethod==1)
  {
cout<<"SIFT features extracting..."<<endl;
SiftFeatureDetector detector(100);
detector.detect(img,key_points);
SiftDescriptorExtractor extractor;
extractor.compute(img,key_points,descriptors);
/***optionally we can use this constructor also****/
  // sift(img, Mat(), key_points, descriptors);
  }


// finding keypoints and descriptors using surf feature extractor
 
 if(featuremethod==2)
 {
cout<<"SURF features extracting.."<<endl;
SurfFeatureDetector detector(100);
detector.detect(img,key_points);
SurfDescriptorExtractor extractor;
extractor.compute(img,key_points,descriptors);
 //surf(img, Mat(), key_points, descriptors);
   } 

}  //end of function findfeature.


void matchfeatures(Mat &descriptors1, Mat &descriptors2, vector< DMatch > &matches, int &knn, int &radius, int matchmethod)
{

vector<vector<cv::DMatch> > MatchesVector;

if(matchmethod==1)
     {
   cout<<"Flann Based Matching"<<endl;
    FlannBasedMatcher matcher;
    matcher.match( descriptors1, descriptors2, matches);
    }
 
 if(matchmethod==2) 
  {
   cout<<"Flann Based KNN Matching"<<endl;
   FlannBasedMatcher matcher;
   matcher.knnMatch(descriptors1, descriptors2, MatchesVector, knn);

    for(unsigned int i = 0; i < MatchesVector.size(); i++){
	for(unsigned int k = 0; k < MatchesVector[i].size(); k++){
	  matches.push_back(MatchesVector[i][k]);
	}
      }
    }

//fixed radius based matching method
 if(matchmethod==3)
   {
 cout<<"Flann Based Radius Matching"<<endl;
    FlannBasedMatcher matcher;
    matcher.radiusMatch(descriptors1,descriptors2, MatchesVector, radius);
    for(unsigned int i = 0; i < MatchesVector.size(); i++){
	for(unsigned int j = 0; j < MatchesVector[i].size(); j++){
	  matches.push_back(MatchesVector[i][j]);
	}
      }
   
  }

 if(matchmethod==4)
  {
  cout<<"Brute Force Matching"<<endl;
 cv::BruteForceMatcher< L2<float> > matcher;
 matcher.match( descriptors1, descriptors2, matches);
 }
    
  
}//end of the match function



void filtermatches(Mat &descriptors1,vector< DMatch > &matches,vector<DMatch> &good_matches)
{

double max_dist = 0; double min_dist = 100000;

 for( int i = 0; i < descriptors1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }
  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );
    
//finding the good matches
  for( int i = 0; i < descriptors1.rows; i++)
  { if( matches[i].distance < 4*min_dist )
    { good_matches.push_back( matches[i]); }
  }
for( int i = 0; i < good_matches.size(); i++ )
  { 
cout<<"Good Matches "<<" "<<i<<"Keypoint1 "<<good_matches[i].queryIdx<<" "<<"Keypoint2 "<<good_matches[i].trainIdx<<endl;
 }
 

}//end of filtermatches function

