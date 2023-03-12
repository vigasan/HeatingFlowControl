import QtQuick 2.0

Rectangle
{
    id: homePage
    color: "transparent"

    function updateTargetTemperature()
    {
        ctrlZone1.targetTemperature = g_targetTemp1;
        ctrlZone2.targetTemperature = g_targetTemp2;
        ctrlZone3.targetTemperature = g_targetTemp3;
        ctrlZone4.targetTemperature = g_targetTemp4;
        ctrlZone5.targetTemperature = g_targetTemp5;
        ctrlZone6.targetTemperature = g_targetTemp6;
    }

    Image
    {
      source: "qrc:/images/plan.png"
      //anchors.top: parent.top
      //anchors.left: parent.left
    }

    Text
    {
       text: "Enable"
       anchors.horizontalCenter: swSystem.horizontalCenter
       anchors.bottom: swSystem.top
       anchors.bottomMargin: 5
       font.pixelSize: 16
       font.family: fontHelvetica.name
       color: "white"
    }

    Switch
    {
       id: swSystem
       anchors.top: parent.top
       anchors.topMargin: 50
       anchors.right: parent.right
       anchors.rightMargin: 100
       backgroundWidth: 100
       backgroundHeight: 50

       onSwitched:
       {
           if(on === true)
           {
               serial.setStatus(1);
           } else
           {
               serial.setStatus(0);
           }
       }
    }

    /// Water Temperature

    Text
    {
       text: "Water Temperature"
       anchors.horizontalCenter: rectWaterTemp.horizontalCenter
       anchors.bottom: rectWaterTemp.top
       anchors.bottomMargin: 10
       font.pixelSize: 16
       font.family: fontHelvetica.name
       color: "white"
    }

    Rectangle
    {
        id: rectWaterTemp
        height: 50
        width: 120
        color: "transparent"
        anchors.top: swSystem.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: swSystem.horizontalCenter
        border.color: "white"
        border.width: 1
        radius: 5

        Text
        {
            id: txtWaterTemp
            text: "0.0 °C"
            anchors.centerIn: parent
            color: "white"

            font.family: fontHelvetica.name
            font.pointSize: 18
        }
    }



    ItemTemperature
    {
        id: ctrlZone1 // Cucina
        x: 90
        y: 260
        realTemperature: g_realTemp1
        targetTemperature: g_targetTemp1
        status: "on"
        onPressed:
        {
            controlPage.nameZone = "Cucina"
            //controlPage.targetTemp = g_targetTemp1
            controlPage.initTarget(g_targetTemp1)
            controlPage.cmdTargetTemp = cmd_TARGET_TEMP_1
            rootPage.reqControlPage();
        }
    }

    ItemTemperature
    {
        id: ctrlZone2 // Sala TV
        x: 240
        y: 290
        realTemperature: g_realTemp2
        targetTemperature: g_targetTemp2
        status: "on"
        onPressed:
        {
            controlPage.nameZone = "Sala TV"
            //controlPage.targetTemp = g_targetTemp2
            controlPage.initTarget(g_targetTemp2)
            controlPage.cmdTargetTemp = cmd_TARGET_TEMP_2
            rootPage.reqControlPage();
        }
    }

    ItemTemperature
    {
        id: ctrlZone3 // Studio
        x: 380
        y: 300
        realTemperature: g_realTemp3
        targetTemperature: g_targetTemp3
        status: "on"
        onPressed:
        {
            controlPage.nameZone = "Studio"
            //controlPage.targetTemp = g_targetTemp3
            controlPage.initTarget(g_targetTemp3)
            controlPage.cmdTargetTemp = cmd_TARGET_TEMP_3
            rootPage.reqControlPage();
        }
    }

    ItemTemperature
    {
        id: ctrlZone4 // Cameretta
        x: 510
        y: 280
        realTemperature: g_realTemp4
        targetTemperature: g_targetTemp4
        status: "on"
        onPressed:
        {
            controlPage.nameZone = "Cameretta"
            //controlPage.targetTemp = g_targetTemp4
            controlPage.initTarget(g_targetTemp4)
            controlPage.cmdTargetTemp = cmd_TARGET_TEMP_4
            rootPage.reqControlPage();
        }
    }

    ItemTemperature
    {
        id: ctrlZone5 // Salone
        x: 300
        y: 150
        realTemperature: g_realTemp5
        targetTemperature: g_targetTemp5
        status: "on"
        onPressed:
        {
            controlPage.nameZone = "Salone"
            //controlPage.targetTemp = g_targetTemp5
            controlPage.initTarget(g_targetTemp5)
            controlPage.cmdTargetTemp = cmd_TARGET_TEMP_5
            rootPage.reqControlPage();
        }
    }

    ItemTemperature
    {
        id: ctrlZone6 // Bagno
        x: 90
        y: 50
        realTemperature: g_realTemp6
        targetTemperature: g_targetTemp6
        status: "on"
        onPressed:
        {
            controlPage.nameZone = "Bagno"
            //controlPage.targetTemp = g_targetTemp6
            controlPage.initTarget(g_targetTemp6)
            controlPage.cmdTargetTemp = cmd_TARGET_TEMP_6
            rootPage.reqControlPage();
        }
    }


    Connections
    {
        target: serial

        onChangedSystemStatus:
        {
            if(value == 1)
                swSystem.setStatus("on")
            else
                swSystem.setStatus("off")
        }

        onChangedWaterTemp:
        {
            txtWaterTemp.text = value.toFixed(1) + " °C"
        }

        onChangedEnvTemp1:
        {
            g_realTemp1 = value;
            ctrlZone1.realTemperature = value;
        }

        onChangedTargetTemp1:
        {
            g_targetTemp1 = value;
            ctrlZone1.targetTemperature = value;
        }

        onChangedStatusHeating1:
        {
            if(value == 1)
                ctrlZone1.status = "on"
            else
                ctrlZone1.status = "off"
        }

        onChangedEnvTemp2:
        {
            g_realTemp2 = value;
            ctrlZone2.realTemperature = value;
        }

        onChangedTargetTemp2:
        {
            g_targetTemp2 = value;
            ctrlZone2.targetTemperature = value;
        }

        onChangedStatusHeating2:
        {
            if(value == 1)
                ctrlZone2.status = "on"
            else
                ctrlZone2.status = "off"
        }

        onChangedEnvTemp3:
        {
            g_realTemp3 = value;
            ctrlZone3.realTemperature = value;
        }

        onChangedTargetTemp3:
        {
            g_targetTemp3 = value;
            ctrlZone3.targetTemperature = value;
        }

        onChangedStatusHeating3:
        {
            if(value == 1)
                ctrlZone3.status = "on"
            else
                ctrlZone3.status = "off"
        }

        onChangedEnvTemp4:
        {
            g_realTemp4 = value;
            ctrlZone4.realTemperature = value;
        }

        onChangedTargetTemp4:
        {
            g_targetTemp4 = value;
            ctrlZone4.targetTemperature = value;
        }

        onChangedStatusHeating4:
        {
            if(value == 1)
                ctrlZone4.status = "on"
            else
                ctrlZone4.status = "off"
        }

        onChangedEnvTemp5:
        {
            g_realTemp5 = value;
            ctrlZone5.realTemperature = value;
        }

        onChangedTargetTemp5:
        {
            g_targetTemp5 = value;
            ctrlZone5.targetTemperature = value;
        }

        onChangedStatusHeating5:
        {
            if(value == 1)
                ctrlZone5.status = "on"
            else
                ctrlZone5.status = "off"
        }

        onChangedEnvTemp6:
        {
            g_realTemp6 = value;
            ctrlZone6.realTemperature = value;
        }

        onChangedTargetTemp6:
        {
            g_targetTemp6 = value;
            ctrlZone6.targetTemperature = value;
        }

        onChangedStatusHeating6:
        {
            if(value == 1)
                ctrlZone6.status = "on"
            else
                ctrlZone6.status = "off"
        }


    }

}
