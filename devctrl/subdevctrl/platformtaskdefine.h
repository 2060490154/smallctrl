/**************************************************
*文件说明:本文件定义平台与任务相关的结构数据
*创建人:
*维护记录:
*
******************************************************/
#ifndef PLATFORMDEVINPUTDEFINE_H
#define PLATFORMDEVINPUTDEFINE_H
#include <QObject>
using namespace std;

#define M_AREA_MEASURE_FOLDER  (QApplication::applicationDirPath()+"/data/area/")

#define M_MAX_WAIT_TIME_CCD  (2*60*1000)

#define M_MONIT_ENERGY_SAMPLE  0.04 //监视能量计光路取样系数
#define M_MAIN_ENERGY_SAMPLE  1 //主能量计光路取样系数

//能量计设备执行任务时输入数据
typedef struct _EnergyDevTaskInputData
{
    //设置时使用的参数
    int nChannelNo = 0;//0-测量 1-监视
    int nWavelength = 1064;//测量波长
    int nMeasureRange = 19;//测量范围 需要根据预期能量修改

    bool bInitMode = false;

    //采集时使用的参数
    int ncapDataCnt = 0x01;//采集能量个数,分光比测试时可采集多个数据
    int nPointNo = -1;//点号
    int nShotIndex = 0;//发次编号 ron1下 同一个点号 多个发次
    bool bSaveData = false;//是否存储数据库
    //int nEnergySection;
    float lfEnergySection;//当前能量衰减值 xx% 存储数据库时使用 数据统计时使用
    bool bFirstPoint = false;//第一个点 由于光闸的原因 最长可到2秒

    float lfCoeff;//能量计修正系数

    int caclMeasureRange(float lfEnergy,float lfEnergyPecent,float lfK)
    {
        float lfCurrentEnergy = lfEnergy*lfEnergyPecent*lfK/100.0;
        if(lfCurrentEnergy <=3.0)
        {
            return 19;
        }
        else if(lfCurrentEnergy <=10.0)
        {
            return 20;
        }
        else if(lfCurrentEnergy <=30.0)
        {
            return 21;
        }
        else if(lfCurrentEnergy <=100.0)
        {
            return 22;
        }
        else if(lfCurrentEnergy <=300.0)
        {
            return 23;
        }
        else if(lfCurrentEnergy <=1000.0)
        {
            return 24;
        }
        else if(lfCurrentEnergy <=3000.0)
        {
            return 25;
        }
        return 19;
    }

}st_EnergyDevTaskInputData;
Q_DECLARE_METATYPE(st_EnergyDevTaskInputData)


typedef struct _shutterparam
{
    int i32Mode;//1-正常 2-流程模式
    int i32Param;//mode==1 param=0 关闭 param=1打开；mode=2 param为放过的脉冲数

}st_shutterparam;
Q_DECLARE_METATYPE(st_shutterparam)

typedef struct _ccdimage
{
    QString sImageFilePath;
    int i32ImageIndex;
}st_ccdimage;
Q_DECLARE_METATYPE(st_ccdimage)

typedef struct _ccdinput
{
    int nImageCnt;//总共采集图像的数量
    int nActionType;//0x02--计算光斑面积
}st_ccdinputdata;
Q_DECLARE_METATYPE(st_ccdinputdata)

typedef struct _motorInputdata
{
    int nMotorIndex;//电机编号
    int nMoveType;//0x01-档位 0x02-相对运动 0x03-绝对运动
    int nPlusCnt;//运动步数或档位信息
}st_motorInputdata;
Q_DECLARE_METATYPE(st_motorInputdata)

typedef struct _dataprocessInput
{
    int nProcessType;//处理类型
    int nParam1;//参数1--能量系数计算时 参数1代表主能量计通道 参数2代表监视能量计通道
    int nParam2;//参数2
}st_dataprocessInput;
Q_DECLARE_METATYPE(st_dataprocessInput)


enum DefectcheckParamType
{
    E_PARAM_PUBLISH = 0x01,//系统初始化 发布实验信息
    E_PARAM_PREPARE = 0x02,//准备
    E_PARAM_CAPIMAGE = 0x03,//查询图像捕获状态
    E_PARAM_DEFECT_REL = 0x4,//获取损伤检测结果
    E_PARAM_DEFECT_CLOSE = 0x5,//获取损伤检测结果

};

typedef struct _defectcheckInputdata
{
    int nParamType;//命令编号  参见DefectcheckParamType
    int nPointNo;//点号
    int nShotNo;//发次号
    float lfEnergySection;
}st_defectcheckInputdata;
Q_DECLARE_METATYPE(st_defectcheckInputdata)


//存储的数据信息 数据处理时使用
//typedef struct _dataInfo
//{
//    int nPointNo;
//    int nShotNo;
//    int i32EnergySection;//能量段信息
//    float lfEnergydata;//能量数据
//    float lfFluxdata;//通量数据
//    bool bDefect;//是否损伤
//}st_pointdataInfo;

