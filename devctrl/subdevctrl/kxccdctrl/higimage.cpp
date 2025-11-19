/**************************************************
*文件说明:hig图像处理类
*提供的功能:
*initDataBuf    初始化buff数据
*clearDataBuff  清理buff资源
*setHiGrayTab   设置映射数据
*processHig2Bmp hig转为bmp
*saveHigImage 保存hig图像到本地
*getPixelValue 得到灰度数据
*维护记录:
*2019-01-25  create by lg
******************************************************/
#include "higimage.h"

CHigImage::CHigImage (QObject *parent) : QObject (parent)
{
    _oldDatalen = 0;
    m_pHigImage = nullptr;
    m_pHeaderInfo = nullptr;
    m_pHigData = nullptr;
    m_pHig2BmpData = nullptr;
    m_pBmpData = nullptr;
    
    for (int i = 0; i < 256; i++)
    {
        m_vcolorTable.append (qRgb (i, i, i));
    }
    
    memset (&m_tMeasureData, 0, sizeof (MeasureData));
    m_tMeasureData.pPixelData = nullptr;
    m_tMeasureData.pStatData = nullptr;
    setHiGrayTab (0, ( (1 << 12) - 1), 16); //默认为12bit
}

CHigImage::~CHigImage()
{
    if (m_pHig2BmpData != nullptr)
    {
        delete[] m_pHig2BmpData;
    }
}




/*************************************************************************************************************************************
@功能：初始化数据
@ 输入：nRow 图像数据行数（高） nCol为图像数据的宽，nBitLen为图像数据的位宽(8bit  12bit),pImageBuff为图像数据
***************************************************************************************************************************************/
void CHigImage::initDataBuf (int nRow, int nCol, int nBitLen, char* pImagBuff)
{
    unsigned int nDataLen = nRow * nCol;

    int nHeaderSize = sizeof (HIG_FILEHEADER);
    
    if (_oldDatalen != nDataLen)
    {
        _oldDatalen = nDataLen;//避免频繁分配内存
        clearDataBuff();

        m_pHigImage = new WORD[nHeaderSize/2 + nDataLen]; //长度为头信息+数据长度
        m_pHeaderInfo = (HIG_FILEHEADER*) m_pHigImage;
        memset (m_pHeaderInfo, 0x0, nHeaderSize);
		m_pHeaderInfo->nType = 'HIG';
        m_pHigData = (WORD*) ((char*)m_pHigImage + nHeaderSize);
    }
    
    if (nBitLen > 8)
    {
        m_pHeaderInfo->nDataLen = nDataLen * 2;
    }
    
    else
    {
        m_pHeaderInfo->nDataLen = nDataLen;
    }
    
    m_pHeaderInfo->nBits = nBitLen;//默认给定为12bit
    m_pHeaderInfo->nWidth = nCol;
    m_pHeaderInfo->nHeight = nRow;
    
    if (pImagBuff != nullptr)
    {
        memcpy ( (char*) m_pHigData, pImagBuff, m_pHeaderInfo->nDataLen);
    }
}

/**************************************************
@功能：清除数据指针
***************************************************/
void CHigImage::clearDataBuff()
{
    m_pHeaderInfo = nullptr;
    m_pHigData = nullptr;
    
    if (m_pHigImage != nullptr)
    {
        delete[] m_pHigImage;
        m_pHigImage = nullptr;
    }
}

/********************************************************
 *
 * 加载HIG图像
 * **********************************************************/
void CHigImage::loadFile (QString sFilePath)
{
    HIG_FILEHEADER stImageHeader;
    unsigned int nFileLen = 0;//文件大小
    QFile file (sFilePath);
    file.open (QIODevice::ReadOnly);
    nFileLen = file.size();
    QByteArray dataArray = file.readAll();
    char* pdata = dataArray.data();
    
    if (nFileLen > sizeof (HIG_FILEHEADER))
    {
        memcpy ( (char*) &stImageHeader, pdata, sizeof (HIG_FILEHEADER));
        initDataBuf (stImageHeader.nHeight, stImageHeader.nWidth, stImageHeader.nBits, nullptr);
        memcpy ( (char*) m_pHigData, (char*) (pdata + nFileLen - m_pHeaderInfo->nDataLen), m_pHeaderInfo->nDataLen);
    }
}


