import QtQuick 2.2

Item
{
    id: control
    width: 60
    height: width

    signal pressed()

    property real realTemperature: 0
    property real targetTemperature: 0
    property string status: "off"

    function getColor()
    {
        if(status === "on")
            return "red"
        else
            return "lightblue"
    }


    Rectangle
    {
        anchors.centerIn: parent
        width: parent.width
        height: width
        radius: width * 0.5
        color: "black"
        border.color: getColor()
        border.width: 3

        Text    // Real Temperature
        {
            id:txtRealTemp
            text: realTemperature.toFixed(1)
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 10
            font.family: fontHelvetica.name
            font.pointSize: 14
        }

        Rectangle
        {
            width: parent.width * 0.8
            height: 2
            border.color: "white"
            border.width: 2
            anchors.centerIn: parent
            visible: false
        }

        Text    // Target Temperature
        {
            id:txtTargetTemp
            text: targetTemperature.toFixed(1)
            color: "lightgreen"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            font.family: fontHelvetica.name
            font.pointSize: 12
        }

        MouseArea
        {
            anchors.fill: parent
            onPressed:
            {
                control.pressed()
            }
        }
    }
}
