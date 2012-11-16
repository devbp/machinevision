#include"feature.h"

int main(int argc, char** argv)
{
    
     if(argc < 2)
      usage(argv[0]);
int featuremethod=1;
int matchmethod=1;
int knn=10;
int radius=15;
   
Mat img1;
Mat img2;    
parseArgs(argc,argv,featuremethod,matchmethod,radius,knn,img1,img2);
      
 //keypoints for two different images
    vector<KeyPoint> key_points1;
    vector<KeyPoint> key_points2;
 //descriptiors for two different images
    Mat descriptors1;
    Mat descriptors2;
    Mat img_matches;
    std::vector< DMatch > good_matches;
	 //vector to store the matched features
    std::vector< DMatch > matches;
    //finding keypoints and descriptors of the images
  findfeatures(img1,key_points1,descriptors1,featuremethod);
  findfeatures(img2,key_points2,descriptors2,featuremethod);
     Mat output_img1;
     
  //draw the keypoints of the image
    drawKeypoints(img1, key_points1, output_img1);
    namedWindow("KeypointsImage1");
    imshow("KeypointsImage1", output_img1);
  //match the features
    matchfeatures(descriptors1,descriptors2,matches,knn,radius,matchmethod);


//draw the matching images
 drawMatches( img1, key_points1, img2, key_points2,matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    namedWindow("MatchedImage before filtering");
    imshow("MatchedImage before filtering", img_matches);
//save the image    
cv::imwrite("result1.jpg", img_matches);
//find the best matches  
filtermatches(descriptors1,matches,good_matches);

//draw after filtering.
           drawMatches( img1, key_points1, img2, key_points2,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
     namedWindow("BestMatchedImage");
     imshow("BestMatchedImage", img_matches);
    //save the image
     cv::imwrite("result2.jpg", img_matches);
     waitKey(0);
     destroyWindow("Image");
    descriptors1.release();
    descriptors2.release();
    img_matches.release();

    return 0;
}
