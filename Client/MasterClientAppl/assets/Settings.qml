import bb.cascades 1.4

Page {
    Container {
        horizontalAlignment: HorizontalAlignment.Center

        verticalAlignment: VerticalAlignment.Top

        leftMargin: ui.du(25.0)
        rightMargin: ui.du(25.0)
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
                    cl.gesture(true);
                else
                    cl.gesture(false);
            }
        }

    }
}
