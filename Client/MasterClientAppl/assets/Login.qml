import bb.cascades 1.4

Page {
    id: pag_login
    Container {
        layout: StackLayout {
        
        }
        Label {
            id: lab_ip
            text: "IP-Address:"
        }
        TextField {
            id: tex_ip
            text: "192.168.1.151"
            enabled: true
        }
        Label {
            id: lab_cmdp
            text: "CMD-Port:"
        }
        TextField {
            id: tex_cmdp
            text: "1337"
        } 
        Label {
            id: lab_datap
            text: "Data-Port:"
        }
        TextField {
            id: tex_datap
            text: "1338"
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
            id: but_login
            onClicked: {
                cl.setKey(tex_pwd.text);
                cl.connectToServer(tex_ip.text, tex_cmdp.text, tex_datap.text);
            }
            text: "Login"        
        } 
    }

}