/*************************************************************************
@功能：生成当前窗宽和窗位下 灰度映射到0-255区间的对应关系数据
***************************************************************************/
void CHigImage::setHiGrayTab (int sg, int wg, int nBit)
{
    double lfMax = 256.0;
    int nMaxGray = 1 << nBit;
    double Gr = 0.0;
    double kf = 0.0;
    double kg = 0.0;
    
    if (m_pHig2BmpData != nullptr)
    {
        delete[] m_pHig2BmpData;
    }
    
    m_pHig2BmpData = new uchar[nMaxGray + 1];
    
    if (wg < 1) { wg = 1; }
    
    if (wg > nMaxGray) { wg = nMaxGray; }
    
    if (sg < 0) { sg = 0; }
    
    if (sg > nMaxGray - wg) { sg = nMaxGray - wg; }
    
    kf = lfMax / wg;
    kg = kf * sg;
    
    for (int i = 0; i <= nMaxGray; i++)
    {
        if (i < sg)
        {
            m_pHig2BmpData[i] = 0;
        }
        
        else
            if (i > sg + wg - 1)
            {
                m_pHig2BmpData[i] = 255;
            }
            
            else
            {
                Gr = i * kf - kg;
                
                if (Gr < 0) { Gr = 0; }
                
                if (Gr > 255.0f) { Gr = 255.0f; }
                
                m_pHig2BmpData[i] = (uchar) Gr;
            }
    }
    
    m_pHig2BmpData[nMaxGray - 1] = 255;
}


/****************************************************
 *将HIG数据转换为BMP图像
 *
 * *****************************************************/
void CHigImage::processHig2Bmp()
{
    WORD nMaxGray = (1 << m_pHeaderInfo->nBits) - 1;
    
    if (m_pBmpData != nullptr)
    {
        delete[] m_pBmpData;
    }
    
    m_pBmpData = new uchar[m_pHeaderInfo->nWidth * m_pHeaderInfo->nHeight];
    
    for (int i = 0; i < m_pHeaderInfo->nWidth * m_pHeaderInfo->nHeight; i++)
    {
        if (m_pHigData[i] <= nMaxGray)
        {
            m_pBmpData[i] = m_pHig2BmpData[m_pHigData[i]];
        }
        else
        {
            m_pBmpData[i] = m_pHig2BmpData[nMaxGray];
        }
    }
}


/****************************************************
 *将HIG内存数据保存为hig文件
 *nBitLen---可选择保存数据的bit位数
 * *****************************************************/
bool CHigImage::saveHigImage (QString sFileName, int nBitLen)
{
    if(m_pHigImage == nullptr)
    {
        return false;
    }

    QFile file (sFileName);
    file.open (QIODevice::WriteOnly);

    QDateTime currentTime = QDateTime::currentDateTime();
    QString sTime = currentTime.toString ("yyyy-MM-dd-hh-mm-ss-zzz");
    memcpy (m_pHeaderInfo->pDate, sTime.data(), 32);
    sTime = currentTime.toString ("hh-mm-ss-zzz");
    memcpy (m_pHeaderInfo->pTime, sTime.data(), 32);

    int ndataLen  = sizeof (HIG_FILEHEADER) + m_pHeaderInfo->nDataLen;

    //数据信息
    file.write ( (char*) m_pHigImage, ndataLen);
    file.close();
    return true;
}

/****************************************************
 *保存bmp图像
 * *****************************************************/
bool CHigImage::saveBmpImage (QString sFileName)
{
    if(m_pBmpData == nullptr)
    {
        return false;
    }

    QImage l_bmpimage = QImage (m_pBmpData, m_pHeaderInfo->nWidth, m_pHeaderInfo->nHeight, QImage::Format_Indexed8); //封装QImage
    l_bmpimage.setColorTable (m_vcolorTable); //设置颜色表
    l_bmpimage = l_bmpimage.mirrored (false, true);
    return l_bmpimage.save (sFileName);
}





/****************************************************
 *获得当前坐标下的灰度值 图片旋转了180度
 * *****************************************************/
WORD CHigImage::getPixelValue (int nPosx, int nPosy)
{
    if (m_pHeaderInfo == nullptr)
    {
        return 0;
    }
    
    if (nPosx >= m_pHeaderInfo->nWidth || nPosx < 0 || nPosy >= m_pHeaderInfo->nHeight || nPosy < 0)
    {
        return 0;//超限
    }
    
    return m_pHigData[ (m_pHeaderInfo->nHeight - nPosy - 1) * m_pHeaderInfo->nWidth + nPosx];
}

