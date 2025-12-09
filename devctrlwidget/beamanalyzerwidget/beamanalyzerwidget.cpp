#include "beamanalyzerwidget.h"
#include <QtWidgets>
#include <QPrintDialog>
#include <QPrinter>

#pragma execution_character_set("utf-8")
BeamAnalyzerWidget::BeamAnalyzerWidget(QWidget* parent)
    : QWidget(parent),
    m_currentFrameIndex(0),
    m_connected(false)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    setupUi();
    setupConnections();
}

BeamAnalyzerWidget::~BeamAnalyzerWidget()
{
    m_timer->stop();
}

void BeamAnalyzerWidget::setupUi()
{
    QVBoxLayout* mainV = new QVBoxLayout(this);

    // Top bar
    QHBoxLayout* topBar = new QHBoxLayout;
    m_connectionIndicator = new QLabel;
    m_connectionIndicator->setFixedSize(14, 14);
    m_serialLabel = new QLabel(tr("S/N: -"));
    m_btnDisconnect = new QPushButton(tr("断开设备"));
    m_btnStart = new QPushButton(tr("开始采集"));
    m_btnStop = new QPushButton(tr("停止采集"));
    topBar->addWidget(m_connectionIndicator);
    topBar->addWidget(m_serialLabel);
    topBar->addStretch();
    topBar->addWidget(m_btnDisconnect);
    topBar->addWidget(m_btnStart);
    topBar->addWidget(m_btnStop);

    m_modeCombo = new QComboBox; m_modeCombo->addItems({ tr("标准"), tr("M²") });
    m_displayCombo = new QComboBox; m_displayCombo->addItems({ tr("3D/2D"), tr("十字线"), tr("光束跟踪") });
    topBar->addWidget(new QLabel(tr("测量模式:"))); topBar->addWidget(m_modeCombo);
    topBar->addWidget(new QLabel(tr("显示模式:"))); topBar->addWidget(m_displayCombo);

    m_btnSaveCurrent = new QPushButton(tr("保存当前图像"));
    m_btnSaveAll = new QPushButton(tr("保存所有缓冲区图像"));
    m_btnExport = new QPushButton(tr("导出测量数据"));
    m_btnPrint = new QPushButton(tr("打印报告"));
    m_btnOpenBG = new QPushButton(tr("打开背景图"));
    m_btnOpenCfg = new QPushButton(tr("打开配置文件"));
    topBar->addWidget(m_btnSaveCurrent);
    topBar->addWidget(m_btnSaveAll);
    topBar->addWidget(m_btnExport);
    topBar->addWidget(m_btnPrint);
    topBar->addWidget(m_btnOpenBG);
    topBar->addWidget(m_btnOpenCfg);

    mainV->addLayout(topBar);

    // Middle split: left display + right params
    QHBoxLayout* midH = new QHBoxLayout;

    // Left display area
    QWidget* leftPanel = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    m_displayStack = new QStackedWidget;
    QLabel* dispPlaceholder = new QLabel(tr("显示窗口（图像渲染区）"));
    dispPlaceholder->setAlignment(Qt::AlignCenter);
    dispPlaceholder->setStyleSheet("background:#111; color:#fff;");
    dispPlaceholder->setMinimumSize(480, 360);
    m_displayStack->addWidget(dispPlaceholder);

    QHBoxLayout* viewCtrl = new QHBoxLayout;
    m_btnZoomIn = new QPushButton(tr("放大"));
    m_btnZoomOut = new QPushButton(tr("缩小"));
    m_btnRotate = new QPushButton(tr("旋转"));
    m_btnPan = new QPushButton(tr("平移"));
    m_btnScreenshot = new QPushButton(tr("截图"));
    m_btnResetView = new QPushButton(tr("重置视图"));
    viewCtrl->addWidget(m_btnZoomIn);
    viewCtrl->addWidget(m_btnZoomOut);
    viewCtrl->addWidget(m_btnRotate);
    viewCtrl->addWidget(m_btnPan);
    viewCtrl->addWidget(m_btnScreenshot);
    viewCtrl->addWidget(m_btnResetView);

    QHBoxLayout* bufCtrl = new QHBoxLayout;
    m_frameIndex = new QSpinBox;
    m_frameIndex->setMinimum(0);
    m_btnPrevFrame = new QPushButton(tr("上一帧"));
    m_btnNextFrame = new QPushButton(tr("下一帧"));
    m_btnPlayAnim = new QPushButton(tr("播放"));
    m_bufferSize = new QSpinBox; m_bufferSize->setRange(1, 128); m_bufferSize->setValue(8);
    bufCtrl->addWidget(new QLabel(tr("帧索引:"))); bufCtrl->addWidget(m_frameIndex);
    bufCtrl->addWidget(m_btnPrevFrame); bufCtrl->addWidget(m_btnNextFrame);
    bufCtrl->addWidget(m_btnPlayAnim); bufCtrl->addWidget(new QLabel(tr("缓冲大小:")));
    bufCtrl->addWidget(m_bufferSize);

    QHBoxLayout* displaySet = new QHBoxLayout;
    m_chkColorBar = new QCheckBox(tr("颜色图例"));
    m_chkGray = new QCheckBox(tr("灰度"));
    m_chkDiameterEllipse = new QCheckBox(tr("直径椭圆"));
    m_chkCrosshair = new QCheckBox(tr("十字线"));
    displaySet->addWidget(m_chkColorBar); displaySet->addWidget(m_chkGray);
    displaySet->addWidget(m_chkDiameterEllipse); displaySet->addWidget(m_chkCrosshair);

    leftLayout->addWidget(m_displayStack);
    leftLayout->addLayout(viewCtrl);
    leftLayout->addLayout(bufCtrl);
    leftLayout->addLayout(displaySet);

    // Right parameter area (scrollable)
    QScrollArea* rightPanel = new QScrollArea;
    rightPanel->setMinimumWidth(320);
    QWidget* rightWidget = new QWidget;
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);

    QGroupBox* gBase = new QGroupBox(tr("基础配置"));
    QFormLayout* gBaseLayout = new QFormLayout(gBase);
    m_exposureMode = new QComboBox; m_exposureMode->addItems({ tr("自动"), tr("手动") });
    m_exposureManualValue = new QDoubleSpinBox; m_exposureManualValue->setRange(0.06, 200.0); m_exposureManualValue->setSuffix(" ms");
    m_frameAverage = new QComboBox; m_frameAverage->addItems({ "2","5","10" });
    m_filterMode = new QComboBox; m_filterMode->addItems({ tr("无"), tr("平滑"), tr("去斑"), tr("IR 校正") });
    m_btnBgCapture = new QPushButton(tr("捕获背景")); m_btnBgLoad = new QPushButton(tr("加载背景"));
    m_gainSlider = new QSlider(Qt::Horizontal); m_gainSlider->setRange(1, 10);
    m_adcBits = new QComboBox; m_adcBits->addItems({ "10","12" });
    m_pmfValue = new QDoubleSpinBox; m_pmfValue->setRange(1.0, 16.0);

    gBaseLayout->addRow(tr("曝光:"), m_exposureMode);
    gBaseLayout->addRow(tr("曝光手动值:"), m_exposureManualValue);
    gBaseLayout->addRow(tr("帧平均:"), m_frameAverage);
    gBaseLayout->addRow(tr("滤波:"), m_filterMode);
    QHBoxLayout* bgRow = new QHBoxLayout; bgRow->addWidget(m_btnBgCapture); bgRow->addWidget(m_btnBgLoad);
    gBaseLayout->addRow(tr("背景减除:"), bgRow);
    gBaseLayout->addRow(tr("增益:"), m_gainSlider);
    gBaseLayout->addRow(tr("ADC 位数:"), m_adcBits);
    gBaseLayout->addRow(tr("像素倍增因子 (PMF):"), m_pmfValue);

    QGroupBox* gMeasure = new QGroupBox(tr("测量配置"));
    QFormLayout* gMeasureLayout = new QFormLayout(gMeasure);
    QComboBox* beamDef = new QComboBox; beamDef->addItems({ tr("4 Sigma"), tr("FWHM"), tr("1/e²"), tr("D86") });
    QComboBox* crossCenter = new QComboBox; crossCenter->addItems({ tr("质心"), tr("峰值"), tr("自定义") });
    gMeasureLayout->addRow(tr("光束定义:"), beamDef);
    gMeasureLayout->addRow(tr("十字线中心:"), crossCenter);

    QGroupBox* gCalib = new QGroupBox(tr("校准与原点"));
    QFormLayout* gCalibLayout = new QFormLayout(gCalib);
    QPushButton* btnCustomOrigin = new QPushButton(tr("自定义原点"));
    QPushButton* btnResetOrigin = new QPushButton(tr("重置原点"));
    QLabel* originX = new QLabel("0.0"); QLabel* originY = new QLabel("0.0");
    gCalibLayout->addRow(btnCustomOrigin, btnResetOrigin);
    gCalibLayout->addRow(tr("X 原点:"), originX);
    gCalibLayout->addRow(tr("Y 原点:"), originY);

    rightLayout->addWidget(gBase);
    rightLayout->addWidget(gMeasure);
    rightLayout->addWidget(gCalib);
    rightLayout->addStretch();
    rightPanel->setWidget(rightWidget);
    rightPanel->setWidgetResizable(true);

    midH->addWidget(leftPanel, 3);
    midH->addWidget(rightPanel, 1);

    mainV->addLayout(midH);

    // Bottom results table
    m_resultsTable = new QTableWidget;
    QStringList paramNames = { tr("x 轴直径"), tr("y 轴直径"), tr("平均直径"),
                              tr("有效直径"), tr("椭圆度"), tr("取向角"),
                              tr("质心 X"), tr("质心 Y") };
    m_resultsTable->setRowCount(paramNames.size());
    m_resultsTable->setColumnCount(2);
    m_resultsTable->setHorizontalHeaderLabels({ tr("参数"), tr("值") });
    for (int i = 0; i < paramNames.size(); ++i) {
        m_resultsTable->setItem(i, 0, new QTableWidgetItem(paramNames.at(i)));
        m_resultsTable->setItem(i, 1, new QTableWidgetItem("-"));
    }

    mainV->addWidget(new QLabel(tr("测量结果:")));
    mainV->addWidget(m_resultsTable);

    updateConnectionIndicator(false);
}

