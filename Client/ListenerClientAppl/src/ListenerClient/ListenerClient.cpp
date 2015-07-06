/*
 * ListenerClient.cpp
 *
 *  Created on: 06.07.2015
 *      Author: JCB
 */

#include "ListenerClient.hpp"

ListenerClient::ListenerClient()
{
    registerdFunctions.insert(CMD_RANF_RESP,static_cast<remoteFunction>(&ListenerClient::redeanfrageResponse));
    registerdFunctions.insert(CMD_RANF_FINISH,static_cast<remoteFunction>(&ListenerClient::redeanfrageFinish));
    ranf = new Redeanfrage(id);

    connect(ranf, SIGNAL(stateChanged(QString)), this, SIGNAL(ranfStateChanged(QString)));
    connect(ranf, SIGNAL(stateChanged(QString)), this, SLOT(onRanfStateChanged(QString)));
}

ListenerClient::~ListenerClient()
{
    delete ranf;
}

Message*
ListenerClient::redeanfrageResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *msg = new Message(CMD_ACK_RESPONSE, id, "server");
    if (parameters.contains("status") && parameter_types.contains("status") && parameter_types.value("status") == "string")
    {
        if (parameters.value("status") == "ACCETPED")
        {
            ranf->accept();
            emit ranfAnswer();
        }
        else
        {
            ranf->reject();
        }
        msg->addParameter("status", QString("ok"));

    }
    else
    {
        msg->addParameter("status", QString("error"));
        msg->addParameter("message", QString("Error: status parameter not given"));
    }
    return msg;
}

Message*
ListenerClient::redeanfrageFinish(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *msg = new Message(CMD_ACK_RESPONSE, id, "server");
    msg->addParameter("status", QString("ok"));
    ranf->finish();
    return msg;
}




void
ListenerClient::onLogin()
{
    ranf -> setClientId(id);
}


void
ListenerClient::onRanfstateChanged(QString state)
{
    if (state == "REJECTED")
    {
        ranf->prepare();
    }
}


void
ListenerClient::doRanf()
{
    Message *msg = ranf->packRedeanfrage();
    xmlmw->writeMessage(msg);
}
void
ListenerClient::acceptRanf()
{
Message *msg = new Message(CMD_RANF_RE_RESP, id, "master");
msg->addParameter("status", QString("ACCEPTED"));
xmlmw->writeMessage(msg);
ranf->accept();
}
void
ListenerClient::rejectRanf()
{
Message *msg = new Message(CMD_RANF_RE_RESP, id, "master");
msg->addParameter("status", QString("REJECTED"));
xmlmw->writeMessage(msg);
ranf->reject();
}



