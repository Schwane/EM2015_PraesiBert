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
            horizontalAlignment: HorizontalAlignment.Center

        }

        Divider {

        }
        /*
        Container {
            layout: GridLayout {
            
            }
            Label {
                id: lab_loginstate
                text: "Login-Status:"
                textStyle.textAlign: TextAlign.Right
            }
            Label {
                id: lab_loginstate_val
            }
        
        }
        */
        /*
        TextArea {
            id: tex_cmd
            text: "request_praesentation"
            autoSize.maxLineCount: 1
        }
        
        Button {
            id: btn_cmd
            onClicked: {
                cl.sendArbitraryCommand(tex_cmd.text);
            }
            text: "Send Command"        
        }
        /*
        Label {
            id: lab_ranf_state
            text: "STATUS RANF"
        
        }  
        */                 
        
        ImageButton {
            id: btn_do_ranf
            onClicked: {
                cl.doRanf();
            }
            horizontalAlignment: HorizontalAlignment.Center
            defaultImageSource: "asset:///img/ranf.png"
            preferredWidth: 120.0
            preferredHeight: 140.0
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
            preferredWidth: 140.0
            preferredHeight: 140.0
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
                        cl.acceptRanf();
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
                        layo.orientation = LayoutOrientation.RightToLeft;
                    else 
                        layo.orientation = LayoutOrientation.TopToBottom;
                }
                
            }
        ]
        layout: StackLayout {
            id: layo
            orientation: LayoutOrientation.TopToBottom

        }

    }   
             

}