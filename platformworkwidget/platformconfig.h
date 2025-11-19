#ifndef QPLATFORMCONFIG_H
#define QPLATFORMCONFIG_H

#include <QObject>
#include <QtXml>
#include <QDomElement>
using namespace std;

#define M_LASER_CNT 2

typedef struct _MotorGear
{
    float m_lfminEnergy;//最小能量
    float m_lfMaxEnergy;//最大能量
    int   m_nGearIndex;//档位信息
}tMotorGear;

typedef struct _WheelGear
{
    int m_nDevIndex;//控制卡编号
    int m_nChannelIndex;//电机编号
    vector<tMotorGear> m_tMotorGearlist;
}tWheelGear;

typedef struct _LaserParam
{
    float m_lfK;//光路系数
    float m_lfPlusWidth;//脉宽数据
    float m_lfArea;//光斑面积
    float m_lfM;//调制度

    QString m_sLaserName;
}tLaserParam;
typedef struct  _tPlatformConfig
{

    int m_nCurrentWaveLength;//0-1064nm  当前系统处于的波长状态
    tLaserParam m_st_laserParam[M_LASER_CNT];

    vector<tWheelGear> m_tWheelGear;//轮盘档位与能量的对应关系

    int getGearIndex(int nDevIndex,int nMotorIndex,float lfEnergy)//根据能量设置档位信息
    {
        foreach (tWheelGear wheelGrear, m_tWheelGear) {

            if(wheelGrear.m_nDevIndex == nDevIndex && wheelGrear.m_nChannelIndex == nMotorIndex)
            {
                foreach (tMotorGear motorGearInfo, wheelGrear.m_tMotorGearlist) {

                    if(lfEnergy >= motorGearInfo.m_lfminEnergy && lfEnergy <= motorGearInfo.m_lfMaxEnergy)
                    {
                        return motorGearInfo.m_nGearIndex;
                    }

                }
            }

        }

        return 0x05;//衰减最大档位

    }

}tPlatformConfig;

class QPlatformConfig : public QObject
{
    Q_OBJECT
public:
    explicit QPlatformConfig(QObject *parent = nullptr);

signals:

    void signal_updateshowParam();

public slots:

    bool onUpdatePlatConfigfile();

public:

    bool loadPlatformConfig(QString sfilePath);

    tPlatformConfig m_tPlatformConfig;

private:
    QString _sConfigfilePath;
};

#endif // QPLATFORMCONFIG_H
