import bb.cascades 1.4
import bb.system 1.0

Page {
    id: pag_praesi
    property alias diag_ranfansw : diag_ranfansw 
    property alias btn_do_ranf : btn_do_ranf
    property alias pic_slide : pic_slide
    Container {
        horizontalAlignment: HorizontalAlignment.Left
        ImageView {
            id: pic_slide
            imageSource: "asset:///img/before_start.png"
            scalingMethod: ScalingMethod.AspectFit
            enabled: true
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

        }

        Divider {

        }
                      
        
        ImageButton {
            id: btn_do_ranf
            onClicked: {
                cl.doRanf();
            }
            horizontalAlignment: HorizontalAlignment.Center
            defaultImageSource: "asset:///img/ranf.png"
            pressedImageSource: "asset:///img/ranf_clicked.png"
            minWidth: 200.0
            minHeight: 200.0
            onEnabledChanged: {
                if (btn_do_ranf.enabled == false)
                {
                    btn_do_ranf.visible = false;
                    act_ranf_active.running = true;
                    act_ranf_active.visible = true;                    
                } else {
                    btn_do_ranf.visible = true;
                    act_ranf_active.running = false;
                    act_ranf_active.visible = false;
                }
                    
            }
            verticalAlignment: VerticalAlignment.Center
        }
        ActivityIndicator {
            id: act_ranf_active
            running: false
            visible: false
            horizontalAlignment: HorizontalAlignment.Center
            minWidth: 200
            minHeight: 200
            verticalAlignment: VerticalAlignment.Center

        }
        
        
        attachedObjects: [
            SystemDialog {
                id: diag_ranfansw
                title: "Redeanfrage"
                body: "Redeanfrage noch aktuell?"
                cancelButton.label: "Nein"
                confirmButton.label: "Ja"
                onFinished: {
                    if (diag_ranfansw.result != SystemUiResult.CancelButtonSelection) {
                        cl.acceptRanf(); //start audio recording
                        var p = "file://" + cl.getBasepath() + "voice/"+ Qt.formatDateTime(new Date(), "yyMMdd_HH_mm_ss") + ".m4a";
                        console.log("Recording to: " + p);
                        recorder.outputUrl = p;
                        recorder.current_path = p;
                        recorder.record();
                        redLED.flash();
                    }
                    else 
                    {
                        cl.rejectRanf();
                    }
                }
            },
            OrientationHandler {
                id: or_handler
                // Call update function to set new orientation
                onOrientationAboutToChange: {
                    if (orientation == UIOrientation.Landscape)
                    {
                        layo.orientation = LayoutOrientation.RightToLeft;
                        pan_root.showTabsOnActionBar = false;
                    }
                    else 
                    {
                        layo.orientation = LayoutOrientation.TopToBottom;
                        pan_root.showTabsOnActionBar = true;
                    }
                        
                }
                
            }
        ]
        layout: StackLayout {
            id: layo
        }

    }   
             

}