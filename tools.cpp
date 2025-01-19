#include "tools.H"
#include "createalpha.h"
#include <cstdlib>

Tools::Tools() {
}
int Tools::randCode() {
  srand(time(0));
  // 生成一个在-1到1之间的随机数并输出
  int randomNumber;
  do {
    // randomNumber = arc4random() % 3 - 1;
    randomNumber = rand() % 3 - 1;
  } while (randomNumber == 0);
  return randomNumber;
}
std::vector<PieceOfImage> Tools::toSideCode(std::vector<PieceOfImage> images,
                                            const int cols) {
  // std::cout<<"size of images"<<images.size()<<"\n";
  for (int i = 0; i < images.size(); i++) {
    std::string type = images[i].type;
    if (type == "左上角") {
      images[i].sidesCode.at("左边") = 0;
      images[i].sidesCode.at("上边") = 0;
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "右上角") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = 0;
      images[i].sidesCode.at("右边") = 0;
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "右下角") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = 0;
      images[i].sidesCode.at("下边") = 0;
      continue;
    }
    if (type == "左下角") {
      images[i].sidesCode.at("左边") = 0;
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = 0;
      continue;
    }

    if (type == "左边") {
      images[i].sidesCode.at("左边") = 0;
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "上边") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = 0;
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "右边") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = 0;
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "下边") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = 0;
      continue;
    }
    images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
    images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
    images[i].sidesCode.at("右边") = randCode();
    images[i].sidesCode.at("下边") = randCode();
  }
  return images;
}

std::string Tools::setType(const Location loc, const int rows, const int cols) {

  if (loc.row == 0 && loc.col == 0) {
    return "左上角";
  }
  if (loc.row == 0 && loc.col == cols - 1) {
    return "右上角";
  }
  if (loc.row == rows - 1 && loc.col == cols - 1) {
    return "右下角";
  }
  if (loc.row == rows - 1 && loc.col == 0) {
    return "左下角";
  }
  if (loc.col == 0) {
    return "左边";
  }
  if (loc.col == cols - 1) {
    return "右边";
  }
  if (loc.row == 0) {
    return "上边";
  }
  if (loc.col == cols - 1) {
    return "下边";
  }
  return "其他";
}

KeyPoints Tools::setCircleScopes(KeyPoints keyPoints, int lta,
                                 std::map<std::string, int> sidesCode) {

  CircleScope circleScope = CircleScope();

  circleScope.code = sidesCode.at("上边");
  circleScope.center =
      (keyPoints.internalScope.ltp + keyPoints.internalScope.rtp) / 2;
  circleScope.r = lta;
  keyPoints.circleScopes.push_back(circleScope);

  circleScope.code = sidesCode.at("右边");
  circleScope.center =
      (keyPoints.internalScope.rtp + keyPoints.internalScope.rbp) / 2;
  circleScope.r = lta;
  keyPoints.circleScopes.push_back(circleScope);

  circleScope.code = sidesCode.at("下边");
  circleScope.center =
      (keyPoints.internalScope.lbp + keyPoints.internalScope.rbp) / 2;
  circleScope.r = lta;
  keyPoints.circleScopes.push_back(circleScope);

  circleScope.code = sidesCode.at("左边");
  circleScope.center =
      (keyPoints.internalScope.ltp + keyPoints.internalScope.lbp) / 2;
  circleScope.r = lta;
  keyPoints.circleScopes.push_back(circleScope);

  // keyPoints.circleScopes[0].code = sidesCode.at("上边");
  // keyPoints.circleScopes[0].center =
  // (keyPoints.internalScope.ltp+keyPoints.internalScope.rtp)/2;
  // //keyPoints.circleScopes[0].r = keyPoints.internalScope.getHeight()/4;
  // keyPoints.circleScopes[0].r = lta;
  // std::cout << "上边圆圆心:" << keyPoints.circleScopes[0].center<<"\n";
  // std::cout << "右边圆圆心:" << keyPoints.circleScopes[1].center<<"\n";
  // std::cout << "下边圆圆心:" << keyPoints.circleScopes[2].center<<"\n";
  // std::cout << "左边圆圆心:" << keyPoints.circleScopes[3].center<<"\n";
  //
  // std::cout << "图片大小:" << keyPoints.externalScope.width <<","
  // <<keyPoints.externalScope.height<<"\n"; std::cout << "外矩形:" <<
  // keyPoints.externalScope.ltp<<","<<keyPoints.externalScope.rbp<<"\n";
  // std::cout<<"外矩形大小"
  // <<keyPoints.externalScope.width<<","<<keyPoints.externalScope.height<<"\n";
  // std::cout << "内矩形:" <<
  // keyPoints.internalScope.ltp<<","<<keyPoints.internalScope.rbp<<"\n";
  // std::cout << "内矩形大小:" <<
  // keyPoints.internalScope.width<<","<<keyPoints.internalScope.height<<"\n";

  return keyPoints;
}

