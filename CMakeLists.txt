cmake_minimum_required (VERSION 2.6)
PROJECT(feature)
FIND_PACKAGE( OpenCV REQUIRED )
ADD_EXECUTABLE( featurematching featurematching1.cpp )
TARGET_LINK_LIBRARIES( featurematching ${OpenCV_LIBS} )






