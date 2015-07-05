import bb.cascades 1.4

Page {
    //property alias lab_loginstate_val : lab_loginstate_val
    //property alias lab_ranf_state : lab_ranf_state
    property alias btn_accept_ranf : btn_accept_ranf
    property alias btn_mute_ranf : btn_mute_ranf
    property alias pic_slide : pic_slide
    Container {
        horizontalAlignment: HorizontalAlignment.Left
        ImageView {
            id: pic_slide
            imageSource: "asset:///img/before_start.png"
            scalingMethod: ScalingMethod.AspectFit
            enabled: true
            onTouch: {
                cl.requestSlideChange(1);
            }
        }
        
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            
            }
            Button {
                id: btn_prev
                onClicked: {
                    cl.requestSlideChange(-1);
                }
                text: "Prev"        
            }
            Button {
                id: btn_next
                onClicked: {
                    cl.requestSlideChange(1);
                }
                text: "Next"        
            }
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
            id: btn_accept_ranf
            onClicked: {
                cl.acceptRanf();
                lab_ranf_state.text = "";
            }
            text: "Redeanfragen: 0"        
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            
            }
            
            
            
            Button {
                id: btn_clear_ranf
                onClicked: {
                    cl.clearRanf();
                    lab_ranf_state.text = "";
                }
                text: "clear"        
            }
            
            Button {
                id: btn_mute_ranf
                onClicked: {
                    cl.muteRanf();
                    lab_ranf_state.text = "";
                }
                text: "mute"        
            }
        }
    
    
    }            

}