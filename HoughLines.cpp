/*******************************************************************************
** 工程：直线霍夫变换
* 
** 日期：2023年05月31日
* 
** 环境Visual Studio 2019  OpenCV3.4.4
* 
** 介绍：直线霍夫变换，自写底层，可直接使用HoughLines（）函数，本项目用于练习OpenCV的使用，深入理解直线霍夫变换算法。
* 
** 过程：先将图片灰度化，在进行高斯滤波，后使用Canny（）函数，进行变沿提取，Canny中的threshold1、threshold2参数
** 通过计算图像中位数得到，实现自适应，对二值化图像变沿的x轴、y轴提取，遍历x、y值计算在不同角度下的截距值，r_num[][]
** 二维数组存放在不同角度下截距值出现的次数，其中数组的行号为角度，列号为截距值，数组内的值为当前截距值出现次数，
** 出现次数大于阈值则是一条直线
* 
** 不足：使用了大量堆栈。
*******************************************************************************/




#include <opencv2/opencv.hpp>  
#include<iostream>  
#include<vector>
#include <map>

#define pi           3.14159
#define angle_max    180
#define w            512                   //图像宽
#define h            512                   //图像高
#define max_point    724                   //直线上最多点数 sqrt(w * w + h * h)
#define r_max        724+724/2             //r的最大值        

using namespace std;
using namespace cv;



//求Mat的中位数
int GetMatMidVal(Mat& img)
{
    //判断如果不是单通道直接返回128
    if (img.channels() > 1) return 128;
    int rows = img.rows;
    int cols = img.cols;
    
   
    //定义数组
    float mathists[256] = { 0 };
    //遍历计算0-255的个数
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int val = img.at<uchar>(row, col);
            mathists[val]++;
        }
    }


    int calcval = rows * cols / 2;
    int tmpsum = 0;
    for (int i = 0; i < 255; ++i) {
        tmpsum += mathists[i];
        if (tmpsum > calcval) {
            return i;
        }
    }
    return 0;
}

//求自适应阈值的最小和最大值
void GetMatMinMaxThreshold(Mat& img, int& minval, int& maxval, float sigma)
{
    int midval = GetMatMidVal(img);

    // 计算低阈值
    minval = saturate_cast<uchar>((1.0 - sigma) * midval);
    //计算高阈值
    maxval = saturate_cast<uchar>((1.0 + sigma) * midval);

}


int main()
{
    int sigma;            //图像方差
    int minthreshold;     //最小阈值
    int maxthreshold;     //最大阈值
    vector<int>x;
    vector<int>y;


    //图像读取
    Mat chessboard = imread("C:/Users/Tiam/Desktop/HoughLines/image/chessboard.jpg");
    Mat chessboard_gray;        //灰度图像
    Mat chessboard_edge;        //边缘提取后图像

    

    //转化为灰度图
    cvtColor(chessboard, chessboard_gray, COLOR_RGB2GRAY);
    
    //高斯滤波
    GaussianBlur(chessboard_gray, chessboard_gray, Size(3, 3), 1.5, 2);      

    //自适应Canny边缘检测
    sigma = GetMatMidVal(chessboard_gray);
    GetMatMinMaxThreshold(chessboard_gray, minthreshold, maxthreshold, sigma);
    Canny(chessboard_gray, chessboard_edge, minthreshold, maxthreshold, 3);

    //提取变沿坐标
    for (int i = 0; i < chessboard_edge.rows; i++)
    {
        for (int j = 0; j < chessboard_edge.cols; j++)
        {
            if (chessboard_edge.at<uchar>(i, j) == 255)
            {
                x.push_back(i);
                y.push_back(j);
            }
        }
    }

    int r_num[angle_max][r_max] = { 0 };        //不同角度、下计算出的截距出现的次数

    //计算截距
    for (int angle = 0; angle < angle_max; angle++)
    {
        for (int i = 0; i < size(x); i++)
        {
            int r = (int)(x[i] * cos((angle * pi) / angle_max) + y[i] * sin((angle * pi) / angle_max));
            int r_d = (r + r_max / 2);          //防止出现负值
            r_num[angle][r_d] = r_num[angle][r_d] + 1;

        }
    }

    //释放容器
    x.clear();
    y.clear();

    //绘图需要的首尾坐标点
    int x1 = 0;
    int y1;
    int x2;
    int y2 = 0;
    
    for (int angle = 0; angle < angle_max; angle++)
    {
        for (int r= 0; r < (r_max+r_max/2); r++)
        {
            if (r_num[angle][r] > 120)
            {
                int r_act = r - (r_max / 2);                //将为防止出现负值加上的值减去
                y1 = r_act/sin((angle * pi) / angle_max);   //计算一个点（0，y1）
                x2 = r_act / cos((angle * pi) / angle_max); //计算一个点（x2,0）
                line(chessboard, Point(x1, y1), Point(x2, y2),(0, 0, 255), 1);  //画线
            }
        }
    }


    imshow("chessboard_edge", chessboard_edge);
    imshow("chessboard", chessboard);

    waitKey(0);

    return 0;
}