SizeOfPartition Tools::setSizeOfPartition(
    const std::string type, const std::map<std::string, int> sidesCode,
    const int col, const int row, const int smallWidth, const int smallHeight,
    const int cols, const int rows) {
  SizeOfPartition sizeOfpartition = SizeOfPartition();
  KeyPoints keyPoints = KeyPoints();
  RectangleScope externalScope = RectangleScope();
  RectangleScope internalScop = RectangleScope();
  // CircleScope circleScope = CircleScope();

  sizeOfpartition.col = col;
  sizeOfpartition.row = row;
  sizeOfpartition.width = smallWidth;
  sizeOfpartition.height = smallHeight;
  int lta = std::min(smallWidth, smallHeight) / 4;
  sizeOfpartition.lta = lta;

  // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
  // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
  // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")->";
  // std::cin.get();

  internalScop.ltp = cv::Point(0, 0);
  internalScop.rtp = cv::Point(0, 0) + cv::Point(smallWidth, 0);
  internalScop.rbp = cv::Point(0, 0) + cv::Point(smallWidth, smallHeight);
  internalScop.lbp = cv::Point(0, 0) + cv::Point(0, smallHeight);
  internalScop.width = smallWidth;
  internalScop.height = smallHeight;

  if (type == "左上角") {

    if (sidesCode.at("右边") == 1) {
      sizeOfpartition.width = smallWidth + lta; // smallWidth/4;
    }
    if (sidesCode.at("下边") == 1) {
      sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
    }
    if (sizeOfpartition.col + sizeOfpartition.width > cols) {
      sizeOfpartition.width =
          sizeOfpartition.width -
          (sizeOfpartition.col + sizeOfpartition.width - cols);
    }
    if (sizeOfpartition.row + sizeOfpartition.height > rows) {
      sizeOfpartition.height =
          sizeOfpartition.height -
          (sizeOfpartition.row + sizeOfpartition.height - rows);
    }

    externalScope.ltp = cv::Point(0, 0);
    externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
    externalScope.rbp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width,
                                                    sizeOfpartition.height);
    externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
    externalScope.width = sizeOfpartition.width;
    externalScope.height = sizeOfpartition.height;

    keyPoints.externalScope = externalScope;
    keyPoints.internalScope = internalScop;
    sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

    // std::cout<<1;
    // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
    // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
    // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
    // std::cin.get();
    return sizeOfpartition;
  }

  if (type == "右上角") {

    if (sidesCode.at("左边") == 1) {
      sizeOfpartition.col = col - lta; // smallWidth/4;

      internalScop.ltp = internalScop.ltp + cv::Point(lta, 0);
      internalScop.rtp = internalScop.rtp + cv::Point(lta, 0);
      internalScop.rbp = internalScop.rbp + cv::Point(lta, 0);
      internalScop.lbp = internalScop.lbp + cv::Point(lta, 0);

      sizeOfpartition.width = smallWidth + lta; // smallWidth/4;
    }
    if (sidesCode.at("下边") == 1) {
      sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
    }
    if (sizeOfpartition.col + sizeOfpartition.width > cols) {
      sizeOfpartition.width =
          sizeOfpartition.width -
          (sizeOfpartition.col + sizeOfpartition.width - cols);
    }
    if (sizeOfpartition.row + sizeOfpartition.height > rows) {
      sizeOfpartition.height =
          sizeOfpartition.height -
          (sizeOfpartition.row + sizeOfpartition.height - rows);
    }

    externalScope.ltp = cv::Point(0, 0);
    externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
    externalScope.rbp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width,
                                                    sizeOfpartition.height);
    externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
    externalScope.width = sizeOfpartition.width;
    externalScope.height = sizeOfpartition.height;

    keyPoints.externalScope = externalScope;
    keyPoints.internalScope = internalScop;
    sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

    // std::cout<<3;
    // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
    // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
    // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
    // std::cin.get();
    return sizeOfpartition;
  }

  if (type == "右下角") {
    if (sidesCode.at("左边") == 1) {
      sizeOfpartition.col = col - lta; // smallWidth/4;

      internalScop.ltp = internalScop.ltp + cv::Point(lta, 0);
      internalScop.rtp = internalScop.rtp + cv::Point(lta, 0);
      internalScop.rbp = internalScop.rbp + cv::Point(lta, 0);
      internalScop.lbp = internalScop.lbp + cv::Point(lta, 0);

      sizeOfpartition.width = smallWidth + lta; // smallWidth/4;
    }
    if (sidesCode.at("上边") == 1) {
      sizeOfpartition.row = row - lta; // smallHeight/4;

      internalScop.ltp = internalScop.ltp + cv::Point(0, lta);
      internalScop.rtp = internalScop.rtp + cv::Point(0, lta);
      internalScop.rbp = internalScop.rbp + cv::Point(0, lta);
      internalScop.lbp = internalScop.lbp + cv::Point(0, lta);

      sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
    }
    if (sizeOfpartition.col + sizeOfpartition.width > cols) {
      sizeOfpartition.width =
          sizeOfpartition.width -
          (sizeOfpartition.col + sizeOfpartition.width - cols);
    }
    if (sizeOfpartition.row + sizeOfpartition.height > rows) {
      sizeOfpartition.height =
          sizeOfpartition.height -
          (sizeOfpartition.row + sizeOfpartition.height - rows);
    }

    externalScope.ltp = cv::Point(0, 0);
    externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
    externalScope.rbp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width,
                                                    sizeOfpartition.height);
    externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
    externalScope.width = sizeOfpartition.width;
    externalScope.height = sizeOfpartition.height;

    keyPoints.externalScope = externalScope;
    keyPoints.internalScope = internalScop;
    sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

    // std::cout<<5;
    // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
    // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
    // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
    // std::cin.get();
    return sizeOfpartition;
  }

  if (type == "左下角") {
    if (sidesCode.at("上边") == 1) {
      sizeOfpartition.row = row - lta; // smallHeight/4;

      internalScop.ltp = internalScop.ltp + cv::Point(0, lta);
      internalScop.rtp = internalScop.rtp + cv::Point(0, lta);
      internalScop.rbp = internalScop.rbp + cv::Point(0, lta);
      internalScop.lbp = internalScop.lbp + cv::Point(0, lta);

      sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
    }
    if (sidesCode.at("右边") == 1) {
      sizeOfpartition.width = smallWidth + lta; // smallWidth/4;
    }
    if (sizeOfpartition.col + sizeOfpartition.width > cols) {
      sizeOfpartition.width =
          sizeOfpartition.width -
          (sizeOfpartition.col + sizeOfpartition.width - cols);
    }
    if (sizeOfpartition.row + sizeOfpartition.height > rows) {
      sizeOfpartition.height =
          sizeOfpartition.height -
          (sizeOfpartition.row + sizeOfpartition.height - rows);
    }

    externalScope.ltp = cv::Point(0, 0);
    externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
    externalScope.rbp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width,
                                                    sizeOfpartition.height);
    externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
    externalScope.width = sizeOfpartition.width;
    externalScope.height = sizeOfpartition.height;

    keyPoints.externalScope = externalScope;
    keyPoints.internalScope = internalScop;
    sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

    // std::cout<<7;
    // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
    // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
    // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
    // std::cin.get();
    return sizeOfpartition;
  }

  if (type == "左边") {
    if (sidesCode.at("上边") == 1) {
      sizeOfpartition.row = row - lta; // smallHeight/4;

      internalScop.ltp = internalScop.ltp + cv::Point(0, lta);
      internalScop.rtp = internalScop.rtp + cv::Point(0, lta);
      internalScop.rbp = internalScop.rbp + cv::Point(0, lta);
      internalScop.lbp = internalScop.lbp + cv::Point(0, lta);

      sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
    }
    if (sidesCode.at("下边") == 1) {
      sizeOfpartition.height = sizeOfpartition.height + lta; // smallHeight/4;
    }
    if (sidesCode.at("右边") == 1) {
      sizeOfpartition.width = smallWidth + lta; // smallWidth / 4;
    }
    if (sizeOfpartition.col + sizeOfpartition.width > cols) {
      sizeOfpartition.width =
          sizeOfpartition.width -
          (sizeOfpartition.col + sizeOfpartition.width - cols);
    }
    if (sizeOfpartition.row + sizeOfpartition.height > rows) {
      sizeOfpartition.height =
          sizeOfpartition.height -
          (sizeOfpartition.row + sizeOfpartition.height - rows);
    }

    externalScope.ltp = cv::Point(0, 0);
    externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
    externalScope.rbp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width,
                                                    sizeOfpartition.height);
    externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
    externalScope.width = sizeOfpartition.width;
    externalScope.height = sizeOfpartition.height;

    keyPoints.externalScope = externalScope;
    keyPoints.internalScope = internalScop;
    sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

    // std::cout<<8;
    // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
    // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
    // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
    // std::cin.get();
    return sizeOfpartition;
  }
  if (type == "上边") {
    if (sidesCode.at("左边") == 1) {
      sizeOfpartition.col = col - lta; // smallWidth/4;

      internalScop.ltp = internalScop.ltp + cv::Point(lta, 0);
      internalScop.rtp = internalScop.rtp + cv::Point(lta, 0);
      internalScop.rbp = internalScop.rbp + cv::Point(lta, 0);
      internalScop.lbp = internalScop.lbp + cv::Point(lta, 0);

      sizeOfpartition.width = smallWidth + lta; // smallWidth/4;
    }
    if (sidesCode.at("右边") == 1) {
      sizeOfpartition.width = sizeOfpartition.width + lta; // smallWidth/4;
    }
    if (sidesCode.at("下边") == 1) {
      sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
    }
    if (sizeOfpartition.col + sizeOfpartition.width > cols) {
      sizeOfpartition.width =
          sizeOfpartition.width -
          (sizeOfpartition.col + sizeOfpartition.width - cols);
    }
    if (sizeOfpartition.row + sizeOfpartition.height > rows) {
      sizeOfpartition.height =
          sizeOfpartition.height -
          (sizeOfpartition.row + sizeOfpartition.height - rows);
    }

    externalScope.ltp = cv::Point(0, 0);
    externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
    externalScope.rbp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width,
                                                    sizeOfpartition.height);
    externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
    externalScope.width = sizeOfpartition.width;
    externalScope.height = sizeOfpartition.height;

    keyPoints.externalScope = externalScope;
    keyPoints.internalScope = internalScop;
    sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

    // std::cout<<2;
    // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
    // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
    // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
    // std::cin.get();
    return sizeOfpartition;
  }

  if (type == "右边") {
    if (sidesCode.at("上边") == 1) {
      sizeOfpartition.row = row - lta; // smallHeight/4;

      internalScop.ltp = internalScop.ltp + cv::Point(0, lta);
      internalScop.rtp = internalScop.rtp + cv::Point(0, lta);
      internalScop.rbp = internalScop.rbp + cv::Point(0, lta);
      internalScop.lbp = internalScop.lbp + cv::Point(0, lta);

      sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
    }
    if (sidesCode.at("左边") == 1) {
      sizeOfpartition.col = col - lta; // smallWidth/4;

      internalScop.ltp = internalScop.ltp + cv::Point(lta, 0);
      internalScop.rtp = internalScop.rtp + cv::Point(lta, 0);
      internalScop.rbp = internalScop.rbp + cv::Point(lta, 0);
      internalScop.lbp = internalScop.lbp + cv::Point(lta, 0);

      sizeOfpartition.width = smallWidth + lta; // smallWidth/4;
    }
    if (sidesCode.at("下边") == 1) {
      sizeOfpartition.height = sizeOfpartition.height + lta; // smallHeight/4;
    }
    if (sizeOfpartition.col + sizeOfpartition.width > cols) {
      sizeOfpartition.width =
          sizeOfpartition.width -
          (sizeOfpartition.col + sizeOfpartition.width - cols);
    }
    if (sizeOfpartition.row + sizeOfpartition.height > rows) {
      sizeOfpartition.height =
          sizeOfpartition.height -
          (sizeOfpartition.row + sizeOfpartition.height - rows);
    }

    externalScope.ltp = cv::Point(0, 0);
    externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
    externalScope.rbp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width,
                                                    sizeOfpartition.height);
    externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
    externalScope.width = sizeOfpartition.width;
    externalScope.height = sizeOfpartition.height;

    keyPoints.externalScope = externalScope;
    keyPoints.internalScope = internalScop;
    sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

    // std::cout<<3;
    // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
    // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
    // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
    // std::cin.get();
    return sizeOfpartition;
  }
  if (type == "下边") {
    if (sidesCode.at("左边") == 1) {
      sizeOfpartition.col = col - lta; // smallWidth/4;

      internalScop.ltp = internalScop.ltp + cv::Point(lta, 0);
      internalScop.rtp = internalScop.rtp + cv::Point(lta, 0);
      internalScop.rbp = internalScop.rbp + cv::Point(lta, 0);
      internalScop.lbp = internalScop.lbp + cv::Point(lta, 0);

      sizeOfpartition.width = smallWidth + lta; // smallWidth/4;
    }
    if (sidesCode.at("右边") == 1) {
      sizeOfpartition.width = sizeOfpartition.width + lta; // smallWidth/4;
    }
    if (sidesCode.at("上边") == 1) {
      sizeOfpartition.row = row - lta; // smallHeight/4;

      internalScop.ltp = internalScop.ltp + cv::Point(0, lta);
      internalScop.rtp = internalScop.rtp + cv::Point(0, lta);
      internalScop.rbp = internalScop.rbp + cv::Point(0, lta);
      internalScop.lbp = internalScop.lbp + cv::Point(0, lta);

      sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
    }
    if (sizeOfpartition.col + sizeOfpartition.width > cols) {
      sizeOfpartition.width =
          sizeOfpartition.width -
          (sizeOfpartition.col + sizeOfpartition.width - cols);
    }
    if (sizeOfpartition.row + sizeOfpartition.height > rows) {
      sizeOfpartition.height =
          sizeOfpartition.height -
          (sizeOfpartition.row + sizeOfpartition.height - rows);
    }

    externalScope.ltp = cv::Point(0, 0);
    externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
    externalScope.rbp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width,
                                                    sizeOfpartition.height);
    externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
    externalScope.width = sizeOfpartition.width;
    externalScope.height = sizeOfpartition.height;

    keyPoints.externalScope = externalScope;
    keyPoints.internalScope = internalScop;
    sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

    // std::cout<<6;
    // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
    // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
    // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
    // std::cin.get();
    return sizeOfpartition;
  }
  if (sidesCode.at("左边") == 1) {
    sizeOfpartition.col = col - lta; // smallWidth/4;

    internalScop.ltp = internalScop.ltp + cv::Point(lta, 0);
    internalScop.rtp = internalScop.rtp + cv::Point(lta, 0);
    internalScop.rbp = internalScop.rbp + cv::Point(lta, 0);
    internalScop.lbp = internalScop.lbp + cv::Point(lta, 0);

    sizeOfpartition.width = smallWidth + lta; // smallWidth/4;
  }
  if (sidesCode.at("上边") == 1) {
    sizeOfpartition.row = row - lta; // smallHeight/4;

    internalScop.ltp = internalScop.ltp + cv::Point(0, lta);
    internalScop.rtp = internalScop.rtp + cv::Point(0, lta);
    internalScop.rbp = internalScop.rbp + cv::Point(0, lta);
    internalScop.lbp = internalScop.lbp + cv::Point(0, lta);

    sizeOfpartition.height = smallHeight + lta; // smallHeight/4;
  }
  if (sidesCode.at("右边") == 1) {
    sizeOfpartition.width = sizeOfpartition.width + lta; // smallWidth/4;
  }
  if (sidesCode.at("下边") == 1) {
    sizeOfpartition.height = sizeOfpartition.height + lta; // smallHeight/4;
  }
  if (sizeOfpartition.col + sizeOfpartition.width > cols) {
    sizeOfpartition.width =
        sizeOfpartition.width -
        (sizeOfpartition.col + sizeOfpartition.width - cols);
  }
  if (sizeOfpartition.row + sizeOfpartition.height > rows) {
    sizeOfpartition.height =
        sizeOfpartition.height -
        (sizeOfpartition.row + sizeOfpartition.height - rows);
  }

  externalScope.ltp = cv::Point(0, 0);
  externalScope.rtp = cv::Point(0, 0) + cv::Point(sizeOfpartition.width, 0);
  externalScope.rbp = cv::Point(0, 0) +
                      cv::Point(sizeOfpartition.width, sizeOfpartition.height);
  externalScope.lbp = cv::Point(0, 0) + cv::Point(0, sizeOfpartition.height);
  externalScope.width = sizeOfpartition.width;
  externalScope.height = sizeOfpartition.height;

  keyPoints.externalScope = externalScope;
  keyPoints.internalScope = internalScop;
  sizeOfpartition.keyPoints = setCircleScopes(keyPoints, lta, sidesCode);

  // std::cout<<9;
  // std::cout<<"("<<sizeOfpartition.col<<","<<sizeOfpartition.row<<")";
  // std::cout<<"("<<sidesCode.at("左边")<<sidesCode.at("上边")<<sidesCode.at("右边")<<sidesCode.at("下边")<<")";
  // std::cout<<"("<<sizeOfpartition.width<<","<<sizeOfpartition.height<<")\n";
  // std::cin.get();
  return sizeOfpartition;
}

