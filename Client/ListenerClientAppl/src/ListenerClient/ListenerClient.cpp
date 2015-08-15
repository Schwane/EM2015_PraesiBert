/*
 * ListenerClient.cpp
 *
 *  Created on: 06.07.2015
 *      Author: JCB
 */

#include "ListenerClient.hpp"

ListenerClient::ListenerClient()
{
    //Register remote functions
    registerdFunctions.insert(CMD_RANF_RESP,static_cast<remoteFunction>(&ListenerClient::redeanfrageResponse));
    registerdFunctions.insert(CMD_RANF_FINISH,static_cast<remoteFunction>(&ListenerClient::redeanfrageFinish));

    //initiate talk-request
    ranf = new Redeanfrage(id);

    //connect signals of talk-request
    connect(ranf, SIGNAL(stateChanged(QString)), this, SIGNAL(ranfStateChanged(QString)));
    connect(ranf, SIGNAL(stateChanged(QString)), this, SLOT(onRanfStateChanged(QString)));
    connect(cs, SIGNAL(connectedToCmdServer()),this,SLOT(onLogin()));
}

ListenerClient::~ListenerClient()
{
    delete ranf;
}

Message*
ListenerClient::redeanfrageResponse(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    Message *msg = new Message(CMD_ACK_RESPONSE, id, "server");
    //check parameters
    if (parameters.contains("status") && parameter_types.contains("status") && parameter_types.value("status") == "string")
    {
        if (parameters.value("status") == "ACCEPTED")
        {
            //If accepted, signal that an answer is needed
            ranf->accept();
            emit ranfAnswer();
        }
        else
        {
            //else reject
            ranf->reject();
        }
        msg->addParameter("status", QString("ok"));

    }
    else
    {
        //on error
        msg->addParameter("status", QString("error"));
        msg->addParameter("message", QString("Error: status parameter not given"));
    }
    return msg;
}

Message*
ListenerClient::redeanfrageFinish(QMap<QString, QVariant> parameters, QMap<QString, QString> parameter_types)
{
    //On finish
    Message *msg = new Message(CMD_ACK_RESPONSE, id, "server");
    msg->addParameter("status", QString("ok"));
    ranf->finish();
    return msg;
}


void
ListenerClient::onRanfStateChanged(QString state)
{
    //Re-prepare talk.request if needed
    if (state == "REJECTED" || state == "FINISHED")
    {
        ranf->prepare();
    }
}


void
ListenerClient::doRanf()
{
    //send talk-reqeust
    ranf -> setClientId(id); //has to be set at this point again, because it could change after login
    Message *msg = ranf->packRedeanfrage();
    xmlmw->writeMessage(msg);
}
void
ListenerClient::acceptRanf()
{
    //accpet relevant talk-request
    Message *msg = new Message(CMD_RANF_RE_RESP, id, "master");
    msg->addParameter("status", QString("ACCEPTED"));
    xmlmw->writeMessage(msg);
    ranf->accept();
}
void
ListenerClient::rejectRanf()
{
    //reject, if talk request is not relevant enymore
    Message *msg = new Message(CMD_RANF_RE_RESP, id, "master");
    msg->addParameter("status", QString("REJECTED"));
    xmlmw->writeMessage(msg);
    ranf->reject();
}



