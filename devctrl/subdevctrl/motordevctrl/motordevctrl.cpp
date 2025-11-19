
#include "motordevctrl.h"

#if _MSC_VER >=1600
    #pragma execution_character_set("utf-8")
#endif

QMotorDevCtrl::QMotorDevCtrl() :QMotorDevCtrlBasic()
{
    m_bConnect = false;
    _bStopMotorMove = false;
    memset(m_tMotorStatus, 0, sizeof(m_tMotorStatus));

    InitErrorInfo();
    _initCard();
}

QMotorDevCtrl::~QMotorDevCtrl()
{
    for (int i = 0; i < 4; i++)
    {
        stopMove(i);
    }
    QThread::msleep(100);
    m_card.MoCtrCard_Unload();
}

/******************************************
* 功能:运动至当前档位
******************************************/
bool QMotorDevCtrl::moveToGear(int nMotorIndex, int nGearIndex)
{
    bool bRel = false;

    m_tMotorStatus[nMotorIndex].sErroInfo = "";
    m_tMotorStatus[nMotorIndex].nMotorStatus = 0x01;
    _sendMotorStatus(nMotorIndex);

    short l_errcode = _initCard();
    if (l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }

    _bStopMotorMove = false;

    l_errcode = _moveToHome(nMotorIndex);
    if (l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }

    int nPlusCnt = _getMotorGearConfig(nMotorIndex, nGearIndex);

    l_errcode = _stepmotor_moveMotorToRelationPos(nMotorIndex, nPlusCnt);

    if (l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        bRel = false;
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
* 功能:运动到指定位置(相对运动)
******************************************/
bool QMotorDevCtrl::moveToRelationPos(int nMotorIndex, int nPlusCnt)
{
    bool bRel = false;
    m_tMotorStatus[nMotorIndex].sErroInfo = "";
    m_tMotorStatus[nMotorIndex].nMotorStatus = 0x01;
    _sendMotorStatus(nMotorIndex);

    short l_errcode = _initCard();
    if (l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }

    _bStopMotorMove = false;
    l_errcode = _stepmotor_moveMotorToRelationPos(nMotorIndex, nPlusCnt);

    if (l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        bRel = false;
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
* 功能:运动到指定位置(绝对运动)
******************************************/
bool QMotorDevCtrl::moveToAbsolutePos(int nMotorIndex, int nPlusCnt)
{
    bool bRel = false;
    m_tMotorStatus[nMotorIndex].sErroInfo = "";
    m_tMotorStatus[nMotorIndex].nMotorStatus = 0x01;
    _sendMotorStatus(nMotorIndex);

    short l_errcode = _initCard();
    if (l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }

    _bStopMotorMove = false;
    l_errcode = m_card.MoCtrCard_MCrlAxisAbsMove(nMotorIndex, (McCard_FP32)nPlusCnt, 50.0f, 100.0f);

    if (l_errcode != 0)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        bRel = false;
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
* 功能:回零运动
******************************************/
bool QMotorDevCtrl::moveToHome(int nMotorIndex)
{
    bool bRel = false;
    m_tMotorStatus[nMotorIndex].sErroInfo = "";
    m_tMotorStatus[nMotorIndex].nMotorStatus = 0x01;
    _sendMotorStatus(nMotorIndex);

    short l_errcode = _initCard();
    if (l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        _sendMotorStatus(nMotorIndex);
        return false;
    }

    _bStopMotorMove = false;
    l_errcode = _moveToHome(nMotorIndex);
    if (l_errcode != ERR_NOERR)
    {
        m_tMotorStatus[nMotorIndex].sErroInfo = getMotorErrorCodeInfo(l_errcode);
        m_tMotorStatus[nMotorIndex].nMotorStatus = 0x03;
        bRel = false;
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
******************************************/
bool QMotorDevCtrl::checkMotorRunning(int nMotorIndex)
{
    McCard_INT32 state[4] = { 0 };
    m_card.MoCtrCard_GetRunState(state);
    return (state[nMotorIndex] != 0);
}

/******************************************
* 功能:停止电机运动
******************************************/
bool QMotorDevCtrl::stopMove(int nMotorIndex)
{
    if (m_bConnect == false)
        return true;

    _CardLock.lock();
    _bStopMotorMove = true;
    m_card.MoCtrCard_StopAxisMov(nMotorIndex, 50.0f);
    _CardLock.unlock();
    return true;
}

/******************************************
* 功能:初始化控制卡
******************************************/
short QMotorDevCtrl::_initCard()
{
    if (m_bConnect == true)
        return ERR_NOERR;

    _CardLock.lock();
    McCard_UINT16 ret = m_card.MoCtrCard_Initial(1); // 串口COM1
    _CardLock.unlock();

    if (ret != 0)
    {
        m_bConnect = false;
        return ERR_CANNOT_CONNECTETH;
    }

    m_bConnect = true;
    return ERR_NOERR;
}

/******************************************
* 功能:发送电机当前状态
******************************************/
void QMotorDevCtrl::_sendMotorStatus(int nMotorIndex)
{
    QVariant l_var;
    l_var.setValue(m_tMotorStatus[nMotorIndex]);
    emit signal_updateMotorStatus(nMotorIndex, l_var);
}

/******************************************
* 功能:相对位置运动
******************************************/
short QMotorDevCtrl::_stepmotor_moveMotorToRelationPos(int nMotorIndex, long nPlusCnt)
{
    _CardLock.lock();
    McCard_UINT16 ret = m_card.MoCtrCard_MCrlAxisRelMove(nMotorIndex, (McCard_FP32)nPlusCnt, 50.0f, 100.0f);
    _CardLock.unlock();

    if (ret != 0)
        return ret;

    QTime t; t.start();
    McCard_INT32 state[4] = { 0 };
    while (t.elapsed() < M_MAX_WAIT_TIME && !_bStopMotorMove)
    {
        m_card.MoCtrCard_GetRunState(state);
        if (state[nMotorIndex] == 0)
            return ERR_NOERR;
        QThread::msleep(10);
        _sendMotorStatus(nMotorIndex);
    }
    if (_bStopMotorMove)
        return ERR_MOTOR_STOP;
    return ERR_TIMEOUT;
}

/******************************************
* 功能:回零运动
******************************************/
short QMotorDevCtrl::_moveToHome(int nMotorIndex)
{
    _CardLock.lock();
    McCard_UINT16 ret = m_card.MoCtrCard_SeekZero(nMotorIndex, 30.0f, 100.0f);
    _CardLock.unlock();

    if (ret != 0) return ret;

    QTime t; t.start();
    McCard_INT32 stt[4] = { 0 };
    while (t.elapsed() < M_MAX_WAIT_TIME && !_bStopMotorMove)
    {
        m_card.MoCtrCard_GetRunState(stt);
        if (stt[nMotorIndex] == 0)
        {
            m_card.MoCtrCard_ResetCoordinate(nMotorIndex, 0);
            return ERR_NOERR;
        }
        QThread::msleep(20);
        _sendMotorStatus(nMotorIndex);
    }

    if (_bStopMotorMove)
        return ERR_MOTOR_STOP;
    return ERR_TIMEOUT;
}

/******************************************
* 功能:错误信息初始化
******************************************/
void QMotorDevCtrl::InitErrorInfo()
{
    _errorCodeInfolist[ERR_NOERR] = "正常";
    _errorCodeInfolist[ERR_TIMEOUT] = "超时";
    _errorCodeInfolist[ERR_MOTOR_STOP] = "强制停止";
    _errorCodeInfolist[ERR_CANNOT_CONNECTETH] = "通信失败";
}

/******************************************
* 功能:获取错误信息
******************************************/
QString QMotorDevCtrl::getMotorErrorCodeInfo(int code)
{
    if (_errorCodeInfolist.contains(code))
        return _errorCodeInfolist[code];
    return "未知错误";
}


QVariant QMotorDevCtrl::getMotorStatusInfo(int nMotorIndex)
{
    QVariant l_var;
    if (nMotorIndex >= 0 && nMotorIndex <= 4)
    {
        l_var.setValue(m_tMotorStatus[nMotorIndex]);
    }
    else
    {
        //l_var.setValue("无此电机");
    }

    return l_var;
}
int QMotorDevCtrl::_getMotorGearConfig(int nMotorIndex, int nGearIndex)
{
    // 光源切换轴
    if (nMotorIndex == E_MOTOR_LIGHT_CHANGE)
    {
        // 档位0：最小光；档位1：最大光
        if (nGearIndex == 0) return -222361;
        if (nGearIndex == 1) return 0;
    }

    // 平台轴
    if (nMotorIndex == E_MOTOR_PLATFORM)
    {
        if (nGearIndex == 0) return 0;
        if (nGearIndex == 1) return -96 * 3200;  // 单位：步
    }

    // 近场轮盘轴（旋转式滤光轮等）
    if (nMotorIndex == E_MOTOR_NEARFIELD_WHEEL)
    {
        // 每档位之间相差534步，初始偏移240步
        // 乘1.5用于对齐机械零点
        return static_cast<int>(-1 * (534 * nGearIndex + 240 + 534 * 1.5));
    }

    // 其他未定义轴，返回0
    return 0;
}


