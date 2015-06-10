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
import com.Client 1.0

Page {
    Container {
        ImageView {
            id: bild
            imageSource: "asset:///img/fuchs.jpg"
        }

        Label {
            id: label
            // Localized text with the dynamic translation and locale updates support
            text: qsTr("Hello World") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.BigText
        }

        TextArea {
            id: ta
        }
        TextArea {
            id: loginstate
        }
        Button {
            id: button
            onClicked: {
                console.log("Before Invoke");
                //cl.invokeRemote(_testmsg, false);
                ta.text = _ptyMap.msg;
                cl.simulateSocketReceive(_ptyMap.msg);
            }
            text: "Do It!!"

        }
        Button {
            id: button2
            onClicked: {
                console.log("Before Invoke");
                //cl.invokeRemote(_testmsg, false);
                cl.requestImage();
            }
            text: "Remote!"        
        }
        Button {
            id: button3
            onClicked: {
                console.log("Before Invoke");
                //cl.invokeRemote(_testmsg, false);
                //cl.connectToServer("192.168.1.144", "2000");
                cl.connectToServer("192.168.0.101", "2000");
            }
            text: "Login"        
        }
        attachedObjects: [
            MasterClient {
                id: cl
                onSlideChanged: {
                    bild.image = cl.getSlide();
                }
                onMessageSent: {
                    ta.text = cl.getLastSentMsg();
                }
                onLoginStateChanged: {
                    loginstate.text = cl.getLoginState();
                }
            }
        ]
    }
    onCreationCompleted: {
        console.log("Creation complete");
    }
}
