/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.4

TabbedPane {
    showTabsOnActionBar: true
    Tab{
        title: "Client"
        Page {
            titleBar: TitleBar {
                title: "Client setup"
            }
            Container {
                layout: StackLayout {

                }
                Label {
                    text: "Select IP-Address to connect to:"
                }
                Container {
                    layout: DockLayout {

                    }

                    TextField {
                        id: clientIPTextField
                        text: "127.0.0.1"
                        enabled: true
                    }
                    Button {
                        id: clientConnectButton
                        text: "Connect"
                        horizontalAlignment: HorizontalAlignment.Right
                    }
                }
                TextArea {
                    id: clientConsole
                    editable: false
                    textStyle.textAlign: TextAlign.Left
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                }
            }
        }
    }
    Tab {
        title: "Server"
        Page {
            titleBar: TitleBar {
                title: "Server setup"
            }
            Container {
                Label {
                    text: "Start the server to see the IP-Adress:"
                }
                Container {
                    layout: DockLayout {

                    }
                    TextField {
                        id: serverIPTextField
                        text: "127.0.0.1"
                        enabled: false

                    }
                    Button {
                        id: serverConnectButton
                        text: "Start server"
                        horizontalAlignment: HorizontalAlignment.Right
                        onClicked: {
                            serverIPTextField.text = "127.0.0.2"
                        }
                    }
                }
                TextArea {
                    id: serverConsole
                    editable: false
                    textStyle.textAlign: TextAlign.Left
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                }
            }
        }
    }
}
