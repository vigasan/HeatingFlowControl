import QtQuick 2.12
import QtQuick.Window 2.12

Window
{
    id: rootPage
    visible: true
    width: 800
    height: 480
    color: "black"


    readonly property int cmd_INIT_SYSTEM:      1
    readonly property int cmd_SYSTEM_STATUS:    2
    readonly property int cmd_WATER_TEMP:       3

    readonly property int cmd_ENV_TEMP_1:       10
    readonly property int cmd_TARGET_TEMP_1:    11
    readonly property int cmd_STATUS_HEATING_1: 12

    readonly property int cmd_ENV_TEMP_2:       13
    readonly property int cmd_TARGET_TEMP_2:    14
    readonly property int cmd_STATUS_HEATING_2: 15

    readonly property int cmd_ENV_TEMP_3:       16
    readonly property int cmd_TARGET_TEMP_3:    17
    readonly property int cmd_STATUS_HEATING_3: 18

    readonly property int cmd_ENV_TEMP_4:       19
    readonly property int cmd_TARGET_TEMP_4:    20
    readonly property int cmd_STATUS_HEATING_4: 21

    readonly property int cmd_ENV_TEMP_5:       22
    readonly property int cmd_TARGET_TEMP_5:    23
    readonly property int cmd_STATUS_HEATING_5: 24

    readonly property int cmd_ENV_TEMP_6:       25
    readonly property int cmd_TARGET_TEMP_6:    26
    readonly property int cmd_STATUS_HEATING_6: 27

    property real g_targetTemp1: 15.0
    property real g_realTemp1: 0.0
    property real g_targetTemp2: 15.0
    property real g_realTemp2: 0.0
    property real g_targetTemp3: 15.0
    property real g_realTemp3: 0.0
    property real g_targetTemp4: 15.0
    property real g_realTemp4: 0.0
    property real g_targetTemp5: 15.0
    property real g_realTemp5: 0.0
    property real g_targetTemp6: 15.0
    property real g_realTemp6: 0.0




    FontLoader
    {
       id: fontHelvetica
       source:"qrc:/images/helvetica.ttf"
    }

    FontLoader
    {
       id: fontHelveticaSemibold
       source:"qrc:/images/HelveticaSemiBold.ttf"
    }

    signal reqHomePage()
    signal reqControlPage()

    onReqHomePage:
    {
        //homePage.visible = true;
        homePage.updateTargetTemperature();
        homePage.enabled = true;
        controlPage.visible = false;
    }

    onReqControlPage:
    {
        //homePage.visible = false;
        homePage.enabled = false;
        controlPage.visible = true;
    }

    //////////////////////////////////////////////////////////////
    // Pages
    //////////////////////////////////////////////////////////////
    Home
    {
       id: homePage
       width: parent.width
       height: parent.height
       visible: true
    }

    Control
    {
        id: controlPage
        width: 500
        height: 300
        visible: false
    }


}
