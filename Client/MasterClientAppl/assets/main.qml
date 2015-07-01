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
         layout: StackLayout {

        }
        Label {
             text: "IP-Address:"
         }
         TextField {
             id: clientIPTextField
             text: "192.168.1.151"
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
        ImageView {
            id: bild
            imageSource: "asset:///img/fuchs.jpg"
        }

        TextArea {
            id: loginstate
        }
        TextArea {
            id: acmd
            text: "request_praesentation"
        }
        
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            
            }
            Button {
                id: button3
                onClicked: {
                    cl.connectToServer(clientIPTextField.text, clientCmdPortTextField.text, clientDataPortTextField.text);
                }
                text: "Login"        
            }
            
            Button {
                id: button4
                onClicked: {
                    cl.sendArbitraryCommand(acmd.text);
                }
                text: "Send cmd"        
            }
            
            
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            
            }
            Button {
                id: button5
                onClicked: {
                    cl.dummyRanf();
                }
                text: "RANF erzeugen"        
            }
            
            Button {
                id: button6
                onClicked: {
                    cl.acceptRanf();
                }
                text: "0"        
            }
            
            Button {
                id: button7
                onClicked: {
                    cl.clearRanf();
                }
                text: "clear"        
            }
            
            Button {
                id: button8
                onClicked: {
                    cl.muteRanf();
                }
                text: "mute"        
            }
        
        
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight

            }
            Button {
                id: btnPrev
                onClicked: {
                    cl.requestSlideChange(-1);
                }
                text: "Prev"        
            }
            Button {
                id: btnNext
                onClicked: {
                    cl.requestSlideChange(1);
                }
                text: "Next"        
            }
        }
        
        attachedObjects: [
            MasterClient {
                id: cl
                onSlideChanged: {
                    console.log("slide changed");
                    bild.image = cl.getSlide();
                }
                onLoginStateChanged: {
                    loginstate.text = cl.getLoginState();
                }
                onRanfMuteChanged: {
                    if (mute)
                        button8.text = "unmute";
                    else 
                        button8.text = "mute";
                }
                onRanfSizeChanged: {
                    console.log("size:" + size);
                    button6.text = "" + size;
                }
            }
        ]
    }
}
