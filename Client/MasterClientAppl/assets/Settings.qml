import bb.cascades 1.4

Page {
    Container {
        horizontalAlignment: HorizontalAlignment.Center

        verticalAlignment: VerticalAlignment.Top
        maxWidth: ui.du(50.0)
        layout: StackLayout {

        }
        clipContentToBounds: true
        
        CheckBox {
            text: "HDMI Ausgabe"
            onCheckedChanged: {
                if (checked)
                    cl.hdmi(true);
                else
                    cl.hdmi(false);
            }
        }
        CheckBox {
            text: "Gestensteuerung"
            onCheckedChanged: {
                if (checked)
                {
                    cl.activateGesture(true);
                    vib.checked = false;   
                }
                else
                    cl.activateGesture(false);
            }
        }
        CheckBox {
            id: vib
            text: "Vibration"
            onCheckedChanged: {
                if (checked)
                    vibration_time = 100;
                else
                    vibration_time = 0;
            }
            checked: true
        }

    }
}
