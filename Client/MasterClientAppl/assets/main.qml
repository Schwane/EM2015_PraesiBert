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
import bb.device 1.0
import bb.multimedia 1.0
import bb.cascades.pickers 1.0

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
    
    Tab {
        id: tab_settings
        title: "Einstellungen"
        Settings {
            id: settings
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
            /*
            onRanfMuteChanged: {
                if (mute)
                    praesi.btn_mute_ranf.text = "unmute";
                else 
                    praesi.btn_mute_ranf.text = "mute";
            }
            */
            onRanfSizeChanged: {
                if (size > 0)
                {
                    praesi.incoming.proceed = true;
                    praesi.incoming.play();
                    praesi.btn_accept_ranf.color = Color.Red;
                }
                else 
                {
                    praesi.incoming.proceed = false;
                    praesi.btn_accept_ranf.color = Color.Green;
                }
                praesi.btn_accept_ranf.text = "Redeanfragen: " + size;
                pan_root.vibrate();
            }
            onRanfFinalAnswer: {
                toast.body = answ;
                if (answ == "ACCEPTED")
                {
                    praesi.btn_finish_ranf.visible = true;
                    praesi.btn_accept_ranf.visible = false;
                    praesi.btn_mute_ranf.visible = false;
                    praesi.btn_clear_ranf.visible = false;
                }
                toast.show();
            }
            onWait: {
                waiting = active;                    
            }
            onPraesentationRunning:
             {
                 if (active)
                 {
                     var p = "file:///accounts/1000/shared/voice/" + Qt.formatDateTime(new Date(), "yyMMdd_HH_mm_ss") + ".m4a";
                     console.log("Recording to: " + p);
                     recorder.outputUrl = p;
                     recorder.current_path = p;
                     recorder.record();
                     redLED.flash();
                 }
                 else 
                 {
                     recorder.pause();
                     recorder.reset();
                     recorder.outputUrl = "file:///accounts/1000/shared/voice/dummy.m4a";
                     redLED.cancel();
                     cl.deliverRecording(recorder.current_path);
                 }
                 
             }
        },
        Led {
            id: redLED
            color: LedColor.Red
        },
        SystemToast {
            id: toast
        },
    
        VibrationController {
        id: vib
        },
        
        SystemDialog {
            id: diag_waiting
            title: "Bitte warten"
            body: "Es wird gewarted..."
            cancelButton.label: undefined
            confirmButton.label: undefined
        },
        AudioRecorder {
            id: recorder
            property string current_path : ""
            outputUrl: "file:///accounts/1000/shared/voice/recording.m4a"
        },
        FilePicker {
            id:filePicker
            mode: FilePickerMode.SaverMultiple
            title : "Präsentationsordner wählen"
            directories : ["/accounts/1000/shared/misc"]
            onFileSelected : {
                cl.selectPraesentation(selectedFiles);
                cl.deliverPraesentation();
                praesi.btn_praesi_stat.defaultImageSource = "asset:///img/stop.png"
                praesi.btn_praesi_stat.selection = true;
                console.log("FileSelected signal received : " + selectedFiles);
                            
            }
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
