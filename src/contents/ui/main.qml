// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.ChatQTP 1.0
import QtWebSockets 1.2
import QtWebChannel 1.0

Kirigami.ApplicationWindow {
    id: root

    title: i18n("ChatQTP")

    minimumWidth: Kirigami.Units.gridUnit * 30
    minimumHeight: Kirigami.Units.gridUnit * 27

    onClosing: App.saveWindowGeometry(root)

    onWidthChanged: saveWindowGeometryTimer.restart()
    onHeightChanged: saveWindowGeometryTimer.restart()
    onXChanged: saveWindowGeometryTimer.restart()
    onYChanged: saveWindowGeometryTimer.restart()

    Component.onCompleted: App.restoreWindowGeometry(root)

    // This timer allows to batch update the window size change to reduce
    // the io load and also work around the fact that x/y/width/height are
    // changed when loading the page and overwrite the saved geometry from
    // the previous session.
    Timer {
        id: saveWindowGeometryTimer
        interval: 1000
        onTriggered: App.saveWindowGeometry(root)
    }

    property int counter: 0

        globalDrawer: Kirigami.GlobalDrawer {
            title: i18n("ChatQTP")
            titleIcon: "applications-graphics"
            isMenu: !root.isMobile
            actions: [
                Kirigami.Action {
                    text: i18n("About ChatQTP")
                    icon.name: "help-about"
                    onTriggered: pageStack.layers.push('qrc:About.qml')
                },
                Kirigami.Action {
                    text: i18n("Quit")
                    icon.name: "application-exit"
                    onTriggered: Qt.quit()
                }
            ]
        }

        contextDrawer: Kirigami.ContextDrawer {
            id: contextDrawer
        }

        pageStack.initialPage: page

        // Kirigami.ScrollablePage {
        Kirigami.Page {
            id: page

            Layout.fillWidth: true
            Layout.fillHeight: true

            title: i18n("Chat")

            actions.main: Kirigami.Action {
                text: i18n("Copy")
                icon.name: "edit-copy"
                tooltip: i18n("Copy the response")
                onTriggered: {
                    responseField.copy()
                }
            }

            Kirigami.FormLayout {
                anchors.fill: parent

                Layout.fillWidth: true
                Layout.fillHeight: true

                wideMode: false

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Kirigami.FormData.label: "Response"

                    Controls.ScrollView {
                        anchors.fill: parent
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        rightPadding: 0

                        Layout.minimumHeight: 150

                        Controls.TextArea {
                            id: responseField
                            readOnly: true

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            rightPadding: Kirigami.Units.gridUnit * 1

                            placeholderText: i18n("I am waiting for your question...")
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Kirigami.FormData.label: "Message"

                    Controls.ScrollView {
                        anchors.fill: parent
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        rightPadding: 0

                        Layout.minimumHeight: 150

                        Controls.TextArea {
                            id: messageField

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            placeholderText: i18n("Type here what you want to ask...")
                        }
                    }
                }

                Controls.Button {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    text: "Send"
                    onClicked: {
                        App.request(responseField, messageField.text)
                    }
                }
            }
        }
    }
