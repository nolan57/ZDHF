#ifndef PIECEOFIMAGE_H
#define PIECEOFIMAGE_H
#include <vector>
#include <core/mat.hpp>
#include <opencv.hpp>

class RectangleScope{
public:
    RectangleScope(){
        ltp = cv::Point(0,0);
        rtp = cv::Point(0,0);
        rbp = cv::Point(0,0);
        lbp = cv::Point(0,0);
        width = 0;
        height = 0;
    };
    cv::Point ltp;
    cv::Point rtp;
    cv::Point rbp;
    cv::Point lbp;
    //int getWidth(){return rtp.x-ltp.x;}
    //int getHeight(){return rbp.y-ltp.y;}
    int width;
    int height;
};

class CircleScope{
public:
    CircleScope(){
        center = cv::Point(0,0);
        r = 0;
        code = 0;
    };
    cv::Point center;
    int r;
    int code;
};

struct KeyPoints{
    RectangleScope externalScope=RectangleScope();
    RectangleScope internalScope=RectangleScope();
    std::vector<CircleScope> circleScopes=std::vector<CircleScope>();
};

class Location
{
public:
    Location(){
        row = 0;
        col = 0;
    }
    int row;
    int col;
};


class SizeOfImage
{
public:
    SizeOfImage():width(0),height(0){}
    int width;
    int height;
};

class PieceOfImage
{
public:
    PieceOfImage(){sizeOfSmallImage = SizeOfImage();sizeOfShapedImage = SizeOfImage();}
    Location location = Location();
    std::map<std::string,int> sidesCode={{"左边",0},{"上边",0},{"右边",0},{"下边",0}};
    SizeOfImage sizeOfSmallImage;
    SizeOfImage sizeOfShapedImage;
    std::string type="左上角";
    cv::Mat smallImage;
    int r=0;
    cv::Mat shapedImage;
    KeyPoints getKeyPoints();
    void setKeyPoints(KeyPoints keyPoints);
private:
    KeyPoints keyPoints;
};




class ImageToTest{
public:
    ImageToTest(){};
    Location location=Location();
    std::map<std::string,int> sidesCode={{"左边",0},{"上边",0},{"右边",0},{"下边",0}};
    std::string type;
};

#endif // PIECEOFIMAGE_H