std::vector<PieceOfImage> Tools::toPartition(const cv::Mat originalImage,
                                             const int irows, const int icols) {
  datasToCombine = DatasToCombine();
  int rows = irows;
  int cols = icols;
  if(rows != cols){
      std::cout << "error:rows not equal to cols!\n";
      int less = std::min(rows,cols);
      rows = less;
      cols = less;
      std::cout << "now set to rows and cols to " << less <<"\n";
  }

  std::vector<PieceOfImage> images = std::vector<PieceOfImage>();
  SizeOfPartition sizeOfPartition = SizeOfPartition();
  int smallWidth = originalImage.cols / cols;
  int smallHeight = originalImage.rows / rows;

  if(smallWidth <100 || smallHeight <100){
      std::cout << "too many and too small pieces!\n";
      int less = std::min(originalImage.cols/100,originalImage.rows/100);
      rows = less;
      cols = less;
      smallWidth = originalImage.cols / cols;
      smallHeight = originalImage.rows / rows;
      std::cout << "set the width and height of pieces to at least "
                << std::min(smallWidth,smallHeight) << "\n";
      std::cout << "and now,the number of pieces is: "
                << less * less << "\n";
  }

  datasToCombine.originalImageHeight = originalImage.rows;
  datasToCombine.originalImageWidth = originalImage.cols;
  datasToCombine.smallImageHeight = smallHeight;
  datasToCombine.smallImageWidth = smallWidth;
  std::cout << "original image's rows:" << originalImage.rows << ";";
  std::cout << "original image's cols:" << originalImage.cols << "\n";
  std::cout << "small width:" << smallWidth << ",";
  std::cout << "small height:" << smallHeight << "\n";
  // std::cin.get();

  // 划分原始图片为N*M个小图片
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {

      PieceOfImage image = PieceOfImage();

      image.location.row = i;
      image.location.col = j;
      image.sizeOfSmallImage.width = smallWidth;
      image.sizeOfSmallImage.height = smallHeight;
      image.type = setType(image.location, rows, cols);
      cv::Rect roi(j * smallWidth, i * smallHeight, smallWidth, smallHeight);
      cv::Mat smallImage = originalImage(roi).clone();
      image.smallImage = smallImage;
      images.push_back(image);
    }
  }
  images = toSideCode(images, cols);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      PieceOfImage image = images[i * cols + j];
      std::string type = image.type;
      std::map<std::string, int> sidesCode = image.sidesCode;

      sizeOfPartition = setSizeOfPartition(
          type, sidesCode, j * smallWidth, i * smallHeight, smallWidth,
          smallHeight, originalImage.cols, originalImage.rows);
      images[i * cols + j].setKeyPoints(sizeOfPartition.keyPoints);
      images[i * cols + j].r = sizeOfPartition.lta;

      cv::Rect roi(sizeOfPartition.col, sizeOfPartition.row,
                   sizeOfPartition.width, sizeOfPartition.height);
      cv::Mat shapedImage = originalImage(roi).clone();
      images[i * cols + j].shapedImage = shapedImage;

      // 保存小图片
      // std::string filename = "small_image_" + std::to_string(i) + "_" +
      // std::to_string(j) + ".png"; cv::imwrite(filename, shapedImage);
    }
  }
  return images;
}

