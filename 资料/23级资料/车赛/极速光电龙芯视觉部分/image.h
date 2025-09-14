#pragma once
#include"zf_common_headfile.h"
#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<utility>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<fstream>
#include<string>
#define cap_id "/dev/video0"
#define cap_fps 120
#define cap_width 160
#define cap_height 120
#define BOUNDARY_NUM          (cap_height  * 4 / 2)
#define wei 0.5
#define _wei 0.8
#define _wei2 0.5
extern int16_t len_m;
const int8_t N = 5;//将边分N份大体遍历
const int16_t M = 800; // 最大边数
typedef std::pair<uint8_t, uint8_t> pii;
typedef std::pair<double, double> pdd;
typedef std::pair<int16_t, double> pdi;
/**
 * @brief 初始化函数
 * len 保存初始边线
 * l_min,r_min 保存最高点
 * index_l,index_r 第一次修改边线动态开点
 * cz_l,cz_r 第一次修改数组时保存上一个修改点的下标
 * viss 保存该x坐标是否被保存
 * count 保存该方向的点数
 * inflection 保存每个分段的生长方向
 * inflection_num 保存拐点数
 */
void init();
/**
 * @brief 八邻域算法寻线
 * que队列模拟bfs
 * vis标记数组
 * x,y为当前点坐标
 * i为当前点的八邻域方向
 * j为下一个点的八邻域方向
 * nx,ny为下一个点坐标
 * @param img 
 */
void eight_neighbour(cv::Mat& img);
/**
 * @brief 获取起始坐标
 * 
 * @param img 
 */
void get_start(cv::Mat& img);
/**
 * @brief 添加四条边线
 * 
 * @param img 
 */
void add_line(cv::Mat& img);
/**
 * @brief 第一次修改获取每个x坐标的最靠近中线的点
 * 
 * @param width 
 */
void get_final();
/**
 * @brief 
 * 
 * @param begin 
 * @param maxx 
 * @return true 
 * @return false 
 */
bool is_near_straight(int begin,int maxx);
/**
 * @brief 
 * 
 * @param begin 其中一侧的3 7 很多另一侧很少
 * @param 
 * @return true 
 * @return false 
 */
bool is_Circle(bool &flag);
/**
 * @brief 初入圆环补线
 * 
 * @param flag true 左侧为圆环 false 右侧为圆环
 * @param arr line_l or line_r
 * @param dir growth_dir_l or growth_dir_r
 */
void cir_line(bool flag, int16_t (*arr)[2], int8_t *dir);
/**
 * @brief 判断十字左右3 7 较多
 * 
 * @param begin 
 * @param maxx 
 * @return true 
 * @return false 
 */
bool is_Cross();
/**
 * @brief 如果第一次找到十字执行该函数进行补线 寻找上下两个拐点连接补线
 * 
 * @param img 
 */
void crosshair(cv::Mat& img);
/**
 * @brief 最小二乘法拟合直线方程
 * 
 * @param k 需要修改的斜率
 * @param b 需修改的截距
 * @param begin 开始的位置的下标
 * @param end 结束的位置的下标  左闭右闭
 * @param arr 点数组
 */
bool leastSquares(double &k,double &b, int16_t begin,const int16_t end, const int16_t (*arr)[2]);
/**
 * @brief 得到中线并划线
 * 
 * @param img 
 */
void get_mid();
/**
 * @brief 初始化图像  得到图像的行列  得到图像的二值化图像高斯滤波大津法
 * 
 * @param img 
 * @return cv::Mat 
 */
cv::Mat img_init(cv::Mat& img);
/**
 * @brief 求曲率
 * 
 * @param x1 第一个点的x坐标
 * @param y1 第一个点的y坐标
 * @param x2 第二个点的x坐标
 * @param y2 第二个点的y坐标
 * @param x3 第三个点的x坐标
 * @param y3 第三个点的y坐标
 * @return double  曲率
 */
double curvature(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t x3,uint8_t y3);
/**
 * @brief 求中线偏移量
 * 
 * @return pdi.first 偏移量  pdi.second 曲率
 */
pdi midOffset();
/**
 * @brief 获取图像指针
 * 
 * @return uint8_t* 
 */
uint8_t *image_p();
/**
 * @brief 画中线图传
 * 
 * @param x_bound 二维数组中的第一个下标
 * @param y_bound 二维数组中的第二个下标
 */
double MidOffset();
/**
 * @brief 纯跟踪计算角度和半径
 *          后轮为零点 
 * @param x 线上点的x坐标
 * @param y 线上点的y坐标
 * @param x2 车轮x坐标
 * @param y2 车轮y坐标
 * @return pdd 
 */
pdd Pure_tracking(uint8_t x,uint8_t y,uint8_t x2,uint8_t y2);
void draw_midline(uint8_t* y_bound, uint8_t* x_bound);
void draw_lline(uint8_t* y_bound, uint8_t* x_bound);
void draw_rline(uint8_t* y_bound, uint8_t* x_bound);
int coutmidllen();
/**
 * @brief 初始化摄像头
 * 
 * @param cap1 
 */
void cap_init(cv::VideoCapture &cap1);

void stop(cv::Mat img);

void solve(cv::VideoCapture &cap1);
void emit();