/****************************************************
 *获得范围内的灰度数据 返回值为当前数据长度 数据内容
 * *****************************************************/
int CHigImage::getAreaData (QPoint tStartPoint, QPoint tEndPoint)
{
    int minx = qMin (tStartPoint.x(), tEndPoint.x());
    int maxx = qMax (tStartPoint.x(), tEndPoint.x());
    int miny = qMin (tStartPoint.y(), tEndPoint.y());
    int maxy = qMax (tStartPoint.y(), tEndPoint.y());
    m_tMeasureData.nHeight = maxy - miny + 1;
    m_tMeasureData.nWidth = maxx - minx + 1;
    int nDataLen = m_tMeasureData.nHeight * m_tMeasureData.nWidth;
    
    //清除之前的数据
    if (m_tMeasureData.pPixelData != nullptr)
    {
        delete[] m_tMeasureData.pPixelData;
        m_tMeasureData.pPixelData = nullptr;
    }
    
    m_tMeasureData.pPixelData =  new WORD[m_tMeasureData.nHeight * m_tMeasureData.nWidth];
    memset (m_tMeasureData.pPixelData, 0, nDataLen * sizeof (WORD));
    
    for (int nRow = miny; nRow < maxy + 1; ++nRow)
    {
        for (int nCol = minx; nCol < maxx + 1; ++nCol)
        {
            m_tMeasureData.pPixelData[ (nRow - miny) *m_tMeasureData.nWidth + (nCol - minx)] = getPixelValue (nCol, nRow);
        }
    }
    
    return nDataLen;
}
/****************************************************
 *计算测量参数
 * *****************************************************/
void CHigImage::calMeasureValue (int nDataLen)
{
    double sqmean = 0.0 ;
    double sqsum = 0.0;
    WORD maxvalue = 0;
    WORD minvalue = m_tMeasureData.pPixelData[0];
    __int64  sum = 0;
    int nMaxPixel = 1 << m_pHeaderInfo->nBits;
    m_tMeasureData.nPixelDataLen = nDataLen;
    m_tMeasureData.nStatDataLen = nMaxPixel;
    
    if (m_tMeasureData.pStatData != nullptr)
    {
        delete[] m_tMeasureData.pStatData;
        m_tMeasureData.pStatData = nullptr;
    }
    
    m_tMeasureData.pStatData = new WORD[nMaxPixel];
    memset (m_tMeasureData.pStatData, 0, nMaxPixel * sizeof (WORD));
    
    for (int i = 0; i < nDataLen; ++i)
    {
        maxvalue = qMax (maxvalue, m_tMeasureData.pPixelData[i]);
        minvalue = qMin (minvalue, m_tMeasureData.pPixelData[i]);
        sum += m_tMeasureData.pPixelData[i];
        m_tMeasureData.pStatData[m_tMeasureData.pPixelData[i]]++;
    }
    
    m_tMeasureData.nMaxvalue = maxvalue;
    m_tMeasureData.nMinvalue = minvalue;
    
    if (nDataLen > 0)
    {
        m_tMeasureData.lfMeanvalue = (double) sum / (double) nDataLen;
    }
    
    else
    {
        m_tMeasureData.lfMeanvalue = 0.0;
    }
    
    for (int j = 0; j < nDataLen; j++)
    {
        sqsum += (m_tMeasureData.pPixelData[j] - m_tMeasureData.lfMeanvalue) * (m_tMeasureData.pPixelData[j] - m_tMeasureData.lfMeanvalue);
    }
    
    if (nDataLen > 0)
    {
        sqmean = sqsum / nDataLen;
        m_tMeasureData.lfRms = sqrt (sqmean);
    }
    
    else
    {
        m_tMeasureData.lfRms = 0.0;
    }
    
    if (m_tMeasureData.lfMeanvalue > 1e-12)
    {
        m_tMeasureData.lfm = (double) m_tMeasureData.nMaxvalue / m_tMeasureData.lfMeanvalue;
        m_tMeasureData.lfc = (double) m_tMeasureData.lfRms / m_tMeasureData.lfMeanvalue;
    }
    
    else
    {
        m_tMeasureData.lfm = 0.0;
        m_tMeasureData.lfc = 0.0;
    }
}