bool Tools::inScope(cv::Point point, RectangleScope scope) {
  // 使用叉乘的方法判断点是否在四边形内部
  int cross1 = (scope.rtp.x - scope.ltp.x) * (point.y - scope.ltp.y) -
               (scope.rtp.y - scope.ltp.y) * (point.x - scope.ltp.x);
  int cross2 = (scope.rbp.x - scope.rtp.x) * (point.y - scope.rtp.y) -
               (scope.rbp.y - scope.rtp.y) * (point.x - scope.rtp.x);
  int cross3 = (scope.lbp.x - scope.rbp.x) * (point.y - scope.rbp.y) -
               (scope.lbp.y - scope.rbp.y) * (point.x - scope.rbp.x);
  int cross4 = (scope.ltp.x - scope.lbp.x) * (point.y - scope.lbp.y) -
               (scope.ltp.y - scope.lbp.y) * (point.x - scope.lbp.x);

  if ((cross1 >= 0 && cross2 >= 0 && cross3 >= 0 && cross4 >= 0) ||
      (cross1 <= 0 && cross2 <= 0 && cross3 <= 0 && cross4 <= 0)) {
    return true;
  } else {
    return false;
  }
}
bool Tools::inScope(cv::Point point, CircleScope scope) {
  // 计算点 (x, y) 到圆心 (n, m) 的距离
  double distance =
      sqrt((point.x - scope.center.x) * (point.x - scope.center.x) +
           (point.y - scope.center.y) * (point.y - scope.center.y));

  // 如果距离小于等于半径 r，则点在圆内
  if (distance < scope.r) {
    return true;
  } else {
    return false;
  }
}