void BeamAnalyzerWidget::setupConnections()
{
    connect(m_btnStart, &QPushButton::clicked, this, &BeamAnalyzerWidget::startAcquisition);
    connect(m_btnStop, &QPushButton::clicked, this, &BeamAnalyzerWidget::stopAcquisition);
    connect(m_btnDisconnect, &QPushButton::clicked, this, &BeamAnalyzerWidget::disconnectDevice);
    connect(m_btnPrevFrame, &QPushButton::clicked, this, &BeamAnalyzerWidget::onPrevFrame);
    connect(m_btnNextFrame, &QPushButton::clicked, this, &BeamAnalyzerWidget::onNextFrame);
    connect(m_btnPlayAnim, &QPushButton::clicked, this, &BeamAnalyzerWidget::onPlayPauseAnimation);
    connect(m_btnSaveCurrent, &QPushButton::clicked, this, &BeamAnalyzerWidget::onSaveCurrentImage);
    connect(m_btnSaveAll, &QPushButton::clicked, this, &BeamAnalyzerWidget::onSaveAllBuffer);
    connect(m_btnExport, &QPushButton::clicked, this, &BeamAnalyzerWidget::onExportData);
    connect(m_btnPrint, &QPushButton::clicked, this, &BeamAnalyzerWidget::onPrintReport);
    connect(m_btnOpenBG, &QPushButton::clicked, this, &BeamAnalyzerWidget::onOpenBackground);
    connect(m_btnOpenCfg, &QPushButton::clicked, this, &BeamAnalyzerWidget::onLoadConfiguration);
    connect(m_timer, &QTimer::timeout, this, &BeamAnalyzerWidget::onTimerTick);
}

