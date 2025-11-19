#include "motordevctrl.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")

#endif



QMotorDevCtrl::QMotorDevCtrl():QMotorDevCtrlBasic()
{
    _nCardNo = M_MOTOR_CARD;

    m_bConnect = false;

    _bStopMotorMove = false;

    //初始化伺服电机参数 目前只使用kd

    memset(&_stServoMotorCtrlData,0,sizeof(servomotorctrldata)*2);
    _stServoMotorCtrlData[0].lfKp = 30;
    _stServoMotorCtrlData[1].lfKp = 100;

    _stServoMotorCtrlData[0].lfKd = 0;
    _stServoMotorCtrlData[1].lfKd = 0;

    _stServoMotorCtrlData[0].lfKi = 0;
    _stServoMotorCtrlData[1].lfKi = 0;


    InitErrorInfo();

    _initCard();

}

QMotorDevCtrl::~QMotorDevCtrl()
{

    for(int i = 0; i < 6; i++)
    {
       stopMove(i);
    }

    QThread::msleep(100);
}

/******************************************
* 功能:运动至当前档位
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QMotorDevCtrl::moveToGear(int nMotorIndex,int nGearIndex)
{
    bool bRel = false;
    //开始运动 发送当前状态
    m_tMotorStatus[nMotorIndex].sErroInfo = "";
    m_tMotorStatus[nMotorIndex].nMotorStatus = 0x01;
    _sendMotorStatus(nMotorIndex);

    short l_errcode  =_initCard();
    if(l_errcode != ERR_NOERR)//初始化板卡
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }

    _bStopMotorMove = false;
    //先回零运动
    l_errcode= _moveToHome(nMotorIndex);
    if(l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }


    int nPlusCnt = _getMotorGearConfig(nMotorIndex,nGearIndex);

    qDebug()<<"***"<<nPlusCnt<<"  "<<nMotorIndex;
    if(nMotorIndex !=E_MOTOR_X && nMotorIndex !=E_MOTOR_Y)//步进电机
    {
        l_errcode = _stepmotor_moveMotorToRelationPos(nMotorIndex,nPlusCnt);
    }
    else//伺服电机
    {
        l_errcode = _seromotor_moveToPos(nMotorIndex,nPlusCnt);//相对位置运动
    }

    if(l_errcode != ERR_NOERR)//运动失败
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        bRel = false;
    }
    else
    {
        //运动结束 发送当前状态
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x02;
        bRel = true;
    }

    _sendMotorStatus(nMotorIndex);

    return bRel;
}
/******************************************
* 功能:运动到指定位置  相对运动
* 输入:npos为输入的脉冲数 向左运动置为负
* 输出:运动是否成功
* 返回值:
* 维护记录:
******************************************/
bool QMotorDevCtrl::moveToRelationPos(int nMotorIndex,int nPlusCnt)
{
    bool bRel = false;
    //开始运动 发送当前状态
    m_tMotorStatus[nMotorIndex].sErroInfo = "";
    m_tMotorStatus[nMotorIndex].nMotorStatus = 0x01;
    _sendMotorStatus(nMotorIndex);

    short l_errcode  =_initCard();
    if(l_errcode != ERR_NOERR)//初始化板卡
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }

    _bStopMotorMove = false;

    if(nMotorIndex !=E_MOTOR_X && nMotorIndex !=E_MOTOR_Y)//步进电机
    {
        l_errcode = _stepmotor_moveMotorToRelationPos(nMotorIndex,nPlusCnt);
    }
    else//伺服电机
    {
        l_errcode = _seromotor_moveToPos(nMotorIndex,nPlusCnt);//相对位置运动
    }

    if(l_errcode != ERR_NOERR)//运动失败
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        bRel = false;
    }
    else
    {
        //运动结束 发送当前状态
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x02;
        bRel = true;
    }

    _sendMotorStatus(nMotorIndex);

    return bRel;
}
/******************************************
* 功能:运动到指定位置  相对运动
* 输入:npos为输入的脉冲数 向左运动置为负
* 输出:运动是否成功
* 返回值:
* 维护记录:
******************************************/
bool QMotorDevCtrl::moveToAbsolutePos(int nMotorIndex,int nPlusCnt)
{
    bool bRel = false;
    //开始运动 发送当前状态
    m_tMotorStatus[nMotorIndex].sErroInfo = "";
    m_tMotorStatus[nMotorIndex].nMotorStatus = 0x01;
    _sendMotorStatus(nMotorIndex);

    short l_errcode  =_initCard();
    if(l_errcode != ERR_NOERR)//初始化板卡
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }


    _bStopMotorMove = false;
    //先回零运动
    l_errcode= _moveToHome(nMotorIndex);
    if(l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }


    if(nMotorIndex !=E_MOTOR_X && nMotorIndex !=E_MOTOR_Y)//步进电机
    {
        l_errcode = _stepmotor_moveMotorToRelationPos(nMotorIndex,nPlusCnt);
    }
    else//伺服电机
    {
        l_errcode = _seromotor_moveToPos(nMotorIndex,nPlusCnt);//相对位置运动
    }

    if(l_errcode != ERR_NOERR)//运动失败
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        bRel = false;
    }
    else
    {
        //运动结束 发送当前状态
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x02;
        bRel = true;
    }

    _sendMotorStatus(nMotorIndex);

    return bRel;
}
/******************************************
* 功能:发送电机当前状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QMotorDevCtrl::moveToHome(int nMotorIndex)
{
    bool bRel = false;
    //开始运动 发送当前状态
    m_tMotorStatus[nMotorIndex].sErroInfo = "";
    m_tMotorStatus[nMotorIndex].nMotorStatus = 0x01;
    _sendMotorStatus(nMotorIndex);

    short l_errcode  =_initCard();
    if(l_errcode != ERR_NOERR)//初始化板卡
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }

    _bStopMotorMove = false;

    //先回零运动
    l_errcode= _moveToHome(nMotorIndex);
    if(l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        bRel =  false;
    }
    else
    {
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x02;
        bRel = true;
    }

    _sendMotorStatus(nMotorIndex);

    return bRel;
}



/******************************************
* 功能:检查电机是否正在运动
* 输入:
* 输出:true--正在运动 false--未运动
* 返回值:
* 维护记录:
******************************************/
bool QMotorDevCtrl::checkMotorRunning(int nMotorIndex)
{
    if(m_tMotorStatus[nMotorIndex].nMotorStatus != 0x01)
    {
        return false;
    }
    return true;

}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool QMotorDevCtrl::stopMove(int nMotorIndex)
{   
    if(m_bConnect == false)
    {
        return true;

    }
    _CardLock.lock();
    _bStopMotorMove = true;
    bool bRel = dmc_stop(_nCardNo,nMotorIndex,0x01);//立即停止
    _CardLock.unlock();
    return bRel;

}

