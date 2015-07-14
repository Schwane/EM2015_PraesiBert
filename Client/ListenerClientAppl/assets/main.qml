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
import bb.multimedia 1.0
import bb.device 1.0

TabbedPane {
    property bool waiting: false;
    id: pan_root
    showTabsOnActionBar: true

    onCreationCompleted: {
        //OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.DisplayPortrait;
        pan_root.activeTab = tab_login;
        enablePraesitab(false);
    }
    
    Tab {
        id: tab_login
        title: "Login"
        imageSource: "asset:///img/wifi.png"
        Login {
            id: login
            
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
        ListenerClient {
            id: cl
            onSlideChanged: {
                praesi.pic_slide.image = img;
                
            }
            onSlideChangedUrl: {
                var p = url;
                if (url.indexOf("://") == - 1)
                    p = "file://" + url
                console.log("Setze Slide auf: " + p);
                praesi.pic_slide.imageSource = p;            
            }
            onLoginStateChanged: {
                var stat = cl.getLoginState();
                if (stat == "accepted")
                {
                    pan_root.activeTab = tab_praesi;
                    pan_root.vibrate();
                    login.btn_login.visible = false;
                    login.btn_logout.visible = true;                    
                }
                else if (stat == "not connected" || stat == "rejected")
                {
                    pan_root.activeTab = tab_login;
                    pan_root.vibrate();
                    login.btn_login.visible = true;
                    login.btn_logout.visible = false;   
                    enablePraesitab(false);
                }
                
                toast.body = stat;
                toast.show();
                
                //praesi.lab_loginstate_val.text = stat;
            }
            onRanfAnswer: {
                praesi.diag_ranfansw.show();
            }
            onRanfStateChanged: {
                if (state == "ACCEPTED")
                {
                        var p = "file://" + cl.getBasepath() + "/voice/"+ Qt.formatDateTime(new Date(), "yyMMdd_HH_mm_ss") + ".m4a";
                        console.log("Recording to: " + p);
                        recorder.outputUrl = p;
                        recorder.current_path = p;
                        recorder.record();
                        redLED.flash();
                        praesi.btn_praesi_stat.defaultImageSource = "asset:///img/stop.png"
                        praesi.btn_praesi_stat.pressedImageSource = "asset:///img/stop_clicked.png"
                        praesi.btn_praesi_stat.selection = true;

                }
                else if (state == "PREPARED")
                {
                    praesi.btn_do_ranf.enabled = true;
                }
                else if (state == "REJECTED")
                {
                    recorder.pause();
                    recorder.reset();
                    recorder.outputUrl = "file:///accounts/1000/shared/voice/dummy.m4a";
                    redLED.cancel();
                    toast.body = "Redenanfrage abgelehnt";
                    toast.show();
                }
                else if (state == "FINISHED")
                {
                    recorder.pause();
                    recorder.reset();
                    recorder.outputUrl = "file:///accounts/1000/shared/voice/dummy.m4a";
                    redLED.cancel();
                    cl.deliverRecording(recorder.current_path);
                    toast.body = "Redenanfrage beendet";
                    toast.show();
                }
                else if ( state == "QUEUED")
                {
                    praesi.btn_do_ranf.enabled = false;
                }
            }
            onWait: {
                waiting = active;                    
            }
            onPraesentationReady: {
                enablePraesitab(true);
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
        },
        Led {
                    id: redLED
                    color: LedColor.Red
        },
        AudioRecorder {
            id: recorder
            property string current_path : ""
            outputUrl: "file:///accounts/1000/shared/voice/recording.m4a"
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
    
    function enablePraesitab(stat)
    {
        praesi.btn_do_ranf.enabled = stat;
    }
    

}