typedef struct _areaparam
{
    float lfM;
    float lfArea;
}st_areaParam;
Q_DECLARE_METATYPE(st_areaParam)


//数据处理时使用
typedef struct _shotdataInfo
{
    int nShotNo;
    float lfEnergySection;//能量段信息 暂时没有使用
    float lfEnergydata;//能量数据
    float lfFluxdata;//通量数据
    bool bDefect;//是否损伤
}st_shotdataInfo;
typedef struct point_dataInfo
{
    int nPointNo;
    float lfMaxFlux = 0.0;//该点 多个发次中 最大的通量
    float lfMinFlux = 65535.0;
    bool bMaxfluxdefect;//最大发次时是否损伤
   vector<st_shotdataInfo> shotdatalist;
}st_pointdataInfo;

//实验数据
typedef struct _expDataInfo
{
    QString sExpNo;//实验编号
    QString sTaskNo;
    QString sSampleName;
    int nWavelengthType;
    float lfArea;//光斑面积
    float lfSampleCoeff;//能量系数
    float lfAngle;//测试角度
    float lfPlusWidth;//脉宽
    int   nMeasureType;//实验类型
    float lfM;//调制度

    float lfExphum;//湿度
    float lfExpdirty;//洁净度
    float lfExpTemp;//温度

    float lfTargetPlus;//图纸要求脉冲宽度
    float lfTargetAngle;//图纸要求测试角度
    QString sTargettypename;//测试样品类型
    vector<st_pointdataInfo> pointdatalist;
}st_expDataInfo;

typedef struct _damageInfo
{
    float lfStartFlux;
    float lfEndFlux;//当前通量段
    float lfFlux;//平均通量
    float lfDamagePro;//损伤概率

}st_damageInfo;


//1on1流程数据处理  能量段数据
typedef struct _energySectionData
{
    float lfFluxSum = 0.0;
    float lfEnergySum = 0.0;
    float lfEnergyRms = 0.0;

    float lfdefectPro = 0.0;//损伤概率
    float lfFluxAvg = 0.0;//计算平均通量
    float lfEnergyAvg = 0.0;//计算平均能量
    int ndefectNum = 0;
    int nPointNum = 0;
    float lfEnergySection = 0.0;
}st_energySectionData;
Q_DECLARE_METATYPE(st_energySectionData)


enum motorDefine
{
    E_MOTOR_X = 0x00,//载物台X 水平轴
    E_MOTOR_Y = 0x01,//载物台Y 垂直轴
    E_MOTOR_Z = 0x02,//载物台Z 旋转台
    E_MOTOR_NEARFIELD_WHEEL = 0x3,//近场衰减轮盘
    E_MOTOR_LIGHT_CHANGE = 0x04,//光路切换平移台 355nm时切入 1064nm时退出
    E_MOTOR_PLATFORM = 0x05,//光路调节平移台
};

typedef struct workflowParam
{
    int m_nMeasureType;//1--1 On 1   ;2-Ron1
    //实验信息
    QString m_sSampleName;
    //扫描区域信息
    double m_dWidth;
    double m_dHigh;

    double m_dStep;//扫描步长

    //能量相关设置
    int m_nWaveLengthType;//0-1064nm 1-355nm
    int m_n1On1PointNum;//1On1模式下 一个能量打多少发
    double m_lfEnergyIncream;//能量调节步长
    double m_lfMinimumEnergy;//最小能量
    double m_lfMaximumEnergy;//最大能量
    int m_nEnergySectionNum;//能量段数
    double m_dManualArea[2];
    double m_dEnergyCoeff[2];//能量系数 分光比
    double m_dM[2];
    double m_dPlusWidth[2];//脉宽
    double m_lfAngle;//测试角度
    QString m_sLaserName[2];//光路名称

    double m_dMinDeg;
    double m_dMaxDeg;


    QString m_sExpTaskNo;//任务编号
    float m_lfExpTemp;//实验温度
    float m_lfExphum;//实验湿度
    float m_lfExpdirty;//实验洁净度

    float m_lfLaserEnergy;//激光器输出能量


}tWorkflowParam;
Q_DECLARE_METATYPE(tWorkflowParam)



enum _MEASURETYPE {
    M_MEASURETYPE_NONE = 0,
    M_MEASURETYPE_1On1 = 1,
    M_MEASURETYPE_ROn1 = 2,
    M_MEASURETYPE_LASERPARAM = 3, //光路参数测量 包括面积、脉宽
    M_MEASURE_MAX_ENERGY_CONFIRM = 4//最大能量确认 手动流程

};

enum _MEASURE_PARAM_TYPE {
    E_MEASUREPARAM_1On1 = 1,
    E_MEASUREPARAM_ROn1 = 2,
    E_MEASUREPARAM_AREA = 3,//面积测量
    E_MEASUREPARAM_COEFF = 4,//能量系数标定
    E_MEASUREPARAM_PLUS_WIDTH = 0x05//脉宽
};


#endif // PLATFORMDEVINPUTDEFINE_H
