#ifndef OSCIDEFINE_H
#define OSCIDEFINE_H

#define M_OSCI_CHANNEL_NUM     4


const int M_MAX_POINTS=500000;


enum _TriggerMode{
    M_TRIGGER_MODE_AUTO = 0,
    M_TRIGGER_MODE_NORMAL = 1,
    M_TRIGGER_MODE_SIGNAL = 2,
};


typedef struct  _tOsciiChannel
{
    QString m_sChannelName;
    int m_nChannelNum;

}tOsciiChannel;


#endif // OSCIDEFINE_H
