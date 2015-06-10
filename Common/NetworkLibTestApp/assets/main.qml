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
                    text: "Select IP-Address and port to connect to:"
                }
                Container {
                    layout: GridLayout {

                    }
                    Label {
                        text: "IP-Address:"
                    }
                    TextField {
                         id: clientIPTextField
                         text: "127.0.0.1"
                         enabled: true
                    }
                    Label {
                        text: "CMD-Port:"
                    }
                    TextField {
                        id: clientCmdPortTextField
                        text: "1337"
                    } 
                    Label {
                        text: "Data-Port:"
                    }
                    TextField {
                        id: clientDataPortTextField
                        text: "1338"
                    }  
                }
                Container {
                    layout: GridLayout {

                    }
                    Button {
                        id: clientConnectButton
                        text: "Connect"
                        horizontalAlignment: HorizontalAlignment.Left
                        onClicked: {
                            clientDisconnectButton.enabled = true;
                            clientConnectButton.enabled = false;
                            clientSocket.connectToServer(clientIPTextField.text, clientCmdPortTextField.text, clientDataPortTextField.text);
                        }
                    }
                    Button {
                        id: clientDisconnectButton
                        text: "Disconnect"
                        horizontalAlignment: HorizontalAlignment.Right
                        enabled: false
                        onClicked: {
                            clientConnectButton.enabled = true;
                            clientDisconnectButton.enabled = false;
                            clientSocket.disconnectFromServer();
                        }
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
                    text: "Select a port for listening and start the server:"
                }
                Container {
                    layout: DockLayout {

                    }
                    Container {
                        layout: GridLayout {
                        
                        }
                        Label {
                            text: "IP-Address:"
                        }
                        TextField {
                            id: serverIPTextField
                            objectName: "serverIPTextField"
                            text: "0.0.0.0"
                            enabled: false
                        }
                        Label {
                            text: "CMD-Port:"
                        }
                        TextField {
                            id: serverCmdPortTextField
                            text: "1337"
                        }
                        Label {
                            text: "Data-Port:"
                        }
                        TextField {
                            id: serverDataPortTextField
                            text: "1338"
                        }     
                    }
                }
                Container{
                    layout: GridLayout {

                    }
                    Button {
                        id: serverStartButton
                        text: "Start server"
                        horizontalAlignment: HorizontalAlignment.Left
                        onClicked: {
                            serverStopButton.enabled = true;
                            serverStartButton.enabled = false;
                            serverSocket.beginListening(serverCmdPortTextField.text, serverDataPortTextField.text);
                        }
                    }
                    Button {
                        id: serverStopButton
                        text: "Stop server"
                        horizontalAlignment: HorizontalAlignment.Right
                        onClicked: {
                            serverStartButton.enabled = true;
                            serverStopButton.enabled = false;
                            serverSocket.closeServer();
                        }
                        enabled: false
                    }
                }
            }
        }
    }
}
