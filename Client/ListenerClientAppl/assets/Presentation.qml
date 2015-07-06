import bb.cascades 1.4
import bb.system 1.0

Page {
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
        
        Button {
            id: btn_do_ranf
            onClicked: {
                cl.acceptRanf();
                lab_ranf_state.text = "";
            }
            text: "Redeanfragen"        
        }
        attachedObjects: [
            SystemDialog {
                id: diag_ranfansw
                title: "Redeanfrage"
                body: "Redeanfrage noch aktuell?"
                cancelButton.label: "Nein"
                confirmButton.label: "Ja"
                onFinished: {
                    if (diag_ranfansw != SystemUiResult.CancelButtonSelection) {
                        cl.acceptRanf();
                    }
                    else 
                    {
                        cl.rejectRanf();
                    }
                }
            }
        ]        
    
    }   
             

}