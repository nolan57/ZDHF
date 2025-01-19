#ifndef TOOLS_H
#define TOOLS_H
#include <pieceofimage.h>

class SizeOfPartition{
public:
    SizeOfPartition(){
        col = 0;
        row = 0;
        width = 0;
        height = 0;
        lta = 0;
        keyPoints = KeyPoints();
    };
    int col;
    int row;
    int width;
    int height;
    int lta;
    KeyPoints keyPoints;
};
struct DatasToCombine{
public:
    int smallImageWidth = 0;
    int smallImageHeight = 0;
    int originalImageWidth = 0;
    int originalImageHeight = 0;
};

class Tools
{
private:
    DatasToCombine datasToCombine;
public:
    Tools();
    int randCode();
    std::vector<PieceOfImage> toSideCode(std::vector<PieceOfImage> images,const int cols);
    void toTestSideCode(const int rows,const int cols);
    std::vector<PieceOfImage> toPartition(const cv::Mat image,int rows,int cols);
    std::string setType(Location loc,int rows,int cols);
    KeyPoints setCircleScopes(KeyPoints keyPoints,int lta,std::map<std::string,int> sidesCode);
    SizeOfPartition setSizeOfPartition(const std::string type,
                                       const std::map<std::string,int> sidesCode,
                                       const int col,const int row,
                                       const int samllWidth,const int smallHeight,
                                       const int cols,int rows);
    bool inScope(cv::Point point,RectangleScope scope);
    bool inScope(cv::Point point,CircleScope scope);
    int inScopes(cv::Point point,std::vector<CircleScope> circleScopes);
    void toShapeImage(PieceOfImage &image);
    cv::Mat addAlphaToImage(cv::Mat &inputImage);
    cv::Mat toCombineShapedImages(std::vector<PieceOfImage> shapedImages);
    DatasToCombine getDatasToCombine();
};

#endif // TOOLS_H
