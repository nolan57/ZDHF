#include "createalpha.h"

cv::Mat CreateAlpha::createAlphaChannel(cv::Mat& src){

    cv::Mat alpha = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    cv::Mat gray = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            alpha.at<uchar>(i, j) = 255;
        }
    }

    return alpha;
}

int CreateAlpha::addAlpha(cv::Mat& src, cv::Mat& dst, cv::Mat& alpha){

    if (src.channels() == 4)
    {
        return -1;
    }
    else if (src.channels() == 1)
    {
        cv::cvtColor(src, src, cv::COLOR_GRAY2RGB);
    }

    dst = cv::Mat(src.rows, src.cols, CV_8UC4);

    std::vector<cv::Mat> srcChannels;
    std::vector<cv::Mat> dstChannels;

    cv::split(src, srcChannels);

    dstChannels.push_back(srcChannels[0]);
    dstChannels.push_back(srcChannels[1]);
    dstChannels.push_back(srcChannels[2]);
    dstChannels.push_back(alpha);
    cv::merge(dstChannels, dst);

    return 0;

}
