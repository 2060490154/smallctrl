#ifndef AUTOSAVEIMAGEWIDGET_H
#define AUTOSAVEIMAGEWIDGET_H
#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QTextEdit>
#include <QFileDialog>

class QAutoSaveImageWidget:public QDialog
{
    Q_OBJECT
public:
    QAutoSaveImageWidget();

signals:

public slots:
    void onSelectSavePath();//保存路径选择

    void onOk();

    void onCanel();

public:
    void InitUI();

    void InitData(QString sFilePath,bool bCheck);



public:
    QPushButton* m_pSelectFilePathButton;
    QCheckBox* m_pAutoSaveCheckBox;
    QTextEdit* m_pFilePathEdit;

    QPushButton* m_pOKButton;

    QPushButton* m_pCancelButton;



public:
    bool m_bAutoSave;
    QString m_sSavePath;
};

#endif // AUTOSAVEIMAGEWIDGET_H
