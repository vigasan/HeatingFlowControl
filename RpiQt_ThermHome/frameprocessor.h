#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include <QObject>
#include "frame.h"
#include "serialworker.h"
#include <QQueue>

class FrameProcessor : public QObject
{
    static const quint8 CMD_INIT_SYSTEM = 1;
    static const quint8 CMD_SYSTEM_STATUS = 2;
    static const quint8 CMD_WATER_TEMP = 3;

    static const quint8 CMD_ENV_TEMP_1 = 10;
    static const quint8 CMD_TARGET_TEMP_1 = 11;
    static const quint8 CMD_STATUS_HEATING_1 =  12;

    static const quint8 CMD_ENV_TEMP_2 = 13;
    static const quint8 CMD_TARGET_TEMP_2 = 14;
    static const quint8 CMD_STATUS_HEATING_2 = 15;

    static const quint8 CMD_ENV_TEMP_3 = 16;
    static const quint8 CMD_TARGET_TEMP_3 = 17;
    static const quint8 CMD_STATUS_HEATING_3 = 18;

    static const quint8 CMD_ENV_TEMP_4 = 19;
    static const quint8 CMD_TARGET_TEMP_4 = 20;
    static const quint8 CMD_STATUS_HEATING_4 = 21;

    static const quint8 CMD_ENV_TEMP_5 = 22;
    static const quint8 CMD_TARGET_TEMP_5 = 23;
    static const quint8 CMD_STATUS_HEATING_5 = 24;

    static const quint8 CMD_ENV_TEMP_6 = 25;
    static const quint8 CMD_TARGET_TEMP_6 = 26;
    static const quint8 CMD_STATUS_HEATING_6 = 27;

    Q_OBJECT
public:
    explicit FrameProcessor(QQueue<Frame*> *outFrameQueue, QObject *parent = nullptr);

private:
    //SerialWorker *m_serialWorker;
    QQueue<Frame*> *m_outFrameQueue;

signals:
    void changedSystemStatus(int value);
    void changedWaterTemp(float value);
    void changedEnvTemp1(float value);
    void changedTargetTemp1(float value);
    void changedStatusHeating1(int value);
    void changedEnvTemp2(float value);
    void changedTargetTemp2(float value);
    void changedStatusHeating2(int value);
    void changedEnvTemp3(float value);
    void changedTargetTemp3(float value);
    void changedStatusHeating3(int value);
    void changedEnvTemp4(float value);
    void changedTargetTemp4(float value);
    void changedStatusHeating4(int value);
    void changedEnvTemp5(float value);
    void changedTargetTemp5(float value);
    void changedStatusHeating5(int value);
    void changedEnvTemp6(float value);
    void changedTargetTemp6(float value);
    void changedStatusHeating6(int value);

public slots:
    void FrameIncoming(Frame *frame);
    void timeout();

    void initCommunication();
    void setTargetTemp(quint8 cmd, quint16 value);
    void setStatus(quint8 value);
};

#endif // FRAMEPROCESSOR_H
