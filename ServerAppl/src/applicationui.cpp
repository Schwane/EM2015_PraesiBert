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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/TextField>

using namespace bb::cascades;

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    // Initialize server
    server = new ServerAppl::Server();

    QObject::connect(
        server, SIGNAL(gotIpAddress(QString)),
        this, SLOT(onGotIpAddress(QString))
        );

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    root = qml->createRootObject<AbstractPane>();

    TextField * tfIpAddress = root->findChild<TextField *>("textIpAddress");
    TextField * tfCmdPort = root->findChild<TextField *>("textCmdPort");
    TextField * tfDataPort = root->findChild<TextField *>("textDataPort");

    tfIpAddress->setText(server->getIpAddress());
    tfCmdPort->setText(server->getCommandPort());
    tfDataPort->setText(server->getDataPort());

//    connect( this, SIGNAL(setIp(QString)), root->findChild<TextField *>("tex_ip"), SLOT(onButtonClicked1()));
//    connect( root->findChild<Button *>("butTriggerAction2"), SIGNAL(clicked()), this, SLOT(onButtonClicked2()));
//    connect( root->findChild<Button *>("butTriggerAction3"), SIGNAL(clicked()), this, SLOT(onButtonClicked3()));

    // Set created root object as the application scene
    Application::instance()->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("ServerAppl_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::onGotIpAddress(QString ipAddress)
{
    TextField * tfIpAddress = root->findChild<TextField *>("textIpAddress");

    tfIpAddress->setText(server->getIpAddress());
}
