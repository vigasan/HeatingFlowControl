#include "frameprocessor.h"
#include <QTimer>
#include <QDebug>

FrameProcessor::FrameProcessor(QQueue<Frame*> *outFrameQueue, QObject *parent) : QObject(parent)
{
    m_outFrameQueue = outFrameQueue;

    QTimer::singleShot(4000, this, &FrameProcessor::timeout);
}

void FrameProcessor::timeout()
{
    qDebug()<< "INIT COMMUNICATION!!!";
    initCommunication();
}

void FrameProcessor::FrameIncoming(Frame *frame)
{
    if (frame != nullptr)
    {
        quint8 cmd = frame->GetCmd();

        qDebug()<< "CMD: " << cmd;

        switch (cmd)
        {

            case CMD_SYSTEM_STATUS:
            {
                emit changedSystemStatus(frame->GetUByte());
            } break;

            case CMD_WATER_TEMP:
            {
                emit changedWaterTemp(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_ENV_TEMP_1:
            {
                emit changedEnvTemp1(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_TARGET_TEMP_1:
            {
                emit changedTargetTemp1(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_STATUS_HEATING_1:
            {
                emit changedStatusHeating1(frame->GetUByte());
            } break;

            case CMD_ENV_TEMP_2:
            {
                emit changedEnvTemp2(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_TARGET_TEMP_2:
            {
                emit changedTargetTemp2(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_STATUS_HEATING_2:
            {
                emit changedStatusHeating2(frame->GetUByte());
            } break;

            case CMD_ENV_TEMP_3:
            {
                emit changedEnvTemp3(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_TARGET_TEMP_3:
            {
                emit changedTargetTemp3(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_STATUS_HEATING_3:
            {
                emit changedStatusHeating3(frame->GetUByte());
            } break;

            case CMD_ENV_TEMP_4:
            {
                emit changedEnvTemp4(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_TARGET_TEMP_4:
            {
                emit changedTargetTemp4(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_STATUS_HEATING_4:
            {
                emit changedStatusHeating4(frame->GetUByte());
            } break;

            case CMD_ENV_TEMP_5:
            {
                emit changedEnvTemp5(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_TARGET_TEMP_5:
            {
                emit changedTargetTemp5(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_STATUS_HEATING_5:
            {
                emit changedStatusHeating5(frame->GetUByte());
            } break;

            case CMD_ENV_TEMP_6:
            {
                emit changedEnvTemp6(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_TARGET_TEMP_6:
            {
                emit changedTargetTemp6(static_cast<float>(frame->GetUInt16()) / 10);
            } break;

            case CMD_STATUS_HEATING_6:
            {
                emit changedStatusHeating6(frame->GetUByte());
            } break;

        }
    }
}

void FrameProcessor::initCommunication()
{
    Frame *frameToSend = new Frame(CMD_INIT_SYSTEM, 0);
    m_outFrameQueue->enqueue(frameToSend);
}

void FrameProcessor::setTargetTemp(quint8 cmd, quint16 value)
{

    qDebug()<< "CMD: " << cmd << "Target: " << value;
    Frame *frameToSend = new Frame(cmd, value);
    m_outFrameQueue->enqueue(frameToSend);
}

void FrameProcessor::setStatus(quint8 value)
{
    Frame *frameToSend = new Frame(CMD_SYSTEM_STATUS, value);
    m_outFrameQueue->enqueue(frameToSend);
}


