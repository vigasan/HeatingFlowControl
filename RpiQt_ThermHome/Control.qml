import QtQuick 2.0

Rectangle
{
    id: controlPage

    color: "black"
    anchors.centerIn: parent
    border.color: "white"
    border.width: 3
    radius: 5

    property string nameZone: " "
    property real targetTemp: 18.0
    property int cmdTargetTemp: 0

    function initTarget(value)
    {
        targetTemp = value;
        ctrlTargetTemperature.paramValue = targetTemp;
    }

    Text
    {
       text: nameZone
       anchors.horizontalCenter: parent.horizontalCenter
       anchors.bottom: ctrlTargetTemperature.top
       anchors.bottomMargin: 10
       font.pixelSize: 20
       font.family: fontHelvetica.name
       color: "white"
    }

    Ctrl_ParameterBig
    {
        id: ctrlTargetTemperature
        anchors.centerIn: parent
        paramName: "Target"
        paramUnit: "°C"
        minValue: 13
        maxValue: 28
        delta: 0.1
        paramValue: targetTemp
        decimal: 1
        ctrl_color: "white"
        meno_Color:"qrc:/images/MenoBGrande.png"
        più_Color:"qrc:/images/PiuBGrande.png"
    }

    Image
    {
        id: back
        source: "qrc:images/back.png"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        //scale: 0.8

        MouseArea
        {
            anchors.fill: parent
            onPressed:
            {
                switch(cmdTargetTemp)
                {
                    case cmd_TARGET_TEMP_1: g_targetTemp1 = ctrlTargetTemperature.paramValue; break;
                    case cmd_TARGET_TEMP_2: g_targetTemp2 = ctrlTargetTemperature.paramValue; break;
                    case cmd_TARGET_TEMP_3: g_targetTemp3 = ctrlTargetTemperature.paramValue; break;
                    case cmd_TARGET_TEMP_4: g_targetTemp4 = ctrlTargetTemperature.paramValue; break;
                    case cmd_TARGET_TEMP_5: g_targetTemp5 = ctrlTargetTemperature.paramValue; break;
                    case cmd_TARGET_TEMP_6: g_targetTemp6 = ctrlTargetTemperature.paramValue; break;
                }


                serial.setTargetTemp(cmdTargetTemp, ctrlTargetTemperature.paramValue * 10.0);
                rootPage.reqHomePage();
            }
        }
    }

}
