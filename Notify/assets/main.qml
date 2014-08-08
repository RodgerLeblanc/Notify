/*
 * Copyright (c) 2013 BlackBerry Limited.
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

import bb.cascades 1.2

Page {
    titleBar: TitleBar {
        title: "Notify"
    }
    Container {
        layout: StackLayout {}
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        Container {
            leftPadding: 20
            rightPadding: 20
            Header {
                title: "Enter the message"
                bottomMargin: 10
            }
            TextField {
                id: messageTextField
                hintText: "Message to send"
                bottomMargin: 20
                text: _app.getValueFor("message", "")
                onTextChanged: {
                    _app.setValueFor("message", text)
                }
            }
            DateTimePicker {
                id: timer
                mode: DateTimePickerMode.Time
                minuteInterval: 1
                bottomMargin: 20
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                preferredWidth: 250
                value: _app.getValueFor("timePicker", "")
                onValueChanged: {
                    _app.setValueFor("timePicker", value)
                }
            }
        }
        Divider {}
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            Button {
                text: "Shut down headless"
                onClicked: {
                    _app.shutDown();
                }
            }
        }
    }
}