/******************************************
* 功能:初始化板块
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
short QMotorDevCtrl::_initCard()
{

    short l_nErrorCode;
    WORD l_cardNum,l_cardIdList;
    DWORD l_cardTypeList;

    if(m_bConnect == true)
    {
        return ERR_NOERR;
    }

    _CardLock.lock();
    if( dmc_board_init() <= 0 )
    {
        _CardLock.unlock();
        return ERR_CANNOT_CONNECTETH;
    }

    l_nErrorCode = dmc_get_CardInfList(&l_cardNum,&l_cardTypeList,&l_cardIdList);

    if(l_cardNum == 0)
    {
        _CardLock.unlock();
        m_bConnect = false;
        return ERR_CANNOT_CONNECTETH;
    }

    if(l_nErrorCode == ERR_NOERR)
    {
        m_bConnect = true;
        //初始化参数
        for(int i = 0; i < 6; i++)
        {
            int l_ORG_level = 1;//原点信号有效电平 0-低有效 1-高有效
            dmc_set_pulse_outmode(_nCardNo, i, 0);//脉冲输出模式 根据驱动器接收脉冲的模式设置
            dmc_set_home_pin_logic(_nCardNo, i, l_ORG_level,0);//原点信号低电平有效

            if(i== E_MOTOR_Z || i== E_MOTOR_NEARFIELD_WHEEL)
            {
                //设置速度曲线参数 轮盘速度要低一些
               dmc_set_el_mode(_nCardNo,i,0x00,0x01,0x0);//轮盘 限位无效
            }
            else
            {
                dmc_set_el_mode(_nCardNo,i,0x01,0x01,0x0);//0x01-正负限位允许，0x01-正负限位高电平有效 0x0-立即停止
            }

            InitProfile(i);//设置速度曲线
        }
    }

    _CardLock.unlock();

    return l_nErrorCode;
}

/******************************************
* 功能:发送电机当前状态
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMotorDevCtrl::_sendMotorStatus(int nMotorIndex)
{
    QVariant l_var;
    l_var.setValue(m_tMotorStatus[nMotorIndex]);
    emit signal_updateMotorStatus(nMotorIndex,l_var);
}
/******************************************
* 功能:相对位置运动
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
short QMotorDevCtrl::_stepmotor_moveMotorToRelationPos(int nMotorIndex,long nPlusCnt)
{

    _CardLock.lock();
    getMotorStatus(nMotorIndex);
    _CardLock.unlock();

    if(m_tMotorStatus[nMotorIndex].isRightLimit == true && nPlusCnt > 0)
    {
        return ERR_MOTOR_LIMITED;
    }
    else if(m_tMotorStatus[nMotorIndex].isLeftLimit == true && nPlusCnt < 0)
    {
        return ERR_MOTOR_LIMITED;
    }


    long l_currentPosion_bak = m_tMotorStatus[nMotorIndex].nMotorCurrentPos;//获取当前位置信息

    _CardLock.lock();
    short l_nErrorCode = dmc_pmove(_nCardNo,nMotorIndex,nPlusCnt,0);//0--相对位置运动
    _CardLock.unlock();
    if(l_nErrorCode != ERR_NOERR)
    {
        return l_nErrorCode;
    }

    //监视是否运动到位
    QTime l_time;
    l_time.start();
    while(l_time.elapsed()< M_MAX_WAIT_TIME && _bStopMotorMove==false)
    {
        _CardLock.lock();
        DWORD status = dmc_check_done(_nCardNo, nMotorIndex);//1-静止 0-正在运动
        getMotorStatus(nMotorIndex);//获取当前电机状态
        _CardLock.unlock();

        if(m_tMotorStatus[nMotorIndex].isLeftLimit && nPlusCnt < 0)
        {
            return ERR_MOTOR_LIMITED;
        }

        if(m_tMotorStatus[nMotorIndex].isRightLimit && nPlusCnt > 0)
        {
            return ERR_MOTOR_LIMITED;
        }

        int l_pos_err_cnt  = abs(abs(l_currentPosion_bak - m_tMotorStatus[nMotorIndex].nMotorCurrentPos) - abs(nPlusCnt));

        //静止 且位置正常
        if(l_pos_err_cnt <= 20 && status == 1)
        {
            return ERR_NOERR;
        }
        else if(l_pos_err_cnt > 20 && status == 1)
        {
            return ERR_POS_SUB_LIMIT;
        }
        QThread::msleep(10);
        _sendMotorStatus(nMotorIndex);
    }


    if(_bStopMotorMove)
    {
        return ERR_MOTOR_STOP;
    }

    return ERR_TIMEOUT;
}

/******************************************
* 功能:回零运动
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
short QMotorDevCtrl::_moveToHome(int nMotorIndex)
{
    int l_home_dir = 0;//回零方向 0-负方向 1-正方向
    int l_vel_mode = 1;//回零速度  0-低速回零 1-高速回零 dmc_set_profile的最低速、最高速
    int l_gohome_mode = 1;//0-一次回零 1-一次回零加回找

    if(nMotorIndex == E_MOTOR_LIGHT_CHANGE || nMotorIndex == E_MOTOR_PLATFORM)
    {
        l_home_dir = 0x01;
        l_gohome_mode = 0x0;
    }

    //设置回零模式
    InitProfile(nMotorIndex);

    _CardLock.lock();
    short l_nErrorCode = dmc_set_homemode(_nCardNo,nMotorIndex,l_home_dir,l_vel_mode,l_gohome_mode,0);
    _CardLock.unlock();

    if(l_nErrorCode != ERR_NOERR)
    {
        return l_nErrorCode;
    }

    //开启回零
    _CardLock.lock();
    l_nErrorCode = dmc_home_move(_nCardNo,nMotorIndex);
    _CardLock.unlock();
    if(l_nErrorCode != ERR_NOERR)
    {
        return l_nErrorCode;
    }

    //监视回零是否到位
    WORD l_status = 0;
    QTime l_time;
    l_time.start();
    while(l_time.elapsed()< M_MAX_WAIT_TIME && _bStopMotorMove == false)
    {
        _CardLock.lock();
        l_nErrorCode = dmc_get_home_result(_nCardNo,nMotorIndex,&l_status);//获取回零状态
        _CardLock.unlock();

        if(l_nErrorCode != ERR_NOERR)
        {
            return l_nErrorCode;
        }

        _CardLock.lock();
        getMotorStatus(nMotorIndex);//获取当前电机状态
        _CardLock.unlock();

        if(l_status == 0x01)//1-回零完成 0-回零未完成
        {
            QThread::msleep(100);
            _CardLock.lock();
            dmc_set_position(_nCardNo,nMotorIndex,0);//位置数据清零
            dmc_set_encoder(_nCardNo,nMotorIndex,0);//位置数据清零
            getMotorStatus(nMotorIndex);
            _CardLock.unlock();

            _sendMotorStatus(nMotorIndex);//更新状态
            break;
        }

        _sendMotorStatus(nMotorIndex);
        QThread::msleep(10);
    }

    if(l_time.elapsed() > M_MAX_WAIT_TIME)
    {
        return ERR_TIMEOUT;
    }

    if(_bStopMotorMove)
    {
        return ERR_MOTOR_STOP;
    }


    return ERR_NOERR;
}

/******************************************
* 功能:伺服电机控制 pid控制
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
short QMotorDevCtrl::_seromotor_moveToPos(int nMotorIndex,int nPlusCnt)
{
    QTime l_time2;
    l_time2.start();

    //设置s曲线
    InitProfile(nMotorIndex);

    servomotorctrldata l_servomotorctrldata;

    if(nMotorIndex == E_MOTOR_X)
    {
        l_servomotorctrldata = _stServoMotorCtrlData[0];
    }
    if(nMotorIndex == E_MOTOR_Y)
    {
        l_servomotorctrldata = _stServoMotorCtrlData[1];
    }

    l_servomotorctrldata.lfUiData = 0.0;
    l_servomotorctrldata.lfUiData_bak = 0.0;
    l_servomotorctrldata.lfUdData = 0.0;
    l_servomotorctrldata.lfUdData_bak = 0.0;



    _CardLock.lock();
    getMotorStatus(nMotorIndex);
    _CardLock.unlock();


    long l_currentPosion_bak = m_tMotorStatus[nMotorIndex].nMotorCurrentPos;//获取当前位置信息
    m_tMotorStatus[nMotorIndex].nMotorTargetPos = nPlusCnt;

    //监视是否运动到位
    QTime l_time;
    l_time.start();
    int l_nStartTime = l_time.elapsed();
    int l_nEndStartTime = l_time.elapsed();

    _CardLock.lock();
    if(nPlusCnt >0)
    {
        dmc_vmove(_nCardNo,nMotorIndex,1);
    }
    else
    {
        dmc_vmove(_nCardNo,nMotorIndex,0);
    }

    _CardLock.unlock();


    int l_deta_e_bak = 0;
    double l_output = 0;

    while(l_time.elapsed()< M_MAX_WAIT_TIME && _bStopMotorMove==false)
    {
        _CardLock.lock();
        getMotorStatus(nMotorIndex);//获取当前电机状态
        _CardLock.unlock();

        if(m_tMotorStatus[nMotorIndex].isLeftLimit && nPlusCnt < 0)
        {
            return ERR_MOTOR_LIMITED;
        }

        if(m_tMotorStatus[nMotorIndex].isRightLimit && nPlusCnt > 0)
        {
            return ERR_MOTOR_LIMITED;
        }



        if(l_nEndStartTime - l_nStartTime >= 10)
        {

            //控制
            int l_current_pos = m_tMotorStatus[nMotorIndex].nMotorCurrentPos;
            int l_target_pos = m_tMotorStatus[nMotorIndex].nMotorTargetPos;

            int l_move_dis = l_current_pos - l_currentPosion_bak;

            int l_deta_e = l_target_pos-l_move_dis;


            if(abs(l_deta_e) <= 1 && abs(m_tMotorStatus[nMotorIndex].lfCurrentSpeed)< 100.0)
            {
                dmc_stop(_nCardNo,nMotorIndex,0x01);
                break;
            }

            l_servomotorctrldata.lfUdData = l_servomotorctrldata.lfKd*(l_deta_e - l_deta_e_bak);
            l_servomotorctrldata.lfUiData = l_servomotorctrldata.lfUiData_bak+ l_servomotorctrldata.lfKi*l_deta_e;

            l_deta_e_bak = l_deta_e;
            l_servomotorctrldata.lfUiData_bak = l_servomotorctrldata.lfUiData;




            l_output = l_servomotorctrldata.lfKp*l_deta_e+l_servomotorctrldata.lfUiData+l_servomotorctrldata.lfUdData;

            //目前没有用到 ki kd 只用到kp
//            if(abs(l_output) > 1000)
//            {
//                l_servomotorctrldata.lfKi = 0;
//                l_servomotorctrldata.lfUiData_bak = 0;
//            }
//            else
//            {
//                l_servomotorctrldata.lfKi = 0;
//            }

            if(abs(l_output) > 10000)
            {
                l_output = l_output >0?10000:-10000;
            }

           // qDebug()<<l_time2.elapsed()<<l_output<<l_deta_e<<m_tMotorStatus[nMotorIndex].lfCurrentSpeed;

            _CardLock.lock();
            dmc_change_speed(_nCardNo,nMotorIndex,l_output,0);
            _CardLock.unlock();

            _sendMotorStatus(nMotorIndex);

            l_nStartTime = l_time.elapsed();
            l_nEndStartTime = l_time.elapsed();

        }
        else
        {
            l_nEndStartTime = l_time.elapsed();
        }

    }


    if(l_time.elapsed() > M_MAX_WAIT_TIME)
    {
       return ERR_TIMEOUT;
    }

    if(_bStopMotorMove)
    {
        return ERR_MOTOR_STOP;
    }

    getMotorStatus(nMotorIndex);//获取当前电机状态

    qDebug()<<"*******"<<l_time2.elapsed()<<m_tMotorStatus[nMotorIndex].nMotorCurrentPos - l_currentPosion_bak-m_tMotorStatus[nMotorIndex].nMotorTargetPos;


    return ERR_NOERR;
}
/******************************************
* 功能:伺服电机控制
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
short QMotorDevCtrl::_seromotor_moveToPos1(int nMotorIndex,int nPlusCnt)
{
    _CardLock.lock();
    getMotorStatus(nMotorIndex);
    _CardLock.unlock();
    if(m_tMotorStatus[nMotorIndex].isRightLimit == true && nPlusCnt > 0)
    {
        return ERR_MOTOR_LIMITED;
    }
    else if(m_tMotorStatus[nMotorIndex].isLeftLimit == true && nPlusCnt < 0)
    {
        return ERR_MOTOR_LIMITED;
    }


    long l_currentPosion_bak = m_tMotorStatus[nMotorIndex].nMotorCurrentPos;//获取当前位置信息

     qDebug()<<"***"<<m_tMotorStatus[nMotorIndex].nMotorCurrentPos;
    _CardLock.lock();
    short l_nErrorCode = dmc_pmove(_nCardNo,nMotorIndex,nPlusCnt,0);//0--相对位置运动
    _CardLock.unlock();
    if(l_nErrorCode != ERR_NOERR)
    {
        return l_nErrorCode;
    }

    //监视是否运动到位
    QTime l_time;
    l_time.start();
    while(l_time.elapsed()< M_MAX_WAIT_TIME && _bStopMotorMove==false)
    {
        _CardLock.lock();
        DWORD status = dmc_check_done(_nCardNo, nMotorIndex);//1-静止 0-正在运动
        getMotorStatus(nMotorIndex);//获取当前电机状态
        _CardLock.unlock();

        int l_pos_err_cnt  = abs(abs(l_currentPosion_bak - m_tMotorStatus[nMotorIndex].nMotorCurrentPos) - abs(nPlusCnt));

        //静止 且位置正常
        if(l_pos_err_cnt <= 20 && status == 1)
        {
            qDebug()<<l_pos_err_cnt<<l_currentPosion_bak<<m_tMotorStatus[nMotorIndex].nMotorCurrentPos<<nPlusCnt;
            return ERR_NOERR;
        }
        else if(l_pos_err_cnt > 20 && status == 1)
        {
            qDebug()<<l_pos_err_cnt<<l_currentPosion_bak<<m_tMotorStatus[nMotorIndex].nMotorCurrentPos<<nPlusCnt;

            return ERR_POS_SUB_LIMIT;
        }
        QThread::msleep(10);
        _sendMotorStatus(nMotorIndex);
    }


    if(_bStopMotorMove)
    {
        return ERR_MOTOR_STOP;
    }

    return ERR_TIMEOUT;
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMotorDevCtrl::getMotorStatus(int nMotorIndex)
{
    long l_currentPosion = dmc_get_position(_nCardNo,nMotorIndex);//获取当前位置

    double l_current_speed = dmc_read_current_speed(_nCardNo,nMotorIndex);

    DWORD l_status = dmc_axis_io_status(_nCardNo,nMotorIndex);
    st_motorstatus* l_motorStatus = (st_motorstatus*)&l_status;
    m_tMotorStatus[nMotorIndex].isLeftLimit = l_motorStatus->leftlimit;
    m_tMotorStatus[nMotorIndex].isRightLimit = l_motorStatus->rightlimit;
    m_tMotorStatus[nMotorIndex].lfCurrentSpeed = l_current_speed;

    if(nMotorIndex == E_MOTOR_X || nMotorIndex == E_MOTOR_Y)
    {
        m_tMotorStatus[nMotorIndex].nMotorCurrentPos = dmc_get_encoder(_nCardNo,nMotorIndex)*(-1);
    }
    else
    {
        m_tMotorStatus[nMotorIndex].nMotorCurrentPos = l_currentPosion;
    }


}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void QMotorDevCtrl::InitErrorInfo()
{
    _errorCodeInfolist.insert(ERR_NOERR,"无错误");
    _errorCodeInfolist.insert(ERR_UNKNOWN,"未知错误");
    _errorCodeInfolist.insert(ERR_PARAERR,"参数错误");
    _errorCodeInfolist.insert(ERR_TIMEOUT,"操作超时");
    _errorCodeInfolist.insert(ERRORERR_CONTROLLERBUSY,"控制卡状态忙");
    _errorCodeInfolist.insert(ERR_CANNOT_CONNECTETH,"控制设备连接失败");
    _errorCodeInfolist.insert(MOTION_ERR_HANDLEERR,"卡号错误");
    _errorCodeInfolist.insert(ERR_SENDERR,"数据传输错误");
    _errorCodeInfolist.insert(ERR_POS_SUB_LIMIT,"位置偏差较大");
    _errorCodeInfolist.insert(ERR_MOTOR_LIMITED,"已在限位状态无法运动");
    _errorCodeInfolist.insert(ERR_MOTOR_STOP,"强制停止运动");
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QString QMotorDevCtrl::getMotorErrorCodeInfo(int nerrcode)
{
    return _errorCodeInfolist.value(nerrcode,"未知错误");
}
/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
QVariant QMotorDevCtrl::getMotorStatusInfo(int nMotorIndex)
{
    QVariant l_var;
    if(nMotorIndex >=0 && nMotorIndex <= 6)
    {
        l_var.setValue(m_tMotorStatus[nMotorIndex]);
    }

    return l_var;
}

/******************************************
* 功能:获取电机档位的配置信息
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
int QMotorDevCtrl::_getMotorGearConfig(int nMotorIndex,int nGearIndex)
{
    if(nMotorIndex == E_MOTOR_LIGHT_CHANGE && nGearIndex == 0)
    {
       return -222361;
    }
    if(nMotorIndex == E_MOTOR_LIGHT_CHANGE && nGearIndex == 1)
    {
       return 0;
    }

    if(nMotorIndex == E_MOTOR_PLATFORM && nGearIndex == 0)
    {
       return 0;
    }
    if(nMotorIndex == E_MOTOR_PLATFORM && nGearIndex == 1)
    {
       return -96*3200;
    }


    if(nMotorIndex == E_MOTOR_NEARFIELD_WHEEL)//轮盘
    {
        return -1*(534*nGearIndex+240+534*1.5);
    }

    return 0;

}

void QMotorDevCtrl::InitProfile(int nMotorIndex)
{
    switch (nMotorIndex) {
    case E_MOTOR_X:
        dmc_set_s_profile(_nCardNo,nMotorIndex,0,0);
        dmc_set_profile(_nCardNo,nMotorIndex,1000,10000,0.01,0.001,10);
        break;
    case E_MOTOR_Y:
        dmc_set_s_profile(_nCardNo,nMotorIndex,0,0);
        dmc_set_profile(_nCardNo,nMotorIndex,1000,50000,0.01,0.001,10);
        break;
    case E_MOTOR_Z:
        dmc_set_profile(_nCardNo,nMotorIndex,100,5000,0.2,0.2,10);
        break;
    case E_MOTOR_NEARFIELD_WHEEL:
        dmc_set_profile(_nCardNo,nMotorIndex,100,1000,0.2,0.2,10);
        break;
    case E_MOTOR_LIGHT_CHANGE:
    case E_MOTOR_PLATFORM:
        dmc_set_profile(_nCardNo,nMotorIndex,1000,20000,0.2,0.2,10);
    default:
        break;
    }

}
