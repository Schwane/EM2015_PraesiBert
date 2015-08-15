/*
 * commands.hpp
 *
 *  Created on: 25.06.2015
 *      Author: Jan Zimmer
 */

#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

/*
 * Definition of all commands for each use case
 */

#define CMD_UNKNOWN "unknown_command"

/* Presentation commands */
#define CMD_SET_SLIDE "set_slide"
#define CMD_SET_PRAESENTATION "praesentation"
#define CMD_STOP_PRAESENTATION "stop_praesentation"
#define DATA_PRAESENTATION "deliver_praesentation"

/* Login commands */
#define CMD_LOGIN "login"
#define CMD_LOGIN_RESP "login_resp"

/* Authentication commands */
#define CMD_AUTH_PHASE1 "auth_phase1"
#define CMD_AUTH_PHASE2 "auth_phase2"
#define CMD_AUTH_PHASE3 "auth_phase3"
#define CMD_AUTH_PHASE4 "auth_phase4"
#define CMD_ACK_RESPONSE "ack"

/* talk reqeust commands */
#define CMD_RANF_ASK "redeanfrage_request"
#define CMD_RANF_RESP "redeanfrage_antwort"
#define CMD_RANF_RE_RESP "redeanfrage_finale_antwort"
#define CMD_RANF_FINISH "redeanfrage_finish"

/* data delivery commands */
#define DATA_AUDIO "deliver_audio"


#endif /* COMMANDS_HPP_ */
