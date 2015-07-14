/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.4

Page {
    Container {
        Label {
            // Localized text with the dynamic translation and locale updates support
            text: qsTr("PräsiBert - Server") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.BigText
        }
        
        Container {
            layout: StackLayout {
            }
            Label {
                id: lab_ip
                text: "IP-Addresse:"
            }
            TextField {
                id: tex_ip
                objectName: "textIpAddress"
                text: ""
                enabled: false
                inputMode: TextFieldInputMode.NumbersAndPunctuation
            }
            
            Label {
                id: lab_cmd_port
                text: "Command Port:"
            }
            TextField {
                id: tex_cmd_port
                objectName: "textCmdPort"
                text: ""
                enabled: false
                inputMode: TextFieldInputMode.NumbersAndPunctuation
            }
            
            Label {
                id: lab_data_port
                text: "Data Port:"
            }
            TextField {
                id: tex_data_port
                objectName: "textDataPort"
                text: ""
                enabled: false
                inputMode: TextFieldInputMode.NumbersAndPunctuation
            }
        }
    }
}
