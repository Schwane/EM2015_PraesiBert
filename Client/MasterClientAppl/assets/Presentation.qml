import bb.cascades 1.4

Page {
    //property alias lab_loginstate_val : lab_loginstate_val
    //property alias lab_ranf_state : lab_ranf_state
    property alias btn_accept_ranf : btn_accept_ranf
    property alias btn_finish_ranf: btn_finish_ranf
    property alias btn_mute_ranf : btn_mute_ranf
    property alias pic_slide : pic_slide
    Container {
        horizontalAlignment: HorizontalAlignment.Left
        verticalAlignment: VerticalAlignment.Fill
        
        ImageView {
            id: pic_slide
            imageSource: "asset:///img/before_start.png"
            scalingMethod: ScalingMethod.AspectFit
            enabled: true
            horizontalAlignment: HorizontalAlignment.Center

        }

        Container {
            layout: DockLayout {

            }
            horizontalAlignment: HorizontalAlignment.Fill
            
            ImageButton {
                id: btn_prev
                onClicked: {
                    cl.requestSlideChange(-1);
                }
                defaultImageSource: "asset:///img/previous.png"
                pressedImageSource: "asset:///img/previous_clicked.png"
                horizontalAlignment: HorizontalAlignment.Left
                minWidth: 200.0
                minHeight: 200.0

            }

            ImageButton {
                id: btn_next
                onClicked: {
                    cl.requestSlideChange(1);
                }
                defaultImageSource: "asset:///img/next.png"
                pressedImageSource: "asset:///img/next_clicked.png"
                horizontalAlignment: HorizontalAlignment.Right
                minWidth: 200.0
                minHeight: 200.0
            }
        }
        /*
         * Container {
         * layout: GridLayout {
         * 
         * }
         * Label {
         * id: lab_loginstate
         * text: "Login-Status:"
         * textStyle.textAlign: TextAlign.Right
         * }
         * Label {
         * id: lab_loginstate_val
         * }
         * 
         * }
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
        Container {

            
        }
            
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight

            }
            Button {
                id: btn_accept_ranf
                onClicked: {
                    cl.acceptRanf();
                }
                text: "Redeanfragen: 0"       
            }
            /*ImageButton {
                id: btn_accept_ranf
                onClicked: {
                    cl.acceptRanf();
                }
                horizontalAlignment: HorizontalAlignment.Left
                defaultImageSource: "asset:///img/podcast.png"
                minWidth: 150.0
                minHeight: 150.0
                pressedImageSource: "asset:///img/podcast_clicked.png"
            }
            Label {
                id:lbl_ranf_count
                text: "0"
                horizontalAlignment: HorizontalAlignment.Left
            }*/
            Button {
                id: btn_finish_ranf
                onClicked: {
                    cl.finishRanf();
                    btn_finish_ranf.visible = false;
                }
                visible: true
                text: "beenden"
                horizontalAlignment: HorizontalAlignment.Right
            }
        }
        Container {
            layout: DockLayout {

            }
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Fill
            Button {
                id: btn_clear_ranf
                onClicked: {
                    cl.clearRanf();
                    lab_ranf_state.text = "";
                }
                text: "clear"
                horizontalAlignment: HorizontalAlignment.Left
                verticalAlignment: VerticalAlignment.Bottom
            }
            ImageToggleButton {
                id: btn_mute_ranf
                onCheckedChanged: {
                    if(!checked)
                    {
                        cl.muteRanf();
                        lab_ranf_state.text = ""; 
                    }
                    else
                    {
                        cl.acceptRanf();
                    }
                }
                imageSourceDefault: "asset:///img/speaker_on.png"
                imageSourceChecked: "asset:///img/speaker_off.png"
                minWidth: 150.0
                minHeight: 150.0
                horizontalAlignment: HorizontalAlignment.Right
                verticalAlignment: VerticalAlignment.Bottom
            }
        }
    }   
}