void Tools::toShapeImage(PieceOfImage &image) {
  cv::Mat ShapedImage = image.shapedImage.clone();
  KeyPoints keyPoints = image.getKeyPoints();

  // 将图像转换为BGRA格式
  cv::cvtColor(ShapedImage, ShapedImage, cv::COLOR_BGR2BGRA);

  for (int col = 0; col < image.shapedImage.cols; col++) {
    for (int row = 0; row < image.shapedImage.rows; row++) {
      cv::Point point = cv::Point(col, row);
      
      // 判断点是否在内矩形中
      if (inScope(point, keyPoints.internalScope)) {
        // 如果在内矩形中，则遍历所有圆
        for (CircleScope circleSope : keyPoints.circleScopes) {
          if (inScope(point, circleSope)) {
            // 判断是不是在空圆内
            if (circleSope.code == -1) {
              // 若是在空圆中，则设为完全透明
              ShapedImage.at<cv::Vec4b>(point.y, point.x)[3] = 0;
              break;
            }
          }
        }
      } else {
        // 如果不在内矩形内
        bool IsInACircle = false;
        // 则遍历所有圆
        for (CircleScope circleSope : keyPoints.circleScopes) {
          // 判断是不是在某个圆内
          if (inScope(point, circleSope)) {
            IsInACircle = true;
            // 判断是否不在实圆内
            if (circleSope.code == 0 || circleSope.code == -1) {
              // 不在实圆内，就设为完全透明
              ShapedImage.at<cv::Vec4b>(point.y, point.x)[3] = 0;
              break;
            }
          }
        }
        if (!IsInACircle) {
          // 如果不在任何圆内，设为完全透明
          ShapedImage.at<cv::Vec4b>(point.y, point.x)[3] = 0;
        }
      }
    }
  }
  image.shapedImage = ShapedImage.clone();
}

