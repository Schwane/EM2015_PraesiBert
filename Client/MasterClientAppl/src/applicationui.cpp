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
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Image>
#include <QSettings>

#include "Message/Message.hpp"
#include "Client/Client.hpp"
#include "MasterClient/MasterClient.hpp"


using namespace bb::cascades;

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    qmlRegisterType<Client>("com.Client",1,0,"Client");
    qmlRegisterType<MasterClient>("com.Client",1,0,"MasterClient");
    qmlRegisterType<Message>("com.Client",1,0,"Message");

    Message *msg = new Message("slide","gui","cl");
    QString path( "app/native/assets/img/test.jpg");

    QImage img(path);
    QByteArray imgBytes;
    QBuffer b(&imgBytes);
    b.open(QIODevice::WriteOnly);
    img.save(&b, "JPG");
    msg -> addParameter("image", imgBytes);

    QFile xmlDoc("app/native/assets/example/msg.xml");
    xmlDoc.open(QIODevice::ReadOnly | QIODevice::Text);
    QString xmlMsg(xmlDoc.readAll());

    xmlMsg.replace("$(MSG)", imgBytes.toBase64());

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    QDeclarativePropertyMap* propertyMap = new QDeclarativePropertyMap;

    propertyMap->insert("msg", QVariant(xmlMsg));

    qml->setContextProperty("_testmsg", msg);
    qml->setContextProperty("_ptyMap", propertyMap);

    qDebug() << "Nach set context";
    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Template_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}