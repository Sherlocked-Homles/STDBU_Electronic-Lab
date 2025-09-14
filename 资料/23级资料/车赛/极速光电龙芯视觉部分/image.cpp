#include "image.h"
cv::Mat img0, img_bin0;

double thresholdvalue=81;
uint8_t is_stop=0;
int8_t nex_l[8][2] = {
    {1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}// 37  45  
    };//顺时针方向
int8_t nex_r[8][2] = {

    {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}
    };//逆时针方向
    // (-1,-1) (-1,0) (-1,1)     4 5 6   6 5 4
    // (0,-1)         (0,1)      3 顺7   7 逆3
    // (1,-1)  (1,0)  (1,1)      2 1 8   8 1 2
uint8_t growth_dir_l[M];//左边的生长方向
uint8_t growth_dir_r[M];//右边的生长方向
uint8_t line_l[M][2], line_r[M][2];//左右边线 下标从1开始
uint8_t f_line_l[cap_height], f_line_r[cap_height];//左右边线下标 下标从0开始
uint8_t mid_line[cap_height][2]; 
uint8_t l_min=0, r_min=0;//左右边线最高点
uint8_t disline_l=cap_height, disline_r=cap_height;//保存丢线最高点
int16_t len_l = 1, len_r = 1,len_m=1;//左右边线长度   从1开始
pii start_l, start_r;//左右起点
bool vis[cap_height][cap_width];//图像矩阵可见性数组
bool viss[cap_height];//标记横坐标
double k1,b1,k2,b2;//左右直线方程  1 左 2 右
double weight[12]={_wei2,_wei2,_wei2,_wei,_wei,_wei,_wei,_wei,wei,wei,wei,wei};
uint8_t cri_flage=0;// 0 1 2 3 4 非圆环 左圆环 右圆环 出左圆环 出右圆环
uint16_t dis0_l,dis0_r,dis1_l,dis1_r,dis2_l,dis2_r,dis3_l,dis3_r,dis4_l,dis4_r,dis5_l,dis5_r;
int16_t edge_l=0,edge_r=0;//左右边界来判断是否丢县

void init(){
    len_l=1,len_r=1,len_m=1;
    l_min=cap_height,r_min=cap_height;
    disline_l=cap_height, disline_r=cap_height;
    k1=0,b1=0,k2=0,b2=0;
    edge_l=0,edge_r=0;dis0_l=dis1_l=dis0_r=dis2_r=dis3_l=dis3_r=dis4_l=dis4_r=dis5_l=dis5_r=0;
    start_l = pii(cap_height-1,cap_width>>1);
    start_r = pii(cap_height-1,cap_width>>1);
    for(uint8_t i=0;i<cap_height;i++){
        for(uint8_t j=0;j<cap_width;j++){
            vis[i][j] = false;
        }
        viss[i] = false;
        f_line_l[i] = -1;
        f_line_r[i] = -1;
    }
}

