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

#include "applicationui.hpp"
#include "UdpModule.h"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>
#include <bb/system/SystemToast>

using namespace bb::cascades;
using namespace bb::system;

ApplicationUI::ApplicationUI(Application *app):
		QObject(app),
		m_translator(new QTranslator(this)),
		m_localeHandler(new LocaleHandler(this)),
		m_invokeManager(new InvokeManager(this))
{
	// prepare the localization
	if (!QObject::connect(m_localeHandler, SIGNAL(systemLanguageChanged()),
			this, SLOT(onSystemLanguageChanged()))) {
		// This is an abnormal situation! Something went wrong!
		// Add own code to recover here
		qWarning() << "Recovering from a failed connect()";
	}

	// initial load
	onSystemLanguageChanged();

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// Make app available to the qml.
	qml->setContextProperty("_app", this);

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	// Set created root object as the application scene
	app->setScene(root);

	udp = new UdpModule(this);
    udp->listenOnPort(7146);
    connect(udp, SIGNAL(receivedData(QString)), this, SLOT(onUdpDataReceived(QString)));

	// Start HL part
	InvokeRequest request;
	request.setTarget("com.example.NotifyService");
	request.setAction("com.example.NotifyService.START");
	m_invokeManager->invoke(request);
}

void ApplicationUI::onSystemLanguageChanged() {
	QCoreApplication::instance()->removeTranslator(m_translator);
	// Initiate, load and install the application translation files.
	QString locale_string = QLocale().name();
	QString file_name = QString("Notify_%1").arg(locale_string);
	if (m_translator->load(file_name, "app/native/qm")) {
		QCoreApplication::instance()->installTranslator(m_translator);
	}
}

void ApplicationUI::setValueFor(QString key, QVariant value) {
	settings.setValue(key, value);
}

QVariant ApplicationUI::getValueFor(QString key, QVariant defaultValue) {
	// If there's no value, set it to the defaultValue
	if (settings.value(key).isNull())
		settings.setValue(key, defaultValue);

	return settings.value(key);
}

void ApplicationUI::onUdpDataReceived(QString _data) {
	qDebug() << "Received this message from Headless part :\n" << _data;

	if (_data == "NOTIFY_TRIGGER") {
		bb::system::SystemToast* pToast = new bb::system::SystemToast();
		QString message = settings.value("message", "").toString() + "\n\nThis notification will only show if the app is running.";
		pToast->setBody(message);
		pToast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
		pToast->show();
	}
}

void ApplicationUI::shutDown() {
	// This is used while coding the app to shut the headless part easily.
	// You need to shut down headless everytime you make a change in the headless
	// part or your app code. This comes handy while actively coding an headless
	// app, but you should remove the button before releasing to public.
	udp->sendMessage("NOTIFY_SHUTDOWN");
	bb::Application::instance()->quit();
}
