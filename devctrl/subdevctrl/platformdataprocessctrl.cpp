#include "platformdataprocessctrl.h"

#if _MSC_VER >=1600

#pragma execution_character_set("utf-8")
#endif


QPlatformDataProcessCtrl::QPlatformDataProcessCtrl(QObject *parent):QObject(parent)
{

}

/******************************************
* 功能:根据输入的任务信息  执行任务操作
* 输入:任务配置信息
* 输出:
* 返回值:执行成功/失败
* 维护记录:
******************************************/
bool QPlatformDataProcessCtrl::runTask(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{

    bool bRel = false;
    int nProcessType = pTask->m_tParamData.value<st_dataprocessInput>().nProcessType;


    pTask->m_nTaskStatus = M_TASK_STATUS_RUNNING;

    switch(pTask->m_nAction)
    {
        case M_SETPARAM_ACTION:
        {

        }
        break;

        case M_PROCESS_ACTION:
        {
            if(nProcessType == E_MEASUREPARAM_COEFF)
            {
                caclEnergyCoeff(pTask,pCommonInfo);
            }
            if(nProcessType == E_MEASUREPARAM_AREA)
            {
                caclAreaData(pTask,pCommonInfo);
            }
        }
        break;

    }

    return true;

}

/******************************************
* 功能:设备是否连接
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformDataProcessCtrl::checkDevStatus()
{
    return true;
}

//计算能量系数
void QPlatformDataProcessCtrl::caclEnergyCoeff(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{
    float lfMeasure0 = 0.0;
    float lfMeasure1 = 0.0;
    int nChannel0 = pTask->m_tParamData.value<st_dataprocessInput>().nParam1;//代表主能量计通道号
    int nChannel1 = pTask->m_tParamData.value<st_dataprocessInput>().nParam2;//代表监视能量计通道号
    for(int i = 0; i < pCommonInfo->tTaskDatalist.size(); i++)
    {
        tTaskData l_taskdata = pCommonInfo->tTaskDatalist[i];
        float lfvaule = l_taskdata.vardata.value<float>();
        if(l_taskdata.i32DevType == M_DEV_ENERGY)
        {
            if(l_taskdata.i32ChannelNo == nChannel0 && l_taskdata.i32DevNo == 0x0)
            {
                lfMeasure0 = lfMeasure0 + lfvaule;
            }
            if(l_taskdata.i32ChannelNo == nChannel1&& l_taskdata.i32DevNo == 0x0)
            {
                lfMeasure1 = lfMeasure1 + lfvaule;
            }
        }
    }

    if(lfMeasure1 > 0 && lfMeasure0 > 0)
    {
        float lfCoeff = lfMeasure0/lfMeasure1;//能量系数
        pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;

        tTaskData l_taskdata;
        l_taskdata.i32DevType = M_DEV_DATA_PROCESS;
        l_taskdata.i32DevNo = E_MEASUREPARAM_COEFF;
        l_taskdata.vardata = QVariant(lfCoeff);
        pCommonInfo->tTaskDatalist.append(l_taskdata);//存储分光比参数
        QVariant l_var;
        l_var.setValue(l_taskdata);

        emit signal_devdata(l_taskdata.i32DevType,l_taskdata.i32DevNo,l_var);
    }
    else
    {
        pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
        pTask->m_sTaskErrorInfo = "分光比计算失败";
    }



}


/******************************************
* 功能:计算光斑面积
* 输入:数据来源为kxccd文件中的数据
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QPlatformDataProcessCtrl::caclAreaData(TaskInfo *pTask,tCommonInfo* pCommonInfo)
{
    //查询数据库 获取数据
    pCommonInfo->_pDBProcess->getAllCCDImageToFile(pCommonInfo->sExpNo,M_AREA_MEASURE_FOLDER);

    //遍历文件夹下所有的文件
    //设置要遍历的目录
    QDir dir(M_AREA_MEASURE_FOLDER);
    //设置文件过滤器
    QStringList nameFilters;
    //设置文件过滤格式
    nameFilters << "*.bmp"<<"*.BMP";
    //将过滤后的文件名称存入到files列表中
    QStringList filelist = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

    float lfarea = 0.0;
    float lfMSum = 0.0;
    int nFileCnt = 0;
    for(QString sfilename:filelist)
    {

        double maxarea = 0.0;
        double l_lfgrayMax = 0.0;
        double l_lfgrayMin = 0.0;
        double l_lfres_grayMax = 0.0;
        double l_lfres_grayMin = 0.0;
        int l_res_pixel_num = 0;
        int l_pixel_num = 0;
        float lfM = 0.0;
        int maxindex = -1;


        QString sfilePath = M_AREA_MEASURE_FOLDER+QString("%1").arg(sfilename);

        QFile file(sfilePath);
        if (!file.exists())
        {
             continue;
        }

        cv::Mat image_src = cv::imread(sfilePath.toStdString(),cv::IMREAD_GRAYSCALE);

        cv::Mat tem_spot = cv::Mat::zeros(image_src.rows,image_src.cols,image_src.type());
        cv::Mat tem_back = cv::Mat::ones(image_src.rows,image_src.cols,image_src.type());


        cv::Mat ImageBinary;

        cv::Mat  mat_mean, mat_stddev;
        cv::meanStdDev(image_src, mat_mean, mat_stddev);//求灰度图像的均值、均方差
        float m = mat_mean.at<double>(0, 0);
        float s = mat_stddev.at<double>(0, 0);

        threshold(image_src,ImageBinary,m+s*1.5,255,cv::THRESH_BINARY);



//        threshold(image_src,ImageBinary,0,255,cv::THRESH_OTSU);//OTSU


        //查找轮廓
        vector<vector<cv::Point>> contours;
        vector<cv::Vec4i> hireachy;
        std::vector<cv::Point> tem_vp;
        std::vector<std::vector<cv::Point>> tem_vp2;
        findContours(ImageBinary,contours,hireachy,CV_RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE,cv::Point());



        for (size_t t = 0; t < contours.size();t++)
        {
            if (contourArea(contours[t]) > maxarea)
            {
                maxarea = contourArea(contours[t]) ;
                maxindex = static_cast<int>(t);
            }

        }

        if(maxindex == -1)
        {
            continue;
        }

        drawContours(tem_spot,contours,static_cast<int>(maxindex),cv::Scalar(1),CV_FILLED,8,hireachy);


        tem_back = tem_back - tem_spot;

        l_pixel_num = cv::sum(tem_spot)[0];
        l_res_pixel_num = cv::sum(tem_back)[0];

        cv::Mat tem_ori_spot = image_src.mul(tem_spot);
        cv::Mat tem_ori_back = image_src.mul(tem_back);

        if((float)tem_ori_spot.rows/(float)tem_ori_spot.cols > 5.0 ||  (float)tem_ori_spot.rows/(float)tem_ori_spot.cols < 0.2)
        {
           continue;
        }

        uint64 l_graySum = cv::sum(tem_ori_spot)[0];


        cv::minMaxIdx(tem_ori_spot,&l_lfgrayMin,&l_lfgrayMax);
        cv::minMaxIdx(tem_back,&l_lfres_grayMin,&l_lfres_grayMax);

        //计算面积
        float lftemparea =  l_graySum/l_lfgrayMax;

        //计算调制度
        uint64 l_res_graySum = cv::sum(tem_back)[0];
        double res_avg = (double)l_res_graySum/l_res_pixel_num;

        double upValue = (double)l_lfgrayMax - res_avg;
        double downValue = (double)l_graySum/l_pixel_num - res_avg;


        if(downValue != 0.0)
        {
           lfM = upValue/downValue;
        }


        //保存处理后的图片
        cv::Mat image_src_color = cv::imread(sfilePath.toStdString());
        QStringList l_tempfileName = sfilename.split('.');
        QString sSaveFilePath = M_AREA_MEASURE_FOLDER+l_tempfileName[0]+"_pro.bmp";
        cv::drawContours(image_src_color,contours,maxindex, cv::Scalar(255, 0, 0),2);

        cv::imwrite(sSaveFilePath.toStdString().c_str(),image_src_color);//保存处理后的图片

       //释放资源
        image_src_color.release();
        image_src.release();
        ImageBinary.release();
        tem_ori_spot.release();
        tem_ori_back.release();


        //显示数据
        st_ccdimage l_st_ccdimage;
        l_st_ccdimage.sImageFilePath = sSaveFilePath;
        l_st_ccdimage.i32ImageIndex = nFileCnt;

        QVariant var;
        var.setValue(l_st_ccdimage);
        emit this->signal_devdata(pTask->m_nDevType,E_MEASUREPARAM_AREA,var);


        lfarea = lfarea +  lftemparea*13*13*1e-8;
        lfMSum = lfMSum + lfM;//累加

        nFileCnt ++;

    }

    if(nFileCnt != 0)
    {
       lfarea = lfarea/nFileCnt;//平均面积
       lfMSum = lfMSum/nFileCnt;//平均调制度
       pTask->m_nTaskStatus = M_TASK_STATUS_SUCCESS;
    }
    else
    {
       pTask->m_nTaskStatus = M_TASK_STATUS_FAILD;
       pTask->m_sTaskErrorInfo = "光斑面积计算错误";
    }

    st_areaParam l_area_param;
    l_area_param.lfArea = lfarea;
    l_area_param.lfM = lfMSum;


    tTaskData l_taskdata;
    l_taskdata.i32DevType = M_DEV_DATA_PROCESS;
    l_taskdata.i32DevNo = E_MEASUREPARAM_AREA;
    l_taskdata.vardata.setValue(l_area_param);
    pCommonInfo->tTaskDatalist.append(l_taskdata);//存储光斑面积 用于显示

    return true;
}



/******************************************
* 功能:二值化算法
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QPlatformDataProcessCtrl::SauvolaThresh(const cv::Mat& src, cv::Mat& dst, const float k, const cv::Size wndSize)
{
    CV_Assert(src.type() == CV_8UC1);
    CV_Assert((wndSize.width % 2 == 1) && (wndSize.height % 2 == 1));
    CV_Assert((wndSize.width <= src.cols) && (wndSize.height <= src.rows));

    dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    // 产生标志位图像
    unsigned long* integralImg = new unsigned long[src.rows * src.cols];
    unsigned long* integralImgSqrt = new unsigned long[src.rows * src.cols];
    memset(integralImg, 0, src.rows *src.cols*sizeof(unsigned long));
    memset(integralImgSqrt, 0, src.rows *src.cols*sizeof(unsigned long));

    // 计算直方图和图像值平方的和
    for (int y = 0; y < src.rows; ++y)
    {
        unsigned long sum = 0;
        unsigned long sqrtSum = 0;
        for (int x = 0; x < src.cols; ++x)
        {
            int index = y * src.cols + x;
            sum += src.at<uchar>(y, x);
            sqrtSum += src.at<uchar>(y, x)*src.at<uchar>(y, x);
            if (y == 0)
            {
                integralImg[index] = sum;
                integralImgSqrt[index] = sqrtSum;
            }
            else
            {
                integralImgSqrt[index] = integralImgSqrt[(y - 1)*src.cols + x] + sqrtSum;
                integralImg[index] = integralImg[(y - 1)*src.cols + x] + sum;
            }
        }
    }

    double diff         = 0.0;
    double sqDiff       = 0.0;
    double diagSum      = 0.0;
    double iDiagSum     = 0.0;
    double sqDiagSum    = 0.0;
    double sqIDiagSum   = 0.0;
    for (int x = 0; x < src.cols; ++x)
    {
        for (int y = 0; y < src.rows; ++y)
        {
            int xMin = MAX(0, x - wndSize.width / 2);
            int yMin = MAX(0, y - wndSize.height / 2);
            int xMax = MIN(src.cols - 1, x + wndSize.width / 2);
            int yMax = MIN(src.rows - 1, y + wndSize.height / 2);
            double area = (xMax - xMin + 1)*(yMax - yMin + 1);
            if (area <= 0)
            {
                // blog提供源码是biImage[i * IMAGE_WIDTH + j] = 255;但是i表示的是列, j是行
                dst.at<uchar>(y, x) = 255;
                continue;
            }

            if (xMin == 0 && yMin == 0)
            {
                diff = integralImg[yMax*src.cols + xMax];
                sqDiff = integralImgSqrt[yMax*src.cols + xMax];
            }
            else if (xMin > 0 && yMin == 0)
            {
                diff = integralImg[yMax*src.cols + xMax] - integralImg[yMax*src.cols + xMin - 1];
                sqDiff = integralImgSqrt[yMax * src.cols + xMax] - integralImgSqrt[yMax * src.cols + xMin - 1];
            }
            else if (xMin == 0 && yMin > 0)
            {
                diff = integralImg[yMax * src.cols + xMax] - integralImg[(yMin - 1) * src.cols + xMax];
                sqDiff = integralImgSqrt[yMax * src.cols + xMax] - integralImgSqrt[(yMin - 1) * src.cols + xMax];;
            }
            else
            {
                diagSum = integralImg[yMax * src.cols + xMax] + integralImg[(yMin - 1) * src.cols + xMin - 1];
                iDiagSum = integralImg[(yMin - 1) * src.cols + xMax] + integralImg[yMax * src.cols + xMin - 1];
                diff = diagSum - iDiagSum;
                sqDiagSum = integralImgSqrt[yMax * src.cols + xMax] + integralImgSqrt[(yMin - 1) * src.cols + xMin - 1];
                sqIDiagSum = integralImgSqrt[(yMin - 1) * src.cols + xMax] + integralImgSqrt[yMax * src.cols + xMin - 1];
                sqDiff = sqDiagSum - sqIDiagSum;
            }
            double mean = diff / area;
            double stdValue = sqrt((sqDiff - diff*diff / area) / (area - 1));
            double threshold = mean*(1 + k*((stdValue / 128) - 1));
            if (src.at<uchar>(y, x) < threshold)
            {
                dst.at<uchar>(y, x) = 255;
            }
            else
            {
                dst.at<uchar>(y, x) = 0;
            }

        }
    }

    delete[] integralImg;
    delete[] integralImgSqrt;
}
