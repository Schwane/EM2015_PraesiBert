import bb.cascades 1.4


Page {
    //property alias lab_loginstate_val : lab_loginstate_val
    //property alias lab_ranf_state : lab_ranf_state
    property alias btn_accept_ranf : btn_accept_ranf
    property alias btn_finish_ranf: btn_finish_ranf
    property alias btn_mute_ranf : btn_mute_ranf
    property alias btn_clear_ranf : btn_clear_ranf
    property alias btn_praesi_stat : btn_praesi_stat
    property alias pic_slide : pic_slide
    property alias incoming : incoming
    paneProperties: NavigationPaneProperties {

    }
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        layout: DockLayout {

        }

        Container {
            id: top
            verticalAlignment: VerticalAlignment.Top
            horizontalAlignment: HorizontalAlignment.Center
            ImageView {
                id: pic_slide
                imageSource: "asset:///img/before_start.png"
                scalingMethod: ScalingMethod.AspectFit
                enabled: true
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Top
                loadEffect: ImageViewLoadEffect.None

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




        
        
        
        
        
        /*
         * Button {
         * text: "Record"
         * property bool r : false
         * onClicked: {
         * r = !r;
         * cl.onRunning(r);
         * /*
         * if (r)
         * recorder.record();
         * else
         * recorder.reset();
         * 
         * }
         * }
         */
        Container {
            id: bottom
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Fill
            
            Button {
                id: btn_clear_ranf
                onClicked: {
                    cl.clearRanf();
                    lab_ranf_state.text = "";
                }
                text: "Alle ablehnen"
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Top
                appearance: ControlAppearance.Primary
            
            }
            
            Container {
                id: con_accept
                horizontalAlignment: HorizontalAlignment.Center
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                
                }
                animations: [
                    SequentialAnimation {
                        property bool proceed : false;
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
                            if (proceed)
                                incoming.play();
                        }
                    }
                ]
                
                clipContentToBounds: true
                verticalAlignment: VerticalAlignment.Top
                topMargin: 40.0
                Button {
                    id: btn_accept_ranf
                    onClicked: {
                        cl.acceptRanf();
                    }
                    text: "Redeanfragen: 0"
                    horizontalAlignment: HorizontalAlignment.Center
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
                bottomMargin: 50.0
                Button {
                                id: btn_finish_ranf
                                onClicked: {
                                    cl.finishRanf();
                                    btn_finish_ranf.visible = false;
                                    btn_accept_ranf.visible = true;
                                    btn_mute_ranf.visible = true;
                                    btn_clear_ranf.visible = true;
                                }
                                visible: false
                                text: "Beenden"
                                horizontalAlignment: HorizontalAlignment.Center
                                
                                verticalAlignment: VerticalAlignment.Center
                                appearance: ControlAppearance.Primary
                                color: Color.DarkMagenta
                            
                            }
                        }
            
            Container {
                layout: DockLayout {
                
                }
                horizontalAlignment: HorizontalAlignment.Fill
                
                ImageToggleButton {
                    id: btn_mute_ranf
                    onCheckedChanged: {
                        if (! checked) {
                            cl.muteRanf(true);
                        } else {
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
            
            }
            Container {
                layout: DockLayout {
                
                }
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Top

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
                    id: btn_praesi_stat
                    property bool selection: false
                    onClicked: {
                        if (! selection) {
                            filePicker.open();
                        } else {
                            cl.requestStopPraesentation();
                            //cl.resetPraesentation();
                            selection = false;
                            btn_praesi_stat.defaultImageSource = "asset:///img/select.png";
                        }
                    
                    }
                    defaultImageSource: "asset:///img/select.png"
                    pressedImageSource: "asset:///img/select_clicked.png"
                    horizontalAlignment: HorizontalAlignment.Center
                    minWidth: 200.0
                    minHeight: 200.0
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
                    verticalAlignment: VerticalAlignment.Center
                
                }
            }
        }
        
        
    }
}