cv::Mat Tools::addAlphaToImage(cv::Mat &inputImage) {

  cv::Mat image_with_alpha = inputImage.clone();
  CreateAlpha createAlpha = CreateAlpha();

  cv::Mat alpha = createAlpha.createAlphaChannel(inputImage);
  if (createAlpha.addAlpha(inputImage, image_with_alpha, alpha) == -1) {
    std::cout << "already has 4 channels";
  }

  return image_with_alpha;
}

cv::Mat Tools::toCombineShapedImages(std::vector<PieceOfImage> shapedImages)
{
    cv::Mat wholeImage(
        cv::Size(datasToCombine.originalImageWidth,
                 datasToCombine.originalImageHeight),
        shapedImages[0].shapedImage.type());

    int width = datasToCombine.smallImageWidth;
    int height = datasToCombine.smallImageHeight;
    for (PieceOfImage shapedImage:shapedImages) {
        //set the start position to copy to dst
        int ox = shapedImage.location.col * width;
        int oy = shapedImage.location.row * height;
        if(shapedImage.sidesCode["左边"] == 1){
            ox = ox - shapedImage.r;
        }
        if(shapedImage.sidesCode["上边"] == 1){
            oy = oy - shapedImage.r;
        }
        int tx = ox;
        int ty = oy;

        //create mask
        cv::Mat src = shapedImage.shapedImage;
        std::vector<cv::Mat> channels(4);
        cv::split(src,channels);
        cv::Mat alphaChannel = channels[3];
        cv::Mat mask;
        cv::threshold(alphaChannel,mask,0,255,cv::THRESH_BINARY);

        //only rgb from src
        cv::Mat srcRGB;
        cv::merge(channels.data(),3,srcRGB);

        for(int y = 0;y < src.rows;y++){
            for(int x = 0;x < src.cols;x++){

                if(mask.at<uchar>(y,x) == 255){
                    if(ty < wholeImage.rows && tx < wholeImage.cols){
                        //wholeImage.at<cv::Vec3b>(ty,tx) = srcRGB.at<cv::Vec3b>(y,x);
                        wholeImage.at<cv::Vec3b>(ty,tx) = src.at<cv::Vec3b>(y,x);
                    }
                }
                tx++;
            }
            ty++;
            tx = ox;
        }

      //shapedxImage.shapedImage.copyTo(
      //        wholeImage(cv::Rect(x,y,width,height))
      //        wholeImage(cv::Rect(x,y,shapedImage.shapedImage.cols,shapedImage.shapedImage.rows))
      //        );
    }
    return wholeImage;
}