void BeamAnalyzerWidget::startAcquisition()
{
    if (!m_connected) {
        connectDevice();
    }
    // 若已载入图像序列，则从 buffer 播放，否则启用空帧
    if (!m_buffer.isEmpty()) {
        if (!m_timer->isActive()) m_timer->start();
    }
}

void BeamAnalyzerWidget::stopAcquisition()
{
    if (m_timer->isActive()) m_timer->stop();
}

void BeamAnalyzerWidget::connectDevice()
{
    // 模拟连接：设置已连接状态并显示序列号（这里可替换为真实设备接入）
    m_connected = true;
    m_serial = QStringLiteral("SIM-BA-0001");
    m_serialLabel->setText(tr("S/N: %1").arg(m_serial));
    updateConnectionIndicator(true);
}

void BeamAnalyzerWidget::disconnectDevice()
{
    stopAcquisition();
    m_connected = false;
    m_serial.clear();
    m_serialLabel->setText(tr("S/N: -"));
    updateConnectionIndicator(false);
}

bool BeamAnalyzerWidget::loadImageFolder(const QString& folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists()) return false;
    QStringList nameFilters = { "*.png","*.bmp","*.jpg","*.jpeg","*.tif","*.tiff" };
    QFileInfoList fis = dir.entryInfoList(nameFilters, QDir::Files, QDir::Name);
    QVector<QImage> imgs;
    for (const QFileInfo& fi : fis) {
        QImage img(fi.absoluteFilePath());
        if (!img.isNull()) imgs.append(img.convertToFormat(QImage::Format_RGB32));
    }
    if (imgs.isEmpty()) return false;
    m_buffer = imgs;
    m_currentFrameIndex = 0;
    m_frameIndex->setMaximum(qMax(0, m_buffer.size() - 1));
    if (!m_buffer.isEmpty()) updateFrameView(m_buffer.constFirst());
    return true;
}

