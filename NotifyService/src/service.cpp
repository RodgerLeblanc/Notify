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


#include "service.hpp"
#include "UdpModule.h"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>

using namespace bb::platform;
using namespace bb::system;

Service::Service(bb::Application * app)	:
		QObject(app),
		m_notify(new Notification(this)),
		m_invokeManager(new InvokeManager(this))
{
	NotificationDefaultApplicationSettings settings;
	settings.setPreview(NotificationPriorityPolicy::Allow);
	settings.apply();

	m_invokeManager->connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

	// Start a timer with 1 sec interval
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer->start(1000);

	alreadyNotified = false;

	// This will be used to communicate between Headless and UI part
	// There's 2 way to communicate, UDP or through a file with a QFileSystemWatcher on both ends.
	// I prefer UDP, but both methods works.
	udp = new UdpModule(this);
    udp->listenOnPort(7145);
    connect(udp, SIGNAL(receivedData(QString)), this, SLOT(onUdpDataReceived(QString)));
}

void Service::handleInvoke(const bb::system::InvokeRequest & request) {
	if (request.action().compare("com.example.NotifyService.RESET") == 0) {
		// This is an example how you handle invocation from UI part. You can use invocation
		// to determine if the headless part was started because the user clicked on
		// the app icon or if it was started after a system restart.
		// Invocation can't be used as a 2-way instant communication as the headless part
		// can't invoke directly, it has to send an invoke request to the hub and then
		// the user have to click on it and select "Open".

		// You'll most likely not touch this part for now
	}
}

void Service::notify() {
	// Clear previous hub notifications
	Notification::clearEffectsForAll();
	Notification::deleteAllFromInbox();

	// Sets the message
	m_notify->setTitle("Notify app");
	QString message = settings.value("message", "").toString() + "\n\nThis notification will hit the hub no matter if the app is running as an Active Frame or not.";
	m_notify->setBody(message);

	// Sets the target that will be trigger if user click "Open" in the bottom of
	// our hub notification message
	bb::system::InvokeRequest request;
	request.setTarget("com.example.Notify");
	request.setAction("bb.action.START");
	m_notify->setInvokeRequest(request);

	// Send the notification to hub
	m_notify->notify();

	// Sends a message to UI part asking it to trigger a UI notification too
	udp->sendMessage("NOTIFY_TRIGGER");
}

void Service::onTimer() {
	// Make sure settings are sync with UI part
	settings.sync();

	// Checks for a match every second
	QString timeFormat = "hh:mm";

	if (settings.value("timePicker", "").toTime().toString(timeFormat) == QTime::currentTime().toString(timeFormat)) {
		if (!alreadyNotified) {
			// Only notify once in the minute
			notify();
			alreadyNotified = true;
		}
	}
	else {
		alreadyNotified = false;
	}
}

void Service::onUdpDataReceived(QString _data) {
	qDebug() << "Received this message from UI part :\n" << _data;

	if (_data == "NOTIFY_SHUTDOWN") {
		bb::Application::instance()->quit();
	}
}
