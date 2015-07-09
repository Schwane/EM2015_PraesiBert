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
import bb.system 1.0
import com.Client 1.0
import bb.vibrationController 1.0

TabbedPane {
    property bool waiting: false;
    id: pan_root
    showTabsOnActionBar: true
    
    onCreationCompleted: {
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.DisplayPortrait;
        pan_root.activeTab = tab_login;
    }
    
    Tab {
        id: tab_login
        
        title: "Login"
        imageSource: "asset:///img/wifi.png"
        Login {
            
        }

    }
    Tab {
        id: tab_praesi
        title: "Präsentation"
        imageSource: "asset:///img/screen.png"
        Presentation {
            id: praesi            
        }

    }
    
    attachedObjects: [
        MasterClient {
            id: cl
            onSlideChanged: {
                praesi.pic_slide.image = img;
                
            }
            onLoginStateChanged: {
                var stat = cl.getLoginState();
                if (stat == "accepted")
                {
                    pan_root.activeTab = tab_praesi;
                    pan_root.vibrate();
                }
                else if (stat == "not connected" || stat == "rejected")
                {
                    pan_root.activeTab = tab_login;
                    pan_root.vibrate();
                }
                
                toast.body = stat;
                toast.show();
                                    
                //praesi.lab_loginstate_val.text = stat;
            }
            onRanfMuteChanged: {
                if (mute)
                    praesi.btn_mute_ranf.text = "unmute";
                else 
                    praesi.btn_mute_ranf.text = "mute";
            }
            onRanfSizeChanged: {
                praesi.btn_accept_ranf.text = "Redeanfragen: " + size;
                pan_root.vibrate();
            }
            onRanfFinalAnswer: {
                toast.body = answ;
                if (answ == "ACCEPTED")
                    praesi.btn_finish_ranf.visible = true;
                toast.show();
            }
            onWait: {
                waiting = active;                    
            }
        },
        
        SystemToast {
            id: toast
        },
    
        VibrationController {
        id: vib
        },
        
        SystemDialog {
            id: diag_waiting
            title: "Waiting"
            body: "Waiting ..."
            cancelButton.label: undefined
            confirmButton.label: undefined
        }
    ]
    
    function vibrate() {
        //first parameter intensity, second parameter duration for vibration
        vib.start(80, 100);
    }
    onWaitingChanged: {
        if (waiting == true)
        {
            console.log("WAITING");
            diag_waiting.show();
        }
        else
        {
            console.log("NOT WAITING");
            diag_waiting.cancel();
        } 
    }
    

}