void BeamAnalyzerWidget::onPrevFrame()
{
    if (m_buffer.isEmpty()) return;
    m_currentFrameIndex = qMax(0, m_currentFrameIndex - 1);
    updateFrameView(m_buffer.at(m_currentFrameIndex));
    m_frameIndex->setValue(m_currentFrameIndex);
}

void BeamAnalyzerWidget::onNextFrame()
{
    if (m_buffer.isEmpty()) return;
    m_currentFrameIndex = qMin(m_buffer.size() - 1, m_currentFrameIndex + 1);
    updateFrameView(m_buffer.at(m_currentFrameIndex));
    m_frameIndex->setValue(m_currentFrameIndex);
}

void BeamAnalyzerWidget::onPlayPauseAnimation()
{
    if (m_timer->isActive()) {
        m_timer->stop();
        m_btnPlayAnim->setText(tr("播放"));
    }
    else {
        if (!m_buffer.isEmpty()) m_timer->start();
        m_btnPlayAnim->setText(tr("暂停"));
    }
}

void BeamAnalyzerWidget::onSaveCurrentImage()
{
    if (m_buffer.isEmpty()) return;
    QString fn = QFileDialog::getSaveFileName(this, tr("保存当前图像"), QString(), tr("PNG Files (*.png);;BMP Files (*.bmp)"));
    if (fn.isEmpty()) return;
    m_buffer.at(m_currentFrameIndex).save(fn);
}

void BeamAnalyzerWidget::onSaveAllBuffer()
{
    if (m_buffer.isEmpty()) return;
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择保存目录"));
    if (dir.isEmpty()) return;
    for (int i = 0; i < m_buffer.size(); ++i) {
        QString fn = QString("%1/frame_%2.png").arg(dir).arg(i, 4, 10, QChar('0'));
        m_buffer.at(i).save(fn);
    }
}

void BeamAnalyzerWidget::onExportData()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("导出测量数据"), QString(), tr("CSV Files (*.csv)"));
    if (fn.isEmpty()) return;
    QFile f(fn);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream ts(&f);
    ts << "param,value\n";
    for (int r = 0; r < m_resultsTable->rowCount(); ++r) {
        QString p = m_resultsTable->item(r, 0)->text();
        QString v = m_resultsTable->item(r, 1)->text();
        ts << QString("%1,%2\n").arg(p).arg(v);
    }
    f.close();
}

void BeamAnalyzerWidget::onPrintReport()
{
    QPrinter printer;
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.drawText(QPoint(100, 100), tr("Beam Analyzer Report"));
    }
}

void BeamAnalyzerWidget::onOpenBackground()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("打开背景图"), QString(), tr("Image Files (*.png *.bmp *.jpg)"));
    if (fn.isEmpty()) return;
    // TODO: 将背景图应用到显示或减去背景
}

void BeamAnalyzerWidget::onLoadConfiguration()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("打开配置文件"), QString(), tr("INI Files (*.ini);;JSON Files (*.json)"));
    if (fn.isEmpty()) return;
    // TODO: 解析并应用配置
}

void BeamAnalyzerWidget::onTimerTick()
{
    if (m_buffer.isEmpty()) return;
    m_currentFrameIndex = (m_currentFrameIndex + 1) % m_buffer.size();
    updateFrameView(m_buffer.at(m_currentFrameIndex));
    m_frameIndex->setValue(m_currentFrameIndex);

    // TODO: 图像处理/测量逻辑调用位置，结果填到 m_resultsTable
    // 示例：填入假数据以验证 UI
    if (m_resultsTable->rowCount() >= 1) {
        m_resultsTable->item(0, 1)->setText(QString::number(100.0 + (m_currentFrameIndex % 10)));
    }
}

void BeamAnalyzerWidget::updateConnectionIndicator(bool connected)
{
    if (connected) m_connectionIndicator->setStyleSheet("background-color:green;border-radius:7px;");
    else m_connectionIndicator->setStyleSheet("background-color:red;border-radius:7px;");
}

void BeamAnalyzerWidget::updateFrameView(const QImage& img)
{
    QLabel* disp = qobject_cast<QLabel*>(m_displayStack->widget(0));
    if (disp) {
        disp->setPixmap(QPixmap::fromImage(img).scaled(disp->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void BeamAnalyzerWidget::updateResultsTable()
{
    Q_UNUSED(this);
    // TODO: 用实际测量结果更新表格
}