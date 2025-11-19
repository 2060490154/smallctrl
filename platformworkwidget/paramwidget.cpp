/**************************************************
*文件说明:参数设置界面
*创建人:
*维护记录:
*
******************************************************/
#include "paramwidget.h"
#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif


ParamWidget::ParamWidget(QWidget *parent) : QWidget(parent)
{

    InitParam();

    InitUI();
    showParam();

    connect(m_pRefreshParamBtn,&QPushButton::clicked,this,&ParamWidget::onUpdateSetParam);
    void(QComboBox::*fp)(int)=&QComboBox::currentIndexChanged;
    connect(m_pMeasureTypeCombox,fp,this,&ParamWidget::onChangeMeasureType);

}

void ParamWidget::InitUI()
{


    m_pTaskNoEdit = new QTextEdit("");
    m_pExpTempeEdit = new QTextEdit("0");
    m_pExphumityEdit = new QTextEdit("0");
    m_pExpdirtyEdit = new QTextEdit("0");

    m_pLaserInputEnergyEdit = new QTextEdit("0");


    m_pCurrentWaveLengthLabel = new QLabel();
    m_pCurrentWaveLengthLabel->setStyleSheet("color:blue;font-size:16px;");

    m_pWidthLabel = new QLabel("长(mm)",this);
    m_pWidthLabel->hide();

    m_pHightLabel = new QLabel("列数");
    m_pEnergyLabel = new QLabel();

    m_pMaximumEnergyEdit = new QTextEdit("0");

    m_pEnergyCoeffEdit = new QTextEdit();

    m_pRefreshParamBtn = new QPushButton("保存参数",this);

    m_pMeasureStepEdit = new QTextEdit(this);
    m_pSampleName = new QTextEdit(this);
    m_pRectColCntEdit = new QTextEdit(this);
    m_pRectRowCntEdit = new QTextEdit(this);
    m_pRadiusEdit = new QTextEdit(this);
    m_pEnergyIncreamEdit = new QTextEdit(this);
    m_pMinimumEnergyEdit = new QTextEdit(this);

    m_pEnergySectionNumEdit = new QTextEdit();
    m_p1On1CntEdit = new QTextEdit();

    m_pRatioEdit = new QTextEdit();

    m_pPlusWidthEdit = new QTextEdit();


    m_pMeasureTypeCombox = new QComboBox();//测试类型选择
    m_pMeasureTypeCombox->addItem("1 On 1");
    m_pMeasureTypeCombox->addItem("R On 1");

    m_pMeasureTypeCombox->setMinimumHeight(25);

    m_pManualAreaEdit = new QTextEdit();


    m_pDevSizeTypeCombox = new QComboBox(this);//元器件尺寸类型
    m_pDevSizeTypeCombox->addItem("矩形");
    m_pDevSizeTypeCombox->addItem("圆形");
    m_pDevSizeTypeCombox->hide();

    m_pCurrentWaveLengthLabel->setMaximumHeight(30);

    m_pRadiusEdit->setHidden(true);

    m_pwidgetlist.push_back(m_pSampleName);
    m_pwidgetlist.push_back(m_pMeasureStepEdit);
    m_pwidgetlist.push_back(m_pRectColCntEdit);
    m_pwidgetlist.push_back(m_pRectRowCntEdit);
    m_pwidgetlist.push_back(m_pRadiusEdit);

    m_pwidgetlist.push_back(m_pMinimumEnergyEdit);
    m_pwidgetlist.push_back(m_pEnergyIncreamEdit);
    m_pwidgetlist.push_back(m_p1On1CntEdit);
    m_pwidgetlist.push_back(m_pManualAreaEdit);
    m_pwidgetlist.push_back(m_pEnergySectionNumEdit);
    m_pwidgetlist.push_back(m_pEnergyCoeffEdit);

    m_pwidgetlist.push_back(m_pDevSizeTypeCombox);
    m_pwidgetlist.push_back(m_pMeasureTypeCombox);
    m_pwidgetlist.push_back(m_pMaximumEnergyEdit);
    m_pwidgetlist.push_back(m_pRatioEdit);
    m_pwidgetlist.push_back(m_pPlusWidthEdit);

    m_pwidgetlist.push_back(m_pTaskNoEdit);
    m_pwidgetlist.push_back(m_pExpTempeEdit);
    m_pwidgetlist.push_back(m_pExphumityEdit);
    m_pwidgetlist.push_back(m_pExpdirtyEdit);
    m_pwidgetlist.push_back(m_pLaserInputEnergyEdit);

    for(int i = 0; i < m_pwidgetlist.size(); i++)
    {
        m_pwidgetlist[i]->setMaximumHeight(30);
    }


    int nRow = 0;
    QGridLayout* playout =new QGridLayout(this);
    for(int i = 0; i < 7; i++)
    {
        playout->setColumnStretch(i,1);
    }



    playout->addWidget(new QLabel("当前光路"),nRow,0,1,1);
    playout->addWidget(m_pCurrentWaveLengthLabel,nRow,1,1,6);


    nRow++;

    playout->addWidget(new QLabel("任务编号"),nRow,0,1,1);
    playout->addWidget(m_pTaskNoEdit,nRow,1,1,3);

    playout->addWidget(new QLabel("元件编号"),nRow,4,1,1);
    playout->addWidget(m_pSampleName,nRow,5,1,2);

    nRow++;

    playout->addWidget(new QLabel("温度(℃)"),nRow,0,1,1);
    playout->addWidget(m_pExpTempeEdit,nRow,1,1,1);

    playout->addWidget(new QLabel("湿度(％)"),nRow,2,1,1);
    playout->addWidget(m_pExphumityEdit,nRow,3,1,1);

    playout->addWidget(new QLabel("洁净度"),nRow,4,1,1);
    playout->addWidget(m_pExpdirtyEdit,nRow,5,1,2);

    nRow++;



    playout->addWidget(new QLabel("测试类型选择"),nRow,0,1,1);
    playout->addWidget(m_pMeasureTypeCombox,nRow,1,1,3);

    playout->addWidget(new QLabel("激光器输出能量(mJ)"),nRow,4,1,1);
    playout->addWidget(m_pLaserInputEnergyEdit,nRow,5,1,2);


    nRow++;

    playout->addWidget(new QLabel("扫描参数:行数"),nRow,0,1,1);

    playout->addWidget(m_pRectRowCntEdit,nRow,1,1,1);


    playout->addWidget(m_pHightLabel,nRow,2,1,1);
    playout->addWidget(m_pRectColCntEdit,nRow,3,1,1);

    playout->addWidget(new QLabel("扫描步长(mm)"),nRow,4,1,1);
    playout->addWidget(m_pMeasureStepEdit,nRow,5,1,2);

    nRow++;

    playout->addWidget(new QLabel("最小能量(%)"),nRow,0,1,1);
    playout->addWidget(m_pMinimumEnergyEdit,nRow,1,1,3);

    playout->addWidget(new QLabel("最大能量(%)"),nRow,4,1,1);
    playout->addWidget(m_pMaximumEnergyEdit,nRow,5,1,2);


    nRow++;

    playout->addWidget(new QLabel("能量递增(%)"),nRow,0,1,1);
    playout->addWidget(m_pEnergyIncreamEdit,nRow,1,1,3);


    m_pEnergyLabel->setText("每个能量发次数");
    playout->addWidget(m_pEnergyLabel,nRow,4,1,1);
    playout->addWidget(m_p1On1CntEdit,nRow,5,1,2);

    playout->addWidget(m_pEnergySectionNumEdit,nRow,5,1,2);

    if(m_tworkflowParam.m_nMeasureType == M_MEASURETYPE_1On1)
    {
        m_pEnergySectionNumEdit->hide();
    }

    if(m_tworkflowParam.m_nMeasureType == M_MEASURETYPE_ROn1)
    {
        m_p1On1CntEdit->hide();
    }


    nRow++;

    playout->addWidget(new QLabel("光斑面积(cm²)"),nRow,0,1,1);
    playout->addWidget(m_pManualAreaEdit,nRow,1,1,3);


    playout->addWidget(new QLabel("分光比"),nRow,4,1,1);
    playout->addWidget(m_pEnergyCoeffEdit,nRow,5,1,2);

    nRow++;

    playout->addWidget(new QLabel("脉宽(ns)"),nRow,0,1,1);
    playout->addWidget(m_pPlusWidthEdit,nRow,1,1,3);


    playout->addWidget(new QLabel("测试角度(°)"),nRow,4,1,1);
    playout->addWidget(m_pRatioEdit,nRow,5,1,1);

    playout->addWidget(m_pRefreshParamBtn,nRow,6,1,1);

    setWidgetStyleSheet(this);

}
/******************************************
* 功能:切换显示内容
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::onChangeWaveLength(int nIndex)
{
    m_tworkflowParam.m_nWaveLengthType = nIndex;
    m_pEnergyCoeffEdit->setText(QString("%1").arg(m_tworkflowParam.m_dEnergyCoeff[m_tworkflowParam.m_nWaveLengthType]));
    m_pManualAreaEdit->setText(QString("%1").arg(m_tworkflowParam.m_dManualArea[m_tworkflowParam.m_nWaveLengthType]));
}

/******************************************
* 功能:切换显示内容
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::onChangeMeasureType(int nIndex)
{
    if(nIndex+1 == M_MEASURETYPE_1On1)
    {
        m_pEnergyLabel->setText("每个能量发次数");
        m_p1On1CntEdit->show();
        m_pEnergySectionNumEdit->hide();
        m_pMaximumEnergyEdit->setEnabled(true);
    }

    if(nIndex+1 == M_MEASURETYPE_ROn1)
    {
        m_pEnergyLabel->setText("  能量台阶数  ");
        m_p1On1CntEdit->hide();
        m_pEnergySectionNumEdit->show();
        m_pMaximumEnergyEdit->setText("100");
        m_pMaximumEnergyEdit->setEnabled(false);
    }

}


/******************************************
* 功能:切换显示内容
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::onChangeSizeType(int nIndex)
{
    if(nIndex == 0)//矩形
    {
       m_pWidthLabel->setText("长(mm)");
        m_pRectRowCntEdit->show();
        m_pHightLabel->show();

    }

    if(nIndex == 1)//圆形
    {
        m_pWidthLabel->setText("半径(mm)");
        m_pRectRowCntEdit->hide();
        m_pHightLabel->hide();
    }
}

/******************************************
* 功能:保存参数 更新显示
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::onUpdateSetParam()
{
    if(m_pRefreshParamBtn->text()=="保存参数")
    {

        m_tworkflowParam.m_sExpTaskNo = m_pTaskNoEdit->toPlainText();
        m_tworkflowParam.m_lfExpTemp = m_pExpTempeEdit->toPlainText().toFloat();
        m_tworkflowParam.m_lfExphum = m_pExphumityEdit->toPlainText().toFloat();
        m_tworkflowParam.m_lfExpdirty = m_pExpdirtyEdit->toPlainText().toFloat();


        m_tworkflowParam.m_sSampleName = m_pSampleName->toPlainText();

        m_tworkflowParam.m_dStep = m_pMeasureStepEdit->toPlainText().toDouble();

        m_tworkflowParam.m_lfEnergyIncream = m_pEnergyIncreamEdit->toPlainText().toFloat();
        m_tworkflowParam.m_lfMinimumEnergy = m_pMinimumEnergyEdit->toPlainText().toFloat();
        m_tworkflowParam.m_lfMaximumEnergy = m_pMaximumEnergyEdit->toPlainText().toFloat();

        m_tworkflowParam.m_nEnergySectionNum = m_pEnergySectionNumEdit->toPlainText().toInt();
        m_tworkflowParam.m_n1On1PointNum = m_p1On1CntEdit->toPlainText().toInt();
        m_tworkflowParam.m_dManualArea[m_tworkflowParam.m_nWaveLengthType] = m_pManualAreaEdit->toPlainText().toDouble();

        m_tworkflowParam.m_dEnergyCoeff[m_tworkflowParam.m_nWaveLengthType] = m_pEnergyCoeffEdit->toPlainText().toDouble();

        m_tworkflowParam.m_dPlusWidth[m_tworkflowParam.m_nWaveLengthType] = m_pPlusWidthEdit->toPlainText().toDouble();
        m_tworkflowParam.m_lfAngle = m_pRatioEdit->toPlainText().toDouble();


        m_tworkflowParam.m_nMeasureType = m_pMeasureTypeCombox->currentIndex()+1;


        m_tworkflowParam.m_dHigh = m_pRectRowCntEdit->toPlainText().toInt()*m_tworkflowParam.m_dStep;
        m_tworkflowParam.m_dWidth = m_pRectColCntEdit->toPlainText().toInt()*m_tworkflowParam.m_dStep;


        m_tworkflowParam.m_lfLaserEnergy = m_pLaserInputEnergyEdit->toPlainText().toFloat();

        if(m_pRectRowCntEdit->toPlainText().toInt() <= 0 || m_pRectColCntEdit->toPlainText().toInt() <= 0)
        {
            QMessageBox::information(this,"提示","扫描行列均不能为0");
            return;
        }

        if(m_tworkflowParam.m_nEnergySectionNum< 0)
        {
            QMessageBox::information(this,"提示","能量台阶数不正确，不能为0或字符");
            return;
        }


        if(m_tworkflowParam.m_sExpTaskNo.isEmpty())
        {
            QMessageBox::information(this,"提示","任务单编号不能为空！");
            return;
        }

        if(m_tworkflowParam.m_dStep <= 0)
        {
            QMessageBox::information(this,"提示","步长设置不正确，不能为0或字符");
            return;
        }

        if(m_tworkflowParam.m_lfMinimumEnergy < 0 || m_tworkflowParam.m_lfMinimumEnergy > 100.0)
        {
            QMessageBox::information(this,"提示","最小能量设置错误，参数范围为[0,100]");
            return;
        }

        if(m_tworkflowParam.m_lfMaximumEnergy < 0 || m_tworkflowParam.m_lfMaximumEnergy > 100.0)
        {
            QMessageBox::information(this,"提示","最大能量设置错误，参数范围为[0,100]");
            return;
        }

        float lfdata = m_tworkflowParam.m_lfMinimumEnergy + (m_tworkflowParam.m_nEnergySectionNum-1)*m_tworkflowParam.m_lfEnergyIncream;
        if(m_tworkflowParam.m_nMeasureType == 0x02 && (lfdata > 100))
        {
            QMessageBox::information(this,"提示","能量设置错误，递增后能量大于100%");
            return;
        }

        if(m_tworkflowParam.m_lfLaserEnergy <= 0.0)
        {
            QMessageBox::information(this,"提示","请确认激光器当前输出能量值(mJ)");
            return;
        }


        for(int i = 0; i < m_pwidgetlist.size(); i++)
        {
            m_pwidgetlist[i]->setEnabled(false);
        }

        m_pRefreshParamBtn->setText("修改参数");

        emit updatePointUI(0x01,m_tworkflowParam.m_dStep,m_tworkflowParam.m_dWidth,m_tworkflowParam.m_dHigh);

        QVariant l_var;

        l_var.setValue(m_tworkflowParam);
        emit updateWorkFlow(l_var);
    }
    else
    {
        for(int i = 0; i < m_pwidgetlist.size(); i++)
        {
            m_pwidgetlist[i]->setEnabled(true);
        }

        m_pRefreshParamBtn->setText("保存参数");
    }




//    emit updateWorkFlow();
//    emit updateExpParas(m_tworkflowParam.m_sExpNumber,m_tworkflowParam.m_sSampleName);
//    emit updateDetectMethod();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::InitParam()
{
    m_tworkflowParam.m_sSampleName = "##";

    m_tworkflowParam.m_dStep = 10.0;
    m_tworkflowParam.m_nWaveLengthType = 0;
    m_tworkflowParam.m_lfEnergyIncream = 0.0;
    m_tworkflowParam.m_lfMinimumEnergy = 0.0;
    m_tworkflowParam.m_lfMaximumEnergy = 100.0;

    m_tworkflowParam.m_nEnergySectionNum = 10;
    m_tworkflowParam.m_n1On1PointNum = 10;
    m_tworkflowParam.m_dManualArea[0] = 0.0;
    m_tworkflowParam.m_dManualArea[1] = 0.0;
    m_tworkflowParam.m_dEnergyCoeff[0] = 1.0;
    m_tworkflowParam.m_dEnergyCoeff[1] = 1.0;
    m_tworkflowParam.m_dPlusWidth[0] = 1.0;
    m_tworkflowParam.m_dPlusWidth[1] = 1.0;

    m_tworkflowParam.m_lfAngle = 0.0;

    m_tworkflowParam.m_dWidth = 100.0;
    m_tworkflowParam.m_dHigh = 100.0;

    m_tworkflowParam.m_nMeasureType = M_MEASURETYPE_1On1;
    m_tworkflowParam.m_nWaveLengthType = 0x0;
}


/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::showParam()
{
    m_pSampleName->setText(m_tworkflowParam.m_sSampleName);
    m_pMeasureStepEdit->setText(QString("%1").arg(m_tworkflowParam.m_dStep));
    m_pEnergyIncreamEdit->setText(QString("%1").arg(m_tworkflowParam.m_lfEnergyIncream));

    m_pMinimumEnergyEdit->setText(QString("%1").arg(m_tworkflowParam.m_lfMinimumEnergy));
    m_pMaximumEnergyEdit->setText(QString("%1").arg(m_tworkflowParam.m_lfMaximumEnergy));
    m_pEnergySectionNumEdit->setText(QString("%1").arg(m_tworkflowParam.m_nEnergySectionNum));
    m_p1On1CntEdit->setText(QString("%1").arg(m_tworkflowParam.m_n1On1PointNum));
    m_pManualAreaEdit->setText(QString("%1").arg(m_tworkflowParam.m_dManualArea[m_tworkflowParam.m_nWaveLengthType]));
    m_pRectColCntEdit->setText(QString("%1").arg(m_tworkflowParam.m_dWidth/m_tworkflowParam.m_dStep));
    m_pRectRowCntEdit->setText(QString("%1").arg(m_tworkflowParam.m_dHigh/m_tworkflowParam.m_dStep));
    m_pEnergyCoeffEdit->setText(QString("%1").arg(m_tworkflowParam.m_dEnergyCoeff[m_tworkflowParam.m_nWaveLengthType]));

    m_pPlusWidthEdit->setText(QString("%1").arg(m_tworkflowParam.m_dPlusWidth[m_tworkflowParam.m_nWaveLengthType]));
    m_pRatioEdit->setText(QString("%1").arg(m_tworkflowParam.m_lfAngle));

    m_pMeasureTypeCombox->setCurrentIndex(m_tworkflowParam.m_nMeasureType-1);

    m_pCurrentWaveLengthLabel->setText(m_tworkflowParam.m_sLaserName[m_tworkflowParam.m_nWaveLengthType]);

}

/******************************************
* 功能:检查是否保存参数
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
bool ParamWidget::checkParamSaved()
{
    if(m_pRefreshParamBtn->text()=="保存参数")
    {
        return false;
    }

    return true;
}
/******************************************
* 功能:更新光斑面积显示和相关参数
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::setAreaData(float lfArea)
{
    m_tworkflowParam.m_dManualArea[m_tworkflowParam.m_nWaveLengthType] = lfArea;
    showParam();
}

/******************************************
* 功能:更新分光比参数
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::setEnergyCoeffData(float lfCoeff)
{
    m_tworkflowParam.m_dEnergyCoeff[m_tworkflowParam.m_nWaveLengthType] = lfCoeff;
    showParam();
}

/******************************************
* 功能:
* 输入:
* 输出:
* 返回值:
* 维护记录:
******************************************/
void ParamWidget::setWidgetStyleSheet(QWidget* pWidget)
{
    QPalette pal(pWidget->palette());
    pal.setColor(QPalette::Background, QColor(255,255,255));
    pWidget->setAutoFillBackground(true);
    pWidget->setPalette(pal);

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);// 阴影偏移
    shadowEffect->setColor(QColor(180,180,180));// 阴影颜色;
    shadowEffect->setBlurRadius(10);// 阴影半径;
    pWidget->setGraphicsEffect(shadowEffect);// 给窗口设置上当前的阴影效果;

}


