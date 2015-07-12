import bb.cascades 1.4

Page {
    id: pag_login
    property alias btn_login: btn_login
    property alias btn_logout: btn_logout
    ScrollView {
        Container {
            
            Container {
                layout: StackLayout {
                
                }
                Label {
                    id: lab_ip
                    text: "IP-Addresse:"
                }
                TextField {
                    id: tex_ip
                    text: "192.168.1.151"
                    enabled: true
                    inputMode: TextFieldInputMode.NumbersAndPunctuation
                }
                Label {
                    id: lab_cmdp
                    text: "CMD-Port:"
                }
                TextField {
                    id: tex_cmdp
                    text: "1337"
                    inputMode: TextFieldInputMode.NumbersAndPunctuation
                } 
                Label {
                    id: lab_datap
                    text: "Data-Port:"
                }
                TextField {
                    id: tex_datap
                    text: "1338"
                    inputMode: TextFieldInputMode.NumbersAndPunctuation
                } 
            
            
            
            
            
            }
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Top
                topPadding: 40.0
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                
                }
                Label {
                    id: lab_pwd
                    text: "Passwort:"
                }
                TextField {
                    id: tex_pwd
                    text: ""
                    inputMode: TextFieldInputMode.Password
                } 
                Button {
                    id: btn_login
                    onClicked: {
                        cl.setKey(tex_pwd.text);
                        cl.connectToServer(tex_ip.text, tex_cmdp.text, tex_datap.text);
                    }
                    text: "Login"
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    appearance: ControlAppearance.Primary
                } 
                
                Button {
                    id: btn_logout
                    onClicked: {
                        cl.logout();
                    }
                    text: "Logout"
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    appearance: ControlAppearance.Primary
                    color: Color.Red
                    visible: false
                } 
            }
        }
    }

}