void eight_neighbour(cv::Mat& img){
    std::queue <pii> que_l;
    std::queue <pii> que_r;
    que_l.push(start_l);
    que_r.push(start_r);
    // bool vis[cap_height][cap_width];
    vis[start_l.first][start_l.second] = true;
    vis[start_r.first][start_r.second] = true;
    int16_t x_l,y_l;
    int16_t y_r,x_r;
    line_l[0][0] = start_l.first;
    line_l[0][1] = start_l.second;
    len_l = 1;
    line_r[0][0] = start_r.first;
    line_r[0][1] = start_r.second;
    len_r = 1;
    while(!que_l.empty()&&!que_r.empty()&&len_l<M&&len_r<M){
        pii cur_l = que_l.front();
        que_l.pop();
        for(uint8_t i = 0; i < 8; i++){
            x_l = cur_l.first + nex_l[i][0];
            y_l = cur_l.second + nex_l[i][1];
            if(x_l>=0 && x_l< cap_height && y_l>=0 && y_l<cap_width&& vis[x_l][y_l]==false){
                uint8_t j = (i + 1) & 7;
                int16_t nx_l = cur_l.first + nex_l[j][0];
                int16_t ny_l = cur_l.second + nex_l[j][1];
                if(nx_l>=0 && nx_l<cap_height && ny_l >= 0 && ny_l<cap_width){
                    if(img.at<uint8_t>(x_l, y_l) == 0 && img.at<uint8_t>(nx_l, ny_l) == 255){
                        line_l[len_l][0] = x_l;
                        line_l[len_l][1] = y_l;
                        que_l.push({x_l, y_l});
                        l_min=(l_min<x_l?l_min:x_l);
                        growth_dir_l[len_l] = i+1;
                        len_l++;
                        vis[x_l][y_l] = true;
                        /********/
                        if(cri_flage==0){
                            if(len_l>5&&line_l[len_l-1][1]!=0&&line_l[len_l-2][1]==0){
                                dis0_l++;
                                if(dis0_l==2){dis3_l=len_l-2;}
                                dis1_l++;
                            }
                            if(len_r>5&&line_l[len_l][1]&&dis2_l==0){
                                dis1_l++;
                            }
                            if(len_r>5&&dis0_l&&line_l[len_l-1][1]==0&&line_l[len_l-2][1]){
                                dis2_l=1;
                                if(dis0_l==1)dis4_l=len_l-1;
                            }
                            if(len_l>5&&dis0_l==0&&line_l[len_l-1][1]==0){
                                dis5_l=len_l-1;
                            }
                            if(dis4_l!=0&&dis3_l==0){
                                dis3_l=len_l-2;
                            }
                            // if(line_l[len_l-1][1])dis0_l=1;
                        }
                        // if(cri_flage==0&&len_l>10&&line_l[len_l-10][0]>0){
                        //     if(edge_l>line_l[len_l-1][0])edge_l=line_l[len_l-1][0];
                        // }
                    } 
                }
            }
        }
        if(line_l[len_l][0]>line_r[len_r][0]){
            continue;
        }
        while(line_l[len_l-1][0]<line_r[len_r-1][0]&&!que_r.empty()){
            pii cur_r = que_r.front(); 
            que_r.pop();
            for(uint8_t i = 0; i < 8; i++){
                x_r = cur_r.first + nex_r[i][0];
                y_r = cur_r.second + nex_r[i][1];
                if(x_r>=0 && x_r< cap_height && y_r>=0 && y_r<cap_width && vis[x_r][y_r]==false){
                    uint8_t j = (i + 1) & 7;
                    int16_t nx_r = cur_r.first + nex_r[j][0];
                    int16_t ny_r = cur_r.second + nex_r[j][1];
                    if(nx_r>=0 && nx_r<cap_height && ny_r >= 0 && ny_r<cap_width){
                        if(img.at<uint8_t>(x_r, y_r) == 0 && img.at<uint8_t>(nx_r, ny_r) == 255){
                            line_r[len_r][0] = x_r;
                            line_r[len_r][1] = y_r;
                            que_r.push({x_r, y_r});
                            r_min=(r_min>x_r?x_r:r_min);
                            growth_dir_r[len_r] = i+1;
                            len_r++;
                            vis[x_r][y_r] = true;
                            /********/
                            if(cri_flage==0){
                            if(len_r>5&&line_r[len_r-1][1]!=159&&line_r[len_r-2][1]==159){
                                dis0_r++;
                                if(dis0_r==2){dis3_r=len_r-2;}
                                dis1_r++;
                            }
                            if(len_r>5&&line_r[len_r][1]!=159&&dis2_r==0){
                                dis1_r++;
                            }
                            if(len_r>5&&dis0_r&&line_r[len_r-1][1]==159&&line_r[len_r-2][1]!=159){
                                dis2_r=1;
                                if(dis0_r==1)dis4_r=len_r-1;
                            }
                            if(len_r>5&&dis0_r==0&&line_r[len_r-1][1]==159){
                                dis5_r=len_r-1;
                            }
                            if(dis4_r!=0&&dis3_r==0){
                                dis3_r=len_r-2;
                            }
                        }
                        // if(cri_flage==0&&len_r>10&&line_r[len_r-10][0]>0){
                        //     if(edge_r<line_r[len_r-1][0])edge_r=line_r[len_r-1][0];
                        // }
                        } 
                    }
                }
            }
        }
    }
    
    // cout<<"l4: r5:"<<(int)dis5_r<<"  "<<(int)line_l[dis4_l][0]<<"    "<<(int)line_r[dis5_r][0]<<endl;
    // printf("r4: l5:%d %d  %d\n",dis5_l,line_r[dis4_r][0],line_l[dis5_l][0]);
    // std::cout<<"l_min: "<<(int)l_min<<" r_min: "<<(int)r_min<<std::endl;
    // std::cout<<"len_l: "<<(int)line_l[dis3_l][0]<<" len_r: "<<(int)line_r[dis3_r][0]<<std::endl;
    if(dis4_l-dis5_l<15&&dis0_l>1){
        dis0_l--;
        // dis4_l=0;
        // dis5_l=dis
    }
    if(dis4_r-dis5_r<15&&dis0_r>1){
        dis0_r--;
    }
    if(line_l[dis4_l][0]<=13)dis0_l--;
    if(line_r[dis4_r][0]<=13)dis0_r--;
    if(line_l[dis4_l][0]>=35&&(line_l[len_l-3][0]<=20&&line_r[len_r-3][0]<=20)&&dis0_l>=2&&dis0_r<=1&&(line_l[dis3_l][0]-l_min>=10)&&(line_l[dis4_l][0]<line_r[dis5_r][0]-5)&&line_l[dis4_l][0]>25)cri_flage=1;
    else if(line_r[dis4_r][0]>=35&&(line_l[len_l-3][0]<=20&&line_r[len_r-3][0]<=20)&&dis0_r>=2&&dis0_l<=1&&(line_r[dis3_r][0]-r_min>=10)&&(line_r[dis4_r][0]<line_l[dis5_l][0]-5)&&line_r[dis4_r][0]>25)cri_flage=2;
    if(cri_flage==0){
        uint8_t lcount=0,rcount=0;
        if(dis0_l>=2&&dis0_r<=1&&dis5_l!=0&&dis4_l!=0){
            for(int i=dis5_l;i<dis4_l;i++){
                if(growth_dir_l[i]==5){
                    lcount++;
                }
            }
            if(lcount>=10&&line_l[dis4_l][0]>=35&&(line_l[len_l-3][0]<=20&&line_r[len_r-3][0]<=20)&&(line_l[dis3_l][0]-l_min>=10)&&line_l[dis4_l][0]>25){
                cri_flage=1;
            }
        }
        if(dis0_r>=2&&dis0_l<=1&&dis5_r!=0&&dis4_r!=0){
            for(int i=dis5_r;i<dis4_r;i++){
                if(growth_dir_r[i]==5){
                    rcount++;
                }
            }
            if(rcount>=10&&line_r[dis4_r][0]>=35&&(line_r[len_r-3][0]<=20&&line_l[len_l-3][0]<=20)&&(line_r[dis3_r][0]-r_min>=10)&&line_r[dis4_r][0]>25){
                cri_flage=2;
            }
        }
    }
    // while(!que_l.empty())que_l.pop();
    // while(!que_r.empty())que_r.pop();
}