DatasToCombine Tools::getDatasToCombine()
{
    if(datasToCombine.originalImageHeight == 0 ||
        datasToCombine.originalImageWidth == 0 ||
        datasToCombine.smallImageHeight == 0 ||
        datasToCombine.smallImageWidth == 0){
        std::cout << "datas to combine is not setted yet!";
    }
    return datasToCombine;
}

void Tools::toTestSideCode(const int rows, const int cols) {

  // const int c = rows*cols;
  std::vector<ImageToTest> images = std::vector<ImageToTest>();

  // ImageToTest images[c];
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      ImageToTest image = ImageToTest();
      image.location.row = i;
      image.location.col = j;
      image.type = setType(image.location, rows, cols);
      images.push_back(image);
      // std::cout <<i<<":"<< i <<","<<j<<":"<<image.type << "\n";
      //  images[i+j]=ImageToTest();
      //  images[i+j].location.row=i;
      //  images[i+j].location.col=j;
      //  images[i+j].type = setType(images[i+j].location,rows,cols);
    }
  }

  // std::cout << "count of images:" <<sizeof(images)<<"\n";

  for (int i = 0; i < images.size(); i++) {
    std::string type = images[i].type;
    if (type == "左上角") {
      images[i].sidesCode.at("左边") = 0;
      images[i].sidesCode.at("上边") = 0;
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "右上角") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = 0;
      images[i].sidesCode.at("右边") = 0;
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "右下角") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = 0;
      images[i].sidesCode.at("下边") = 0;
      continue;
    }
    if (type == "左下角") {
      images[i].sidesCode.at("左边") = 0;
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = 0;
      continue;
    }

    if (type == "左边") {
      images[i].sidesCode.at("左边") = 0;
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "上边") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = 0;
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "右边") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = 0;
      images[i].sidesCode.at("下边") = randCode();
      continue;
    }
    if (type == "下边") {
      images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
      images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
      images[i].sidesCode.at("右边") = randCode();
      images[i].sidesCode.at("下边") = 0;
      continue;
    }
    images[i].sidesCode.at("左边") = -images[i - 1].sidesCode.at("右边");
    images[i].sidesCode.at("上边") = -images[i - cols].sidesCode.at("下边");
    images[i].sidesCode.at("右边") = randCode();
    images[i].sidesCode.at("下边") = randCode();
  }

  // for(int i=0;i<rows;i++){
  //     for(int j=0;j<cols;j++){
  //         std::cout << "("<<
  //             images[i*cols+j].sidesCode.at("左边")<<","<<
  //             images[i*cols+j].sidesCode.at("上边")<<","<<
  //             images[i*cols+j].sidesCode.at("右边")<<","<<
  //             images[i*cols+j].sidesCode.at("下边")<<","<<
  //             ")"<<",";
  //     }
  //     std::cout << "\n";
  // }
}
