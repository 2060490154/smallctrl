#pragma once
#pragma execution_character_set("utf-8")
#ifndef BEAMANALYZERWIDGET_H
#define BEAMANALYZERWIDGET_H

#include <QWidget>
#include <QVector>
#include <QImage>

class QLabel;
class QPushButton;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QStackedWidget;
class QSlider;
class QCheckBox;
class QLineEdit;
class QTableWidget;
class QTimer;

class BeamAnalyzerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BeamAnalyzerWidget(QWidget* parent = nullptr);
    ~BeamAnalyzerWidget() override;

    // 外部可以调用以加载本地序列（用于无法直接连设备时）
    bool loadImageFolder(const QString& folderPath);

signals:
    // 对外暴露的高层事件（可选）
    void requestOpen();
    void requestClose();

public slots:
    void startAcquisition();
    void stopAcquisition();
    void connectDevice();
    void disconnectDevice();

private slots:
    void onPrevFrame();
    void onNextFrame();
    void onPlayPauseAnimation();
    void onSaveCurrentImage();
    void onSaveAllBuffer();
    void onExportData();
    void onPrintReport();
    void onOpenBackground();
    void onLoadConfiguration();
    void onTimerTick();

private:
    void setupUi();
    void setupConnections();
    void updateConnectionIndicator(bool connected);
    void updateFrameView(const QImage& img);
    void updateResultsTable();

private:
    // Top
    QLabel* m_connectionIndicator;
    QLabel* m_serialLabel;
    QPushButton* m_btnDisconnect;
    QPushButton* m_btnStart;
    QPushButton* m_btnStop;

    QComboBox* m_modeCombo;
    QComboBox* m_displayCombo;

    QPushButton* m_btnSaveCurrent;
    QPushButton* m_btnSaveAll;
    QPushButton* m_btnExport;
    QPushButton* m_btnPrint;
    QPushButton* m_btnOpenBG;
    QPushButton* m_btnOpenCfg;

    // Left (display)
    QStackedWidget* m_displayStack;
    QPushButton* m_btnZoomIn;
    QPushButton* m_btnZoomOut;
    QPushButton* m_btnRotate;
    QPushButton* m_btnPan;
    QPushButton* m_btnScreenshot;
    QPushButton* m_btnResetView;

    QSpinBox* m_frameIndex;
    QPushButton* m_btnPrevFrame;
    QPushButton* m_btnNextFrame;
    QPushButton* m_btnPlayAnim;
    QSpinBox* m_bufferSize;

    QCheckBox* m_chkColorBar;
    QCheckBox* m_chkGray;
    QCheckBox* m_chkDiameterEllipse;
    QCheckBox* m_chkCrosshair;

    // Right (parameters)
    QComboBox* m_exposureMode;
    QDoubleSpinBox* m_exposureManualValue;
    QComboBox* m_frameAverage;
    QComboBox* m_filterMode;
    QPushButton* m_btnBgCapture;
    QPushButton* m_btnBgLoad;
    QSlider* m_gainSlider;
    QComboBox* m_adcBits;
    QDoubleSpinBox* m_pmfValue;

    // Bottom results
    QTableWidget* m_resultsTable;

    // internal
    QVector<QImage> m_buffer;
    QTimer* m_timer;
    int m_currentFrameIndex;
    bool m_connected;
    QString m_serial;
};

#endif // BEAMANALYZERWIDGET_H