void get_start(cv::Mat& img){
    int x, y, y1;
    x = cap_height - 1;
    y = cap_width >> 1;
    if(img.at<uint8_t>(x, y) == 0){
        if(img.at<uint8_t>(x, y>>1) == 255)y=y>>1;
        else if(img.at<uint8_t>(x, y+(cap_width>>2)) == 255)y=y+(cap_width>>2);
    }
    while(x > cap_height/3 && img.at<uint8_t>(x, y) == 0) x--;
    y1 = y;
    while(y < cap_width-1 && img.at<uint8_t>(x, y) == 255) y++;
    start_r = {x, y};
    while(y1 > 0 && img.at<uint8_t>(x, y1) == 255) y1--;
    start_l = {x, y1};
}

void add_line(cv::Mat& img){
    uint8_t x, y;
    x = cap_height - 1;
    y = cap_width - 1;
    cv::line(img, cv::Point(0, 0), cv::Point(0, x), cv::Scalar(0, 0, 0), 1);
    cv::line(img, cv::Point(0, 0), cv::Point(y, 0), cv::Scalar(0, 0, 0), 1);
    cv::line(img, cv::Point(y, 0), cv::Point(y, x), cv::Scalar(0, 0, 0), 1);
    // cv::line(img, cv::Point(0, x), cv::Point(y, x), cv::Scalar(0, 0, 0), 1);
}

void get_final(){
    bool flag= true;
    // bool viss[cap_height];
    for(int16_t i=1;i<len_l;i++){
        if(!viss[line_l[i][0]]){
            f_line_l[line_l[i][0]]=line_l[i][1];
            viss[line_l[i][0]]=true;
        }
        else{
            if(abs(f_line_l[line_l[i][0]]-(cap_width>>1))>abs(line_l[i][1]-(cap_width>>1))){
                f_line_l[line_l[i][0]]=line_l[i][1];
            }
        }
        if(line_l[i][1]!=0&&flag){disline_l=line_l[i][0];flag=false;}
        /**********
        if(!flag&&cri_flage==0&&i>10&&line_l[i-10][0]>0){
            if(edge_l>line_l[i][0])edge_l=line_l[i][0];
        }
        **********/
    }
    if(disline_l==cap_height)disline_l=0;
    flag=true;
    // memset(viss, 0, sizeof(viss));
    for(uint8_t i=0;i<cap_height;i++){viss[i]=false;}
    for(int16_t i=1;i<len_r;i++){
        if(!viss[line_r[i][0]]){
            f_line_r[line_r[i][0]]=line_r[i][1];
            viss[line_r[i][0]]=true;
        }
        else{
            if(abs(f_line_r[line_r[i][0]]-(cap_width>>1))>abs(line_r[i][1]-(cap_width>>1))){
                f_line_r[line_r[i][0]]=line_r[i][1];
            }
        }
        if(line_r[i][1]!=cap_width-1&&flag){disline_r=line_r[i][0];flag=false;}

        /**********
        if(!flag&&cri_flage==0&&i>10&&line_r[i-10][0]>0){
            if(edge_r>line_r[i][0])edge_r=line_r[i][0];
        }
        **********/
    }
    if(disline_r==cap_height)disline_r=0;
    // std::cout<<(int)disline_l<<" "<<(int)disline_r<<std::endl;
}


// bool is_near_straight(int begin,int maxx){
//     return false;
// }


// void cir_line(bool flag, uint8_t (*arr)[2], uint8_t *dir){
//     return ;
// }

// bool is_Cross(){
//     return false;
// }

bool leastSquares(double &k,double &b, int16_t begin,const int16_t end, const uint8_t (*arr)[2]){
    double sum_x=0,sum_y=0,sum_xy=0,sum_xx=0,sum_yy=0;
    bool flag=true;
    if(begin<0)begin=0;
    int16_t len=end-begin+1;
    assert(arr!=nullptr);
    for(int16_t i=begin;i<=end;i++){
        sum_x+=arr[i][0];
        sum_y+=arr[i][1];
        sum_xy+=arr[i][0]*arr[i][1];
        sum_xx+=arr[i][0]*arr[i][0];
        sum_yy+=arr[i][1]*arr[i][1];
    }
    // assert(len>0);
    // assert(len*sum_xx-sum_x*sum_x!=0);
    if(len>0&&len*sum_xx-sum_x*sum_x!=0){
    k=(len*sum_xy-sum_x*sum_y)/(len*sum_xx-sum_x*sum_x);
    b=(sum_y-k*sum_x)/len;
    }
    else{flag=false;}
    return flag;
}

