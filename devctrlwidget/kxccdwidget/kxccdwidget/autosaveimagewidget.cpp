#include "autosaveimagewidget.h"
#include <QGridLayout>

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

QAutoSaveImageWidget::QAutoSaveImageWidget()
{
    InitUI();
    connect(m_pOKButton,&QPushButton::clicked,this,&QAutoSaveImageWidget::onOk);
    connect(m_pCancelButton,&QPushButton::clicked,this,&QAutoSaveImageWidget::onCanel);
    connect(m_pSelectFilePathButton,&QPushButton::clicked,this,&QAutoSaveImageWidget::onSelectSavePath);
}



void QAutoSaveImageWidget::InitUI()
{
    this->resize(600,150);

    m_pAutoSaveCheckBox = new QCheckBox();
    m_pSelectFilePathButton = new QPushButton("...",this);
    m_pSelectFilePathButton->setMaximumWidth(50);
    m_pFilePathEdit = new QTextEdit();
    m_pFilePathEdit->setMaximumHeight(30);
    m_pFilePathEdit->setEnabled(false);
    m_pOKButton = new QPushButton("确定",this);
    m_pOKButton->setMaximumWidth(100);
    m_pCancelButton = new QPushButton("取消",this);
    m_pCancelButton->setMaximumWidth(100);

    QGridLayout* playout = new QGridLayout(this);

    playout->setColumnStretch(0,2);
    playout->setColumnStretch(1,2);
    playout->setColumnStretch(2,1);

    playout->addWidget(new QLabel("自动保存:"),0,0,1,1);
    playout->addWidget(m_pAutoSaveCheckBox,0,1,1,1);

    playout->addWidget(new QLabel("选择保存路径:"),1,0,1,1);
    playout->addWidget(m_pFilePathEdit,1,1,1,5);
    playout->addWidget(m_pSelectFilePathButton,1,6,1,1);

    playout->addWidget(m_pOKButton,2,2,1,1);
    playout->addWidget(m_pCancelButton,2,4,1,1);



}

void QAutoSaveImageWidget:: InitData(QString sFilePath,bool bCheck)
{
    m_pFilePathEdit->setText(sFilePath);
    m_pAutoSaveCheckBox->setChecked(bCheck);
    this->update();
}


void QAutoSaveImageWidget::onSelectSavePath()
{
     QString sPathName = QFileDialog::getExistingDirectory(this,"保存路径");

     m_pFilePathEdit->setText(sPathName);

}

void QAutoSaveImageWidget::onOk()
{
    m_sSavePath = m_pFilePathEdit->toPlainText();
    m_bAutoSave = m_pAutoSaveCheckBox->isChecked();

    this->close();

    this->setResult(QDialog::Accepted);
}

void QAutoSaveImageWidget::onCanel()
{
    this->close();
    this->setResult(QDialog::Rejected);
}
