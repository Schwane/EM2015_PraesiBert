import bb.cascades 1.4

Page {
    //property alias lab_loginstate_val : lab_loginstate_val
    //property alias lab_ranf_state : lab_ranf_state
    property alias btn_accept_ranf : btn_accept_ranf
    property alias btn_finish_ranf: btn_finish_ranf
    property alias btn_mute_ranf : btn_mute_ranf
    property alias pic_slide : pic_slide
    property alias incoming : incoming
    onCreationCompleted: {
        incoming.play();
    }
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
            ImageToggleButton {
                id: btn_mute_ranf
                onCheckedChanged: {
                    if(!checked)
                    {
                        cl.muteRanf(true);
                    }
                    else
                    {
                        cl.muteRanf(false);
                    }
                }
                imageSourceDefault: "asset:///img/speaker_on.png"
                imageSourceChecked: "asset:///img/speaker_off.png"
                minWidth: 150.0
                minHeight: 150.0
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
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
        */
        /*
         * Label {
         * id: lab_ranf_state
         * text: "STATUS RANF"
         * 
         * }
         */

        Container {
            id: con_accept
            horizontalAlignment: HorizontalAlignment.Center
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight

            }
            animations: [
                SequentialAnimation {
                    id: incoming
                    ScaleTransition {
                        toX: 1.1
                        toY: 1.1
                        duration: 750
                        target: con_accept
                        easingCurve: StockCurve.SineInOut;
                    }
                    ScaleTransition {
                        toX: 1.0
                        toY: 1.0
                        duration: 750
                        fromX: 1.1
                        fromY: 1.1
                        target: con_accept
                        easingCurve: StockCurve.SineInOut;

                    }
                    onEnded: {
                        incoming.play();
                    }
                }
            ]
            preferredWidth: 400.0
            preferredHeight: 200.0
            maxWidth: 400.0
            maxHeight: 200.0
            minHeight: 200.0
            minWidth: 400.0
            clipContentToBounds: true
            verticalAlignment: VerticalAlignment.Top
            Button {
                id: btn_accept_ranf
                onClicked: {
                    cl.acceptRanf();
                }
                text: "Redeanfragen: 0"
                preferredWidth: 400.0
                preferredHeight: 200.0
                maxWidth: 400.0
                maxHeight: 200.0
                horizontalAlignment: HorizontalAlignment.Center
                minWidth: 400.0
                minHeight: 200.0
                appearance: ControlAppearance.Primary
                color: Color.Green
                verticalAlignment: VerticalAlignment.Center
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
            
        }
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            Button {
                id: btn_finish_ranf
                onClicked: {
                    cl.finishRanf();
                    btn_finish_ranf.visible = false;
                    btn_accept_ranf.visible = true;
                }
                visible: false
                text: "beenden"
                horizontalAlignment: HorizontalAlignment.Center
                maxWidth: 400.0
                maxHeight: 200.0
                preferredWidth: 400.0
                preferredHeight: 200.0
                verticalAlignment: VerticalAlignment.Center
            }
        }
        Container {
            layout: DockLayout {

            }
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Center
            Button {
                id: btn_clear_ranf
                onClicked: {
                    cl.clearRanf();
                    lab_ranf_state.text = "";
                }
                text: "clear"
                horizontalAlignment: HorizontalAlignment.Left
                verticalAlignment: VerticalAlignment.Bottom
                appearance: ControlAppearance.Primary

            }

        }
    }
}