void crosshair(){//l_min,r_min;
    bool flag_l=false,flag_r=false;//是否找到下拐点 找不到就找上拐点
    bool _flag_l=false,_flag_r=false;
    int16_t tot_l=0,tot_r=0;
    uint8_t left=0,right=0,front=0;
    uint8_t _left=0,_right=0,_front=0;
    for(int16_t i=1;i<len_l-6;i++){
        if(line_l[i][0]<cap_height/8)break;
        if(growth_dir_l[i]>=2&&growth_dir_l[i]<=4)left++;
        if(growth_dir_l[i]>=4&&growth_dir_l[i]<=6)front++;
        if(growth_dir_l[i]>=6&&growth_dir_l[i]<=8)right++;
        if(i>11){
            if(growth_dir_l[i-11]>=2&&growth_dir_l[i-11]<=4)left--;
            if(growth_dir_l[i-11]>=4&&growth_dir_l[i-11]<=6)front--;
            if(growth_dir_l[i-11]>=6&&growth_dir_l[i-11]<=8)right--;
            // std::cout<<line_l[i][0]<<": "<<(int)left<<" "<<(int)front<<" "<<(int)right<<std::endl;
        }
        if(left>=4&&right>=4&&(front<=8&&front>=2)){
            flag_l=true;
            tot_l=i-6;
            // std::cout<<"yes0 "<<line_l[tot_l][0]<<std::endl;
        }
        if(flag_l)break;
        uint8_t cnt=0;
        if((growth_dir_l[i]==5||growth_dir_l[i]==6)&&(growth_dir_l[i-1]==5||growth_dir_l[i-1]==6)){
            uint16_t j=i+1;
            while(j<=len_l-1&&abs(j-i)<=10){
                if(growth_dir_l[j]==3)cnt++;
                j++;
            }    
            if(cnt>=0.8*abs(j-i)){
                flag_l=true;
                tot_l=i;
                // std::cout<<"yes4 "<<line_l[tot_l][0]<<std::endl;
            }
        }
        if(flag_l)break;
        cnt=0;
        if((growth_dir_l[i]==5||growth_dir_l[i]==6)&&(growth_dir_l[i+1]==5||growth_dir_l[i+1]==6)){
            uint16_t j=i-1;
            while(j>=1&&abs(i-j)<=10){
                if(growth_dir_l[j]==7)cnt++;
                j--;
            }    
            if(cnt>=0.8*abs(i-j)){
                _flag_l=true;
                tot_l=i;
                // std::cout<<"yes4 "<<line_l[tot_l][0]<<std::endl;
            }
        }
        if(_flag_l)break;
    }
    for(int16_t i=1;i<len_r-6;i++){
        if(line_r[i][0]<cap_height/8)break;
        if(growth_dir_r[i]>=2&&growth_dir_r[i]<=4)_left++;
        if(growth_dir_r[i]>=4&&growth_dir_r[i]<=6)_front++;
        if(growth_dir_r[i]>=6&&growth_dir_r[i]<=8)_right++;
        if(i>11){
            if(growth_dir_r[i-11]>=2&&growth_dir_r[i-11]<=4)_left--;
            if(growth_dir_r[i-11]>=4&&growth_dir_r[i-11]<=6)_front--;
            if(growth_dir_r[i-11]>=6&&growth_dir_r[i-11]<=8)_right--;
        }
        if(_left>=4&&_right>=4&&(_front<=8&&_front>=2)){
            flag_r=true;
            tot_r=i-6;
            // std::cout<<"yes1 "<<line_r[tot_r][0]<<std::endl;
        }
        if(flag_r)break;
        uint8_t cnt=0;
        if((growth_dir_r[i]==5||growth_dir_r[i]==6)&&(growth_dir_r[i-1]==5||growth_dir_r[i-1]==6)){
            uint16_t j=i+1;
            while(j<=len_r-1&&j-i<=10){
                if(growth_dir_r[j]==3)cnt++;
                j++;
            }    
            if(cnt>=0.8*(j-i)){
                flag_r=true;
                tot_r=i;
                // std::cout<<"yes3 "<<line_r[tot_r][0]<<std::endl;
            }
        }
        if(flag_r)break;
        cnt=0;
        if((growth_dir_r[i]==5||growth_dir_r[i]==6)&&(growth_dir_r[i+1]==5||growth_dir_r[i+1]==6)){
            uint16_t j=i-1;
            while(j>=1&&abs(i-j)<=10){
                if(growth_dir_r[j]==7)cnt++;
                j--;
            }    
            if(cnt>=0.8*abs(i-j)){
                _flag_r=true;
                tot_r=i;
                // std::cout<<"yes5 "<<line_r[tot_r][0]<<std::endl;
            }
        }
        if(_flag_r)break;
    }
    if(flag_l){
        // std::cout<<"flag_ldown"<<tot_l<<std::endl;
        // std::cout<<"line_l[tot_l][0]:"<<line_l[tot_l][0]<<std::endl;
        if(cri_flage<=2)cri_flage=0;

        if(cri_flage==6){
            k1=1.0*(cap_width-1-line_l[tot_l][1])/((cap_height>>1)-line_l[tot_l][0]);
            b1=line_l[tot_l][1]-k1*line_l[tot_l][0];
            int16_t x=line_l[tot_l][0];
            for(int16_t i=tot_l+1;i<len_l-1;i++){
                if(x>=l_min&&x<cap_height){
                    line_l[i][0]=x-1;
                    if(k1*line_l[i][0]+b1>=0&&k1*line_l[i][0]+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                    else if(k1*line_l[i][0]+b1<0)line_l[i][1]=0;
                    else line_l[i][1]=cap_width-1;
                    x--;
                }
                else{
                    line_l[i][0]=l_min;
                    if(line_l[i][0]*k1+b1>=0&&line_l[i][0]*k1+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                    else if(line_l[i][0]*k1+b1<0)line_l[i][1]=0;
                    else line_l[i][1]=cap_width-1;
                }
            }
        }
        else{
            if(!leastSquares(k1,b1,tot_l-6,tot_l,line_l))return ;
            // std::cout<<"k1: "<<k1<<" b1: "<<b1<<std::endl;
            int16_t x=line_l[tot_l][0];
            if(x<=40)return ;
            for(int16_t i=tot_l+1;i<len_l-1;i++){
                if(x>=l_min&&x<cap_height){
                    line_l[i][0]=x-1;
                    if(k1*line_l[i][0]+b1>=0&&k1*line_l[i][0]+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                    else if(k1*line_l[i][0]+b1<0)line_l[i][1]=0;
                    else line_l[i][1]=cap_width-1;
                    x--;
                }
                else{
                    line_l[i][0]=l_min;
                    if(line_l[i][0]*k1+b1>=0&&line_l[i][0]*k1+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                    else if(line_l[i][0]*k1+b1<0)line_l[i][1]=0;
                    else line_l[i][1]=cap_width-1;
                }
            }
        }
    }
    if(_flag_l&&(!flag_l)){
        // std::cout<<"flag_lup"<<tot_l<<std::endl;
        // std::cout<<"line_l[tot_l][0]:"<<line_l[tot_l][0]<<std::endl;
        if(cri_flage==1){
            k2=1.0*(line_l[tot_l][1]-cap_width+1)/(line_l[tot_l][0]-(cap_height-1));
            b2=line_l[tot_l][1]-k2*(line_l[tot_l][0]);
            int16_t x=line_l[1][0];
            for(int16_t i=1;line_r[i][0]>=line_l[tot_l][0]+1;i++){
                if(x>=line_l[tot_l][0]&&x<cap_height){
                    line_r[i][0]=x;
                    line_r[i][1]=k2*x+b2;
                    x--;
                }
                else{
                    line_r[i][0]=line_l[tot_l][0];
                    line_r[i][1]=line_l[tot_l][1];
                }
            }
        }
        else {
            if(!leastSquares(k1,b1,tot_l,tot_l+6,line_l))return ;
            int16_t x=line_l[tot_l][0];
            for(int16_t i=tot_l-1;i>0;i--){
                if(x<cap_height&&x>=l_min){
                    line_l[i][0]=x+1;
                    if(line_l[i][0]*k1+b1>=0&&line_l[i][0]*k1+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                    else if(line_l[i][0]*k1+b1<0)line_l[i][1]=0;
                    else line_l[i][1]=cap_width-1;
                    x++;
                }
                else{
                    line_l[i][0]=cap_height-1;
                    if(k1*line_l[i][0]+b1>=0&&k1*line_l[i][0]+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                    else if(k1*line_l[i][0]+b1<0)line_l[i][1]=0;
                    else line_l[i][1]=cap_width-1;
                }
            }
        }
        return ;
    }
    if(flag_r){
        // std::cout<<"flag_rup"<<tot_r<<std::endl;
        // std::cout<<"line_r[tot_r][0]:"<<line_r[tot_r][0]<<std::endl;
        if(cri_flage<=2)cri_flage=0;
        if(cri_flage==5){
            k2=1.0*(0-line_r[tot_r][1])/((cap_height>>1)-line_r[tot_r][0]);
            b2=line_r[tot_r][1]-k2*(line_r[tot_r][0]);
            int16_t x=line_r[tot_r][0];
            for(int16_t i=tot_r+1;i<len_r-1;i++){
                if(x>=r_min&&x<cap_height){
                    line_r[i][0]=x-1;
                    if(k2*line_r[i][0]+b2<=cap_width-1&&k2*line_r[i][0]+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                    else if(k2*line_r[i][0]+b2>cap_width-1)line_r[i][1]=cap_width-1;
                    else line_r[i][1]=0;
                    x--;
                }
                else{
                    line_r[i][0]=r_min;
                    if(line_r[i][0]*k2+b2<=cap_width-1&&line_r[i][0]*k2+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                    else if(line_r[i][0]*k2+b2>cap_width-1)line_r[i][1]=cap_width-1;
                    else line_r[i][1]=0;
                }
            }
        }
        else{
            if(!leastSquares(k2,b2,tot_r-6,tot_r,line_r))return ;
            // std::cout<<"k2: "<<k2<<" b2: "<<b2<<std::endl;
            int16_t x=line_r[tot_r][0];
            if(x<=40)return ;
            for(int16_t i=tot_r+1;i<len_r-1;i++){
                if(x>=r_min&&x<cap_height){
                    line_r[i][0]=x-1;
                    if(k2*line_r[i][0]+b2<=cap_width-1&&k2*line_r[i][0]+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                    else if(k2*line_r[i][0]+b2>cap_width-1)line_r[i][1]=cap_width-1;
                    else line_r[i][1]=0;
                    x--;
                }
                else{
                    line_r[i][0]=r_min;
                    if(line_r[i][0]*k2+b2<=cap_width-1&&line_r[i][0]*k2+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                    else if(line_r[i][0]*k2+b2>cap_width-1)line_r[i][1]=cap_width-1;
                    else line_r[i][1]=0;
                }
            }
        }
    }
    if(_flag_r&&(!flag_r)){
        // std::cout<<"flag_rdown"<<tot_r<<std::endl;
        // std::cout<<"line_r[tot_r][0]:"<<line_r[tot_r][0]<<std::endl;
        if(cri_flage==2){
            k1=1.0*(line_r[tot_r][1]-cap_width+1)/line_r[tot_r][0];
            b1=line_r[tot_r][1]-k1*line_r[tot_r][0];
            int16_t x=line_r[1][0];
            for(int16_t i=1;line_l[i][0]>=line_r[tot_r][0]+1;i++){
                if(x<cap_height&&x>=line_r[tot_r][0]){
                    line_l[i][0]=x;
                    line_l[i][1]=k1*x+b1;
                    x--;
                }
                else{
                    line_l[i][0]=line_r[tot_r][0];
                    line_l[i][1]=line_r[tot_r][1];
                }
            }
        }
        else{
            if(!leastSquares(k2,b2,tot_r,tot_r+6,line_r))return ;
            int16_t x=line_r[tot_r][0];
            for(int16_t i=tot_r-1;i>0;i--){
                if(x<cap_height&&x>=r_min){
                    line_r[i][0]=x+1;
                    if(k2*line_r[i][0]+b2<=cap_width-1&&k2*line_r[i][0]+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                    else if(k2*line_r[i][0]+b2>cap_width-1)line_r[i][1]=cap_width-1;
                    else line_r[i][1]=0;
                    x++;
                }
                else{
                    line_r[i][0]=cap_height;
                    if(line_r[i][0]*k2+b2<=cap_width-1&&line_r[i][0]*k2+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                    else if(line_r[i][0]*k2+b2>cap_width-1)line_r[i][1]=cap_width-1;
                    else line_r[i][1]=0;
                }
            }
        }
        return;
    }
    if(cri_flage==6){
        k1=1.0*(cap_width-1-line_l[1][1])/((cap_height>>1)-line_l[1][0]);
        b1=line_l[1][1]-k1*line_l[1][0];
        int16_t x=line_l[1][0];
        for(int16_t i=1;i<len_l-1;i++){
            if(x>=l_min&&x<cap_height){
                line_l[i][0]=x-1;
                if(k1*line_l[i][0]+b1>=0&&k1*line_l[i][0]+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                else if(k1*line_l[i][0]+b1<0)line_l[i][1]=0;
                else line_l[i][1]=cap_width-1;
                x--;
            }
            else{
                line_l[i][0]=l_min;
                if(line_l[i][0]*k1+b1>=0&&line_l[i][0]*k1+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                else if(line_l[i][0]*k1+b1<0)line_l[i][1]=0;
                else line_l[i][1]=cap_width-1;
            }
        }
        return ;
    }
    if(cri_flage==5){
        k2=1.0*(0-line_r[1][1])/((cap_height>>1)-line_r[1][0]);
        b2=line_r[1][1]-k2*(line_r[1][0]);
        int16_t x=line_r[1][0];
        for(int16_t i=1;i<len_r-1;i++){
            if(x>=r_min&&x<cap_height){
                line_r[i][0]=x-1;
                if(k2*line_r[i][0]+b2<=cap_width-1&&k2*line_r[i][0]+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                else if(k2*line_r[i][0]+b2>cap_width-1)line_r[i][1]=cap_width-1;
                else line_r[i][1]=0;
                x--;
            }
            else{
                line_r[i][0]=r_min;
                if(line_r[i][0]*k2+b2<=cap_width-1&&line_r[i][0]*k2+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                else if(line_r[i][0]*k2+b2>cap_width-1)line_r[i][1]=cap_width-1;
                else line_r[i][1]=0;
            }
        }
        return ;
    }
    if(cri_flage==2){
        k1=1.0*(cap_width-1-line_l[1][1])/((cap_height>>1)-line_l[1][0]);
        b1=line_l[1][1]-k1*line_l[1][0];
        int16_t x=line_l[1][0];
        for(int16_t i=1;i<len_l-1;i++){
            if(x>=l_min&&x<cap_height){
                line_l[i][0]=x-1;
                if(k1*line_l[i][0]+b1>=0&&k1*line_l[i][0]+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                else if(k1*line_l[i][0]+b1<0)line_l[i][1]=0;
                else line_l[i][1]=cap_width-1;
                x--;
            }
            else{
                line_l[i][0]=l_min;
                if(line_l[i][0]*k1+b1>=0&&line_l[i][0]*k1+b1<cap_width)line_l[i][1]=k1*line_l[i][0]+b1;
                else if(line_l[i][0]*k1+b1<0)line_l[i][1]=0;
                else line_l[i][1]=cap_width-1;
            }
        }
        return ;
    }
    if(cri_flage==1){
        k2=1.0*(0-line_r[1][1])/((cap_height>>1)-line_r[1][0]);
        b2=line_r[1][1]-k2*(line_r[1][0]);
        int16_t x=line_r[1][0];
        for(int16_t i=1;i<len_r-1;i++){
            if(x>=r_min&&x<cap_height){
                line_r[i][0]=x-1;
                if(k2*line_r[i][0]+b2<=cap_width-1&&k2*line_r[i][0]+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                else if(k2*line_r[i][0]+b2>cap_width-1)line_r[i][1]=cap_width-1;
                else line_r[i][1]=0;
                x--;
            }
            else{
                line_r[i][0]=r_min;
                if(line_r[i][0]*k2+b2<=cap_width-1&&line_r[i][0]*k2+b2>=0)line_r[i][1]=k2*line_r[i][0]+b2;
                else if(line_r[i][0]*k2+b2>cap_width-1)line_r[i][1]=cap_width-1;
                else line_r[i][1]=0;
            }
        }
        return ;
    }
    return ;
}

void get_mid(){
    // int last = cap_height>>1;
    for(int16_t i=start_l.first-1;i>(l_min>r_min?l_min:r_min);i--){
        if(f_line_l[i]>f_line_r[i])continue;
        if(f_line_l[i]!=-1&&f_line_r[i]!=-1){
            mid_line[len_m][0]=i;
            mid_line[len_m][1]=(f_line_l[i]+f_line_r[i])>>1;
            len_m++;
        }
    }
    return ;
}

cv::Mat img_init(cv::Mat& img){
    cv::Mat img_gray, img_bin,img_temp;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    // cv::GaussianBlur(img_gray, img_temp, cv::Size(5, 5), 0);
    // cv::threshold(img_temp, img_bin, 0, 255, cv::THRESH_OTSU);
    cv::threshold(img_gray, img_bin, 0, 255, cv::THRESH_BINARY+cv::THRESH_OTSU);
    // cv::threshold(img_temp, img_bin, 0, 255, cv::THRESH_BINARY+cv::THRESH_OTSU);
    // cv::threshold(img_temp, img_bin, thresholdvalue, 255, cv::THRESH_BINARY);
    // cv::adaptiveThreshold(img_temp, img_bin, 255, 
    //     cv::ADAPTIVE_THRESH_MEAN_C, 
    //     cv::THRESH_BINARY, 11, 2);
    return img_bin;
}
// double curvature(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t x3,uint8_t y3){
// 	double a,b,c;
// 	a=sqrt(abs(x2-x1)*abs(x2-x1)+abs(y2-y1)*abs(y2-y1));
// 	b=sqrt(abs(x3-x2)*abs(x3-x2)+abs(y3-y2)*abs(y3-y2));
// 	c=sqrt(abs(x1-x3)*abs(x1-x3)+abs(y1-y3)*abs(y1-y3));
// 	double p=(a+b+c)/2.0;
// 	double s=sqrt(p*(p-a)*(p-b)*(p-c));
// 	// double r=a*b*c/(4.0*s);
//     if(s==0)return 1.0/(a>b?b:a);
// 	double val=4.0*s/(a*b*c);
// 	// double r=a*b*c/(4.0*s);
//     // if(y3<y2)r=-r;
// 	// return r;
//     if(y3<y2)val=-val;
// 	return val;
// }
// pdi midOffset(){
//     uint8_t disline_m=(disline_l>disline_r?disline_l:disline_r);
//     while(f_line_l[disline_m]==-1||f_line_r[disline_m]==-1)disline_m--;
//     uint8_t dert=7;
//     while(disline_m-(dert<<1)<mid_line[len_m][0]&&dert>1){
//         dert--;
//     }
//     // std::cout<<"midline_m: "<<(int)disline_m<<" d:"<<(int)dert<<std::endl;
//     // std::cout<<"1:"<<(int)mid_line[2][1]<<std::endl;
//     if(mid_line[disline_m-(dert<<1)][1]==mid_line[disline_m][1]&&mid_line[disline_m-(dert<<1)][1]==mid_line[disline_m-dert][1]){
//         return {mid_line[2][1]-80,0};
//     }
//     else{
//         return {mid_line[2][1]-80,curvature(mid_line[disline_m][0],mid_line[disline_m][1],mid_line[disline_m-dert][0],mid_line[disline_m-dert][1],mid_line[disline_m-(dert<<1)][0],mid_line[disline_m-(dert<<1)][1])};
//     }
// }
double MidOffset(){
    if(is_stop)return 0;
    if(len_m<5)return 0;
    uint8_t disline_m;//误差起点
    disline_m=(disline_l>disline_r?disline_l:disline_r);//误差起点
    // uint8_t min_m;
    // min_m=(l_min>r_min?l_min:r_min);
    // std::cout<<(int)l_min<<" "<<(int)r_min<<std::endl;
    double offset=0;//返回值
    uint8_t num=0;//有效点的个数
    // std::cout<<(int)disline_l<<" "<<(int)disline_r<<std::endl;
    // std::cout<<(int)line_l[2][0]<<" "<<(int)disline_m<<std::endl;
    uint8_t len=start_l.first+1-disline_m;
    // static float offset_sum=0;
    // std::cout<<"midline_m: "<<(int)disline_m<<std::endl;
    // std::cout<<"1:"<<(int)min_m<<std::endl;
    // std::cout<<"midline_m: "<<(int)disline_m<<std::endl;
    // std::cout<<(int)len<<" "<<(int)len_m<<std::endl;
    if(len_m>=len+24){
        for(int16_t i=len+18;i<=len_m;i++){
            // if(f_line_l[i]>0&&f_line_r[i]!=-1&&f_line_r[i]!=cap_width-1&&f_line_l[i]!=cap_width-1){
            // std::cout<<i<<std::endl;
            // if(mid_line[i][0]>60)continue;
            // if(mid_line[i][0]<30)break;
            if(num>=15&&(cri_flage==1||cri_flage==2||cri_flage==5||cri_flage==6))break;
                if(f_line_r[i]!=-1&&f_line_l[i]!=-1){
                    num++;
                    offset+=(mid_line[i][1]-79);//*weight[mid_line[i][0]/10];
                // std::cout<<(int)mid_line[i][0]<<" "<<(int)mid_line[i][1]<<"\n";
                }
        }
    }
    else{
        for(int16_t i=len;i<=len_m;i++){
            if(num>=15&&(cri_flage==1||cri_flage==2||cri_flage==5||cri_flage==6))break;
            if(f_line_r[i]!=-1&&f_line_l[i]!=-1){
                num++;
                offset+=(mid_line[i][1]-79);//*weight[mid_line[i][0]/10];
            // std::cout<<(int)mid_line[i][0]<<" "<<(int)mid_line[i][1]<<"\n";
            }
        }
    }
    // std::cout<<"num: "<<(int)num<<std::endl;
    // if(num==0) printf("num=0\n");
    // else printf("midline_m: %d num: %d offset: %lf oof: %lf\n",disline_m,num,offset/num,offset);
    if(mid_line[len_m-2][0]>=50){
        if(offset>0) return 100;
        else return -100;
    }
    if(num==0)return 0;
    // num += 5 ;
    if(abs(offset/num)>160)return 0;
    return (offset/num);
}
// pdd Pure_tracking(uint8_t x,uint8_t y,uint8_t x2,uint8_t y2){
//     double r,angle;
//     double a,b,c;
//     a=sqrt(abs(x2*x2-x*x)+abs(y2*y2-y*y));
//     b=sqrt(x*x+y*y);
//     c=sqrt(x2*x2+y2*y2);
//     angle=acos((x*x2+y*y2)/(a*b));
//     r=b/2.0/sin(angle);
//     angle*=2;
//     return {r,angle};
// }
void cap_init(cv::VideoCapture &cap1){
    // ips200_init("/dev/fb0");
    cap1.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap1.set(cv::CAP_PROP_FPS, cap_fps);
    cap1.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap1.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
}
// int coutmidllen(){
//     return len_m;
// }
uint8_t *image_p(){
    uint8_t *gray_image = reinterpret_cast<uint8_t *>(img_bin0.ptr(0));
    return gray_image;
}
void draw_midline(uint8_t* y_bound, uint8_t* x_bound){
    for(int i=1;i<len_m;i++){
        x_bound[i-1]=mid_line[i][0];
        // std::cout<<"midline: "<<(int)mid_line[i][0]<<" "<<(int)mid_line[i][1]<<std::endl;
        y_bound[i-1]=mid_line[i][1];
    }
}
void draw_lline(uint8_t* y_bound, uint8_t* x_bound){
    for(int i=1;i<len_l;i++){
        x_bound[i-1]=line_l[i][0];
        y_bound[i-1]=line_l[i][1];
    }
}
void draw_rline(uint8_t* y_bound, uint8_t* x_bound){
    for(int i=1;i<len_r;i++){
        x_bound[i-1]=line_r[i][0];
        y_bound[i-1]=line_r[i][1];
    }
}
void emit(){
    img0.release();
    img_bin0.release();
    // cv::waitKey(1);
}
void stop(cv::Mat img){
    if(is_stop>=70)return ;
    uint8_t cnt=0;
    uint8_t x=119;
    for(uint8_t i=0;i<159;i++){
        if(img.at<uint8_t>(x,i)==0&&img.at<uint8_t>(x,i+1)==255)cnt++;
    }
    // std::cout<<(int)cnt<<std::endl;
    if(cnt>=6||is_stop!=0){is_stop++;}//std::cout<<"yes"<<std::endl;}
    return ;
}
void solve(cv::VideoCapture &cap1){
    init();
    if(!cap1.isOpened()){std::cout<<"摄像头打开失败\n";exit(-1);}
    cap1 >> img0;
    cv::resize(img0, img0, cv::Size(cap_width, cap_height), 0, 0, cv::INTER_LINEAR);//-11
    img_bin0 = img_init(img0);//-13
    stop(img_bin0);//到结束 -2
    if(is_stop<1){
        add_line(img_bin0);
        get_start(img_bin0);
        eight_neighbour(img_bin0);
        crosshair();//补全直线
        get_final();
        get_mid();
        double temperature = MidOffset();
        std::ostringstream ss;
        ss.str(""); // 清空字符串流
        ss << fixed << setprecision(2) << temperature ;
        cv::putText(img_bin0, ss.str(), cv::Point(70, 100), 
                cv::FONT_HERSHEY_PLAIN, 0.7, cv::Scalar(0, 0, 0), 1);
    }
}