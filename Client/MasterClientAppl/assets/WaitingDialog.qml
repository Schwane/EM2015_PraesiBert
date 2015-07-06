import bb.cascades 1.4

Dialog {
    id: diag
    Container {
        
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        // The background is set to semi-transparent to indicate
        // that it is not possible to interact with the screen
        // behind the dialog box.
        
        background: Color.create(0.0, 0.0, 0.0, 0.5)
        
        layout: DockLayout {
        }
        
        Container {
            
            // Set the size of the dialog relative to 
            // the size of the screen that the app
            // is running on.
            
            maxHeight: ui.du(39.7)
            leftPadding: ui.du(3)
            rightPadding: leftPadding
            
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            
            layout: DockLayout {
            }
            
            background: Color.create(1, 1, 1, 1)
            
            ActivityIndicator {
                id: wait_activity
                property int clickcount: 0
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                preferredHeight: 500.0
                preferredWidth: 500
                maxWidth: 500.0
                maxHeight: 500.0
                onTouch: {
                    if (clickcount > 5)
                    {
                        diag.close();
                        clickcount = 0;
                    }
                    else 
                    {
                        clickcount += 1;
                        console.log("CC: " + clickcount);    
                    }
                        
                }
            }
        } 
    } 
    onOpened: {
        wait_activity.running = true;
    }
    
    onClosed: {
        wait_activity.running = false;
    }

}