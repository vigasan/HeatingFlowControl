import QtQuick 2.2

Item
{
    id:parameterBig
    height: 158
    width: 280

    property string paramName
    property string param2Name
    property real paramValue: 100
    property string paramUnit: " "
    property int containerWidth: 150
    property real minValue: 10
    property real maxValue: 2000
    property real delta: 10
    property int decimal: 0

    property color ctrl_color: "#E87168"
    property string più_Color: "qrc:/images/PiuGrande.png"
    property string meno_Color:"qrc:/images/MenoGrande.png"

    property int valuePosition: 60
    property int txtposition: 25
    property int txt2position: 40


    property int counterTimer: 0
    property bool isIncrementing: true;
    property bool btnPressed: false;
    property bool ctrlButtonsEnabled: true

    signal sendParameter()
    signal delayParameter()
    signal start()
    signal stop()

    function increment()
    {
        if(paramValue < maxValue)
            paramValue += delta
    }

    function decrement()
    {
        if(paramValue > minValue)
            paramValue -= delta
    }

    /*function resetButton()
    {
        btnPressed = false;
        btnPressed = false;
        txtBtn.text = "START";
        btn.source =  "qrc://..//..//Library/Images/BottonArancioContorno.png";
    }*/

    /*
    FontLoader
    {
        id: fontHelveticaSemibold
        source:"qrc:/images/HELVETICA-SEMIBOLD.ttf"
    }
    */


    Rectangle
    {
        id: circle
        width: 154
        height: width
        color: "transparent"
        anchors.centerIn: parent
        //anchors.horizontalCenter: parent.horizontalCenter
        //anchors.bottom: parent.bottom
        border.color: ctrl_color
        border.width: 2
        radius: width * 0.5

        Text    // Titolo del parametro
        {
            id:txtParamName
            text: paramName
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: txtposition //25
            anchors.top: parent.top
            font.pointSize: 13
            font.family: fontHelveticaSemibold.name
        }

        Text    // Titolo del parametro
        {
            id:txtParamName2
            text: param2Name
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: txt2position
            anchors.top: parent.top
            font.pointSize: 13
            font.family: fontHelveticaSemibold.name
        }

        Text    // Valore del parametro
        {
            id:txtParamValue
            text: paramValue.toFixed(decimal)
            color: "white"
            //anchors.centerIn: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: valuePosition //60
            font.pointSize: 28
            //font.family: fontHelveticaBold.name
            font.family: fontHelveticaSemibold.name
        }

        Text    // Unità di misura del parametro
        {
            id:txtParamUnit
            text: paramUnit
            color: "white"
            //anchors.left: txtParamValue.right
            //anchors.leftMargin: 5
            anchors.horizontalCenter:parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            font.pointSize: 12
            font.family: fontHelveticaSemibold.name
        }
    }

    ////////////////////////////////////////////////////////////////////
    // Pulsante
    ////////////////////////////////////////////////////////////////////
/*    Image
    {
        id: btn
        source: "qrc:/../../Library/Images/BottonArancioContorno.png"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                if(btnPressed == false)
                {
                    btnPressed = true;
                    txtBtn.text = "STOP";
                    btn.source =  "qrc://..//..//Library/Images/BottonArancio.png";
                    start();
                    //appBridge.startTreatment(1); // MAN_MULTIPOLARE_4RF
                } else
                {
                    btnPressed = false;
                    txtBtn.text = "START";
                    btn.source =  "qrc://..//..//Library/Images/BottonArancioContorno.png";
                    stop();
                    //appBridge.stopTreatment();
                }
            }
        }
    }

    Text
    {
        id:txtBtn
        text: "START"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28
        color: "white"
        font.pointSize: 14
        font.family: fontHelveticaSemibold.name
    }
*/
    /////////////////////////////////////////////////////////////////////

    Image // Pulsante Left (decrementa)
    {
        id:idLeftBtn
        source: meno_Color
        anchors.verticalCenter: parent.verticalCenter
        //anchors.bottom: parent.bottom
        anchors.left: parent.left
        visible: ctrlButtonsEnabled
        MouseArea
        {
            enabled: ctrlButtonsEnabled
            anchors.fill: parent
            onPressed:
            {
                isIncrementing = false
                timerIncrement.interval = 700
                timerIncrement.start()
                timerIncrement.repeat = true;
                decrement()
                delayParameter()
            }

            onReleased:
            {
                timerIncrement.stop()
                timerIncrement.repeat = false
                counterTimer = 0
                sendParameter()
            }
        }
    }

    Image // Pulsante Right (incrementa)
    {
        id:idRightBtn
        source: più_Color
        anchors.verticalCenter: parent.verticalCenter
        //anchors.bottom: parent.bottom
        anchors.right: parent.right
        visible: ctrlButtonsEnabled
        MouseArea
        {
            enabled: ctrlButtonsEnabled
            anchors.fill: parent
            onPressed:
            {
                isIncrementing = true
                timerIncrement.interval = 700
                timerIncrement.start()
                timerIncrement.repeat = true;
                increment()
                delayParameter()
            }

            onReleased:
            {
                timerIncrement.stop()
                timerIncrement.repeat = false
                counterTimer = 0
                sendParameter()
            }
        }
    }


    Timer
    {
        id: timerIncrement
        interval: 700
        running: false
        repeat: false
        onTriggered:
        {
            counterTimer++
            if(counterTimer > 2 && counterTimer < 10)
                interval = 300;
            //else if(counterTimer >= 5 && counterTimer < 10)
            //    interval = 250
            else if(counterTimer >= 10)
                interval = 100

            if(isIncrementing)
                increment()
            else
                decrement()
        }
    }


}
