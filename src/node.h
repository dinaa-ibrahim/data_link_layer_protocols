//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __DATA_LINK_LAYER_PROTOCOLS_SIMULATION_NODE_H_
#define __DATA_LINK_LAYER_PROTOCOLS_SIMULATION_NODE_H_

#include <omnetpp.h>
#include <fstream>
#include <cmath>
#include <vector>
#include <queue>
#include <bitset>
#include "MyMessage_m.h"
using namespace std;
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
  protected:
    //std::fstream my_file;
    bool initWindow = false;
    bool firstone = true;
    //????????????????????MyMessage_Base * selfmsgforNACK;
    std::fstream output;
    std::fstream dataFile;
    string logFile = "pair";
    bool finished = false;
    bool ended = false;
    bool receivedWrong = false;
    bool nackSent = false;
    int nackSentID = -1;
    bool  recghalt = false;
    vector <bool> sentack;
    vector <bool> Recsentack;
    bool prevnack = false;
    int waitfor = -1;
    //SRP////////////////////////////////////////////////////////////////////////////////
    int RwindowSize = 0;  // initialize bakhdo mn node tnya el ha-receive menha
    int RWindowStart = 0; // b3mlo update lma ba reiceve sah
    int msg_ack = 0; // waiting to be received (bet3y el ana mstneh)
    vector<bool> received; // to mark the msgs i received to send the correct piggybacking ack

    int SwindowSize = 0;  // initialize at reading file
    int SWindowStart = 0; // b3mlo update when recieving ack
    int msg_seqno = 0; // waiting to be sent (bet3o el hab3to)
    vector<bool> sent; /// to mark sent msgs to send correct with the piggybacking ack
    //////////////////////////////////////////////////////////////////////////////////////
    //int indexWindow = 0; // sending frames   < startWindow+windowSize (msg_seqno)
    bool ackTimeout = false;
    bool sendNACK = false;
    int no_ack  = -1;
    virtual void initialize();
    virtual void intializeLogFile(string name);
    virtual void handleMessage(cMessage *msg);
    virtual void senderStart(int t,MyMessage_Base * msgg);
    virtual void receiverStart(MyMessage_Base * msgg);
    virtual string byteStuffing(string msg);
    virtual string deStuffing(string msg);
    virtual void addtoLogFile(string m);
    // phase 2
    virtual int nodeSendData(MyMessage_Base *mmsg,double reply);
    virtual int replyNACK(MyMessage_Base *mmsg,int id);
    virtual int nodeReceiveData(MyMessage_Base *mmsg);
    virtual int srSend(MyMessage_Base *mmsg);
    bool sender = false;
    bool doneOnce = false;
    vector<string> messages;
    vector <string> errors;
    vector<bool> done;// to mark that i send this msg before
    bool intializeSending = true;


    int start_time = 0;
    int num_transmissions = 0;
    int numb_correctmsgs = 0;
    //CRC Error Detection
    const string generator="1001";
    virtual string calculateCRC(string M_Payload);
};

#endif
