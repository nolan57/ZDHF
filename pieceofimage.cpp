#include "pieceofimage.h"

void PieceOfImage::setKeyPoints(KeyPoints iKeyPoints){
    keyPoints = iKeyPoints;
}

KeyPoints PieceOfImage::getKeyPoints(){
    if(keyPoints.circleScopes.size() < 4){
        std::cout << "error,circleScopes size < 4";
    }
    return keyPoints;
}
