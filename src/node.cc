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

#include "node.h"
#include "MyMessage_m.h"
#include <stdlib.h> /* atoi */
#include <iostream>
#include <ostream>
#include <vector>
#include <cmath>
using namespace std;
Define_Module(Node);
int index = 0;
double propdelay = 0.2;
int CRC = 0;



// dup by 0.01
// delay by 0.01
//send msg in same frame by 0.05
//time out 0.1
// delete msg ???????????
char BinaryStr(const char* str) {
    char encod = 0;
    for (int i = 0; i < 8; i++) {
        if (str[i] == '1') {
            encod |= 1 << (7 - i);
        }
    }
    return encod;
}

vector<int> hammingGeneration(string Strbits )
{
    string hamStr="";
    int count=Strbits.length();

    int offset = 0;

    while (count + offset + 1 > pow(2, offset))
    {
        offset++;
    }
    int range=count+offset;
    vector<int> hamarray(range,0);
    //set parity bits to -1
    for (int i = 0; i < offset; i++)
        hamarray[pow(2, i) - 1] = -1;

    int j = count-1;
    //reverserly from LSB to MSB
    for (int i = 0; i < range; i++) {
        if (hamarray[i] != -1) {
            if (Strbits[j]=='1')
                hamarray[i] =1 ;
            else
                hamarray[i] =0 ;
            j--;
        }
    }
    /*
    for(int i=hamarray.size()-1;i>=0;i--)
    {
        cout<<hamarray[i]<<" ";
    }*/
    j=0;
    for (int i = 0; i < range; i++)
    {
    //only get parity for -1 values in array
    int c=0;
    //get offset2 of P
    int offset2 = log2(i+1);//1 2
        if (hamarray[i] == -1)
      {
        //3 5
        for (int j = i + 2;j <= range; ++j)
        {
            int dummy=(1 << offset2);

            if ((j & dummy ))
            {
                //cout<<"E"<<j<<" "<<hamarray[j-1]<<" :"<<(j & dummy)<<endl;
                //cout<<"E"<<j<<" "<<hamarray[j-1]<<" :"<<i<<endl;
                if (hamarray[j-1] == 1) {

                    c++;
                }
            }
        }

        if (c % 2 == 1)
            hamarray[i] = 1;
        else
            hamarray[i] = 0;


      }
    }
    return hamarray;
}


void hammingCorrection(vector<int>&a,int &errorIndex,int Strbits,vector<int>parrity)
{
    int p=0;
        int pindex=0;
        int count=Strbits;
        int offset=0;
        vector<int>parrityarr;
        while (count + offset + 1 > pow(2, offset))
        {
            offset++;
        }
        int range=count+offset;
        int j=0;
        for (int i = 0; i < range; i++)
        {
        //only get parity for -1 values in array
        int c=0;
        //get offset2 of P
        if (ceil(log2(i+1)) == floor(log2(i+1)))
            {
                c=parrity[i];
                a[i]=-1;
            }
        int offset2 = log2(i+1);//1 2
            if (a[i] == -1)
          {
            //3 5
            for (int j = i + 2;j <= range; ++j)
            {
                int dummy=(1 << offset2);

                if ((j & dummy ))
                {
                    //cout<<"E"<<j<<" "<<hamarray[j-1]<<" :"<<(j & dummy)<<endl;
                    //cout<<"E"<<j<<" "<<hamarray[j-1]<<" :"<<i<<endl;
                    if (a[j-1] == 1) {

                        c++;
                    }
                }
            }

            if (c % 2 == 1)
                parrityarr.push_back(1);
            else
                parrityarr.push_back(0);

            }
        }
        bool flag=true;
        for (int i = 0; i < parrityarr.size(); i++)
        { //cout<<parrityarr[i]<<endl;
            errorIndex+=(parrityarr[i]*pow(2,i));
            if(parrityarr[i]==1)
                flag=false;
        }
        if(errorIndex==0 && flag)
            errorIndex=-1;

}
void Node::initialize()
{
    // TODO - Generated method body
//    std::ofstream ofs;
//    ofs.open("../outputs/pair01.txt", std::ofstream::out | std::ofstream::trunc);
//    ofs.close();
//    std::ofstream ofs;
//    ofs.open("../outputs/pair23.txt", std::ofstream::out | std::ofstream::trunc);
//    ofs.close();
//    std::ofstream ofs;
//    ofs.open("../outputs/pair45.txt", std::ofstream::out | std::ofstream::trunc);
//    ofs.close();
    for (int i = 0;i<100;i++)
    {
        received.push_back(false);
        sent.push_back(false);
    }
}
void Node::intializeLogFile(string nodeName)
{
    if (nodeName == "node0" ||nodeName == "node1" )
    {
        logFile = "pair01.txt";
    }
    if (nodeName == "node2" ||nodeName == "node3" )
    {
        logFile = "pair23.txt";
    }
    if (nodeName == "node4" ||nodeName == "node5" )
    {
        logFile = "pair45.txt";
    }
    ofstream f;
    f.open ("../outputs/"+logFile,std::ios_base::app);
    f.close();
}
void Node::addtoLogFile(string msg)
{
    ofstream f;
    f.open ("../outputs/"+logFile,std::ios_base::app);
    f << msg+"\n";
    f.close();
}
string Node:: byteStuffing (string msg_input)
{
     string final_payload = "$" ; // first flag byte
     int msg_size = msg_input.size();
     for(int i = 0;i<msg_size;i++)
     {
         if(msg_input[i] == '$' || msg_input[i] == '/') // if in msg esc or flag
         {
             final_payload = final_payload + '/' + msg_input[i] ; // add esc before it
         }
         else
             final_payload = final_payload +  msg_input[i] ;
     }
     final_payload = final_payload + '$'; // last flag byte
     cout << "Node will send message after byte stuffing : " << final_payload <<endl;
     return final_payload;
}

string Node:: deStuffing(string mymsg)
{
    string final_msg = "";
    for (int i = 1;i<mymsg.size();i++)
        {
           if(mymsg[i] == '/')
           {
               final_msg = final_msg + mymsg[i+1];
               i=i+1;
           }
           else if (mymsg[i] == '$')
               break;
           else
               final_msg = final_msg + mymsg[i];
        }
    return final_msg;
}
string XORString(string a, string b)
{
    string str="";
    for(int i=0;i<a.length();i++)
    {
        if(a[i]==b[i])
            str+="0";
        else
            str+="1";
    }
    return str;
}
/*
Main Concept of shifting bits and algorithm of CRC is got from : http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
*/
string Node::calculateCRC(string binaryStr)
{

                int j=0;
                string dividend="";
                string str="";
                bool first=true;
                while( j<(binaryStr.length()))
                {
                    //Ensure that MSB is 1
                    while(binaryStr[j]=='0' && str=="" && first)
                    {
                        j++;
                        first=false;
                    }
                       // >0?str.length()-1:0;
                    for(int z=(str.length());z<generator.length()&& j<binaryStr.length();z++)
                    {
                        str+=binaryStr[j];
                        j++;
                    }
                  //cout<<"jjjjjj"<<j<<"   "<<str<<endl;
                    if(str.length()<4)
                        break;
                    dividend=XORString(str,generator);
                    //cout<<dividend<<endl;
                    //int num=
                    str="";
                    str=stoi(dividend)>0?to_string(stoi(dividend)):"";
                     //cout<<j<<endl;

                }
                //cout<<j<<endl;
               // while(j<binaryStr.length())
                 //   str+=binaryStr[j++];
                if(str=="")
                    str="0";

                return str;
}
void Node::receiverStart(MyMessage_Base * mmsg)
{
    const char* str=getName();
    std::string s = str;
    intializeLogFile(s);
    string m4 = s + " : received msg from coordinator...";
    EV << m4 << endl;
    addtoLogFile(m4);
    sender = false;

    EV <<"Receiver start reading from file.." << endl;
                    string m3 = s + " : Receiver to reading data from file";
                    EV << m3 <<endl;
                    addtoLogFile(m3);
                    string file_name = "../inputsSamples/";
                    cout << "File name from msg: " << mmsg->getM_Payload() <<endl;
                    file_name = file_name + mmsg->getM_Payload();
                    cout << "File name: " << file_name <<endl;
                    //read file
                    dataFile.open(file_name, std::ios::in);
                    if (!dataFile)
                        ended = true;
                    else
                        ended = false;
                     string line;
                     // read all meassages
                     int max = 0;
                     while (getline(dataFile, line))
                     {
                         max ++;
                         cout << "Node start read new line" <<endl;
                         vector<string> inputs_arr;
                         string error_input = line.substr(0,4);
                         errors.push_back(error_input);
                         cout << "error is : " << error_input <<endl;
                         string msg_input = line.substr(5,std::string::npos);
                         cout << " msg is : " << msg_input <<endl;
                         messages.push_back(msg_input);
                     }
                     // LOG FILE

                     const char* str1=getName();
                     std::string s1 = str1;
                     string ww = s1 + " : finished reading from file...";
                     EV << ww << endl;
                     addtoLogFile(ww);
                     cout << "Receiver node finish reading from file" <<endl;
                     SwindowSize = 4;//ceil((max) / 2);
                     string op = "number of msgs = " + to_string(max) + " and window size = " + to_string(SwindowSize)+"..";
                     addtoLogFile(op);
}
void Node::senderStart(int start,MyMessage_Base * mmsg)
{
    const char* str=getName();
                std::string s = str;
                sender = true;
                intializeLogFile(s);
                string h = s + " : received msg from coordinator";
                addtoLogFile(h);
                // LOG FILE
                EV <<"Sender start reading from file.." << endl;
                string m3 = s + " : start to reading data from file";
                EV << m3 <<endl;
                addtoLogFile(m3);
                string file_name = "../inputsSamples/";
                cout << "File name from msg: " << mmsg->getM_Payload() <<endl;
                file_name = file_name + mmsg->getM_Payload();
                cout << "File name: " << file_name <<endl;
                //read file
                dataFile.open(file_name, std::ios::in);
                if (!dataFile)
                    ended = true;
                else
                    ended = false;
                 string line;
                 // read all meassages
                 while (getline(dataFile, line))
                 {
                     cout << "Node start read new line" <<endl;
                     vector<string> inputs_arr;
                     string error_input = line.substr(0,4);
                     errors.push_back(error_input);
                     cout << "error is : " << error_input <<endl;
                     string msg_input = line.substr(5,std::string::npos);
                     cout << " msg is : " << msg_input <<endl;
                     messages.push_back(msg_input);
                 }
                 // LOG FILE
                 SwindowSize = 4;//ceil(( messages.size() ) /2);
                 const char* str1=getName();
                 std::string s1 = str1;
                 string m4 = s1 + " : finished reading from file...";
                 EV << m4 << endl;
                 addtoLogFile(m4);
                 m4 = "number of msgs = " + to_string(messages.size()) + " and window size = " + to_string(SwindowSize) + "..";
                 addtoLogFile(m4);
                 cout << "Sender node finish reading from file" <<endl;
                 // Self message with start time
                 MyMessage_Base * msg_s = new MyMessage_Base();
                 msg_s->setSeq_Num(start);
                 msg_s->setM_Type(6);
                 EV <<"Sender schedule self message" << endl;
                 scheduleAt(start, msg_s);
                 start_time = start;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int Node:: replyNACK(MyMessage_Base *mmsg,int id) // if i received NACK
{

    if ( id > messages.size()-1 )
    {
        // LOG FILE
        double t = simTime().dbl();
         const char* str=getName();
         std::string s = str;
        if (doneOnce == false)
        {
            cout << "Sender finish sending messages.." <<endl;
            string s0= s +  " : finish sending frames at time = " + to_string(t);
            EV << s0 <<endl;
            addtoLogFile(s0);
            doneOnce = true;
        }
        if(mmsg->getM_Type()==55)
        {
         //both finished
            mmsg->setM_Type(55);
            cout << "Both nodes finished..session finished.." <<endl;
            double t = simTime().dbl();
            string s0= "Both nodes finished..session finished.. at time = " + to_string(t);
            EV << s0 <<endl;
            addtoLogFile(s0);
            return 1;
        }
            mmsg->setM_Type(55);
            cout << "okkk ana khlst?" << endl;
            index++;
            sendDelayed(mmsg,0.05*index+propdelay,"out");//send(msgg,"out");

            return 1;
    }

    if(recghalt)
    {

        nackSent = true;
        nackSentID = msg_ack;
        recghalt= false;
    }
    double t = simTime().dbl();
    const char* str=getName();
    std::string s = str;

    string h = s + " S khara on" + to_string(id);
    cout << h <<endl;
    double reply=0.05*index;
    index++;
    int nodeack=mmsg->getpiggybackingID();

    double delayy=par("delay").doubleValue();
    MyMessage_Base * msgg = new MyMessage_Base(); // new msg to be sent
    msgg->setpiggybackingID(msg_ack); //mmsg->getpiggybackingID()
    msgg->setackORnack(mmsg->getackORnack());
    msgg->setM_Type(1);
    if(firstone)
    {
        msgg->setWS(SwindowSize);
        firstone = false;
    }
    string error_msg = "";
    string error_msg_type = "";
    error_msg_type = error_msg_type + "RECEIVED NACKK";
    //start sending new frame
    int index_msg = id; //msg i am going to send
    msgg->setSeq_Num(index_msg);
    //byte stuffing
    cout << s + "ekhs? 2  " + to_string(index_msg)+" aaa"<<endl;
    msgg->setM_Payload(byteStuffing(messages[index_msg]).c_str());
    string Payload=msgg->getM_Payload();
    string binaryStr="";
                    for (int i = 0; i < Payload.length(); i++)
                    {
                            bitset<8> binaryBits(Payload[i]);
                            binaryStr+=binaryBits.to_string();
                    }
                    cout<<binaryStr <<"     "<<binaryStr.length()<<endl;
            string CRCStr=calculateCRC(binaryStr);
            cout<<"CRC Bits: "<<CRCStr<<endl;
            EV<<"CRC Bits: "<<CRCStr<<endl;
            bits CRCbits(CRCStr);
            msgg->setMycheckbits(CRCbits);
    // Errors
    string curr_error = errors[index_msg];
    int out = -1;
    // curr_error = 1101
    char mod_error = curr_error[0];
    char loss_error = curr_error[1];//out = 0 -> loss
    char dup_error = curr_error[2];// out = 2 -> dupw
    char delay_error = curr_error[3];// out = 1 -> delayed
    /// out = 3 -> dup w delay
//    if(loss_error == '1') //skip this message
//    {
//        double t = simTime().dbl();
//        const char* str=getName();
//        std::string s = str;
//        error_msg = s + " : will drop message with id = " +  to_string(index_msg) +
//                " and content = " +  msgg->getM_Payload() +
//                  "and piggy backing ACK ="+  to_string(msg_ack) +" at time = " +  to_string(t);
//        cout << "loss msg" <<endl;
//        addtoLogFile(error_msg);
//        out = 0;
//        //schedule new event to complete (Self message )
//        msgg->setM_Type(4);
//        cout << s +"HAB3tttttttttttt loss" <<endl;
//        scheduleAt(simTime()+0.2, msgg);
//        sent[index_msg] = true;
//        msg_seqno++;
//        num_transmissions++;
//        return;
//    }
//    if(mod_error == '1' )
//    {
//         cout << "modi msg " << endl;
//         error_msg_type = error_msg_type + " Modification ";
//         int index=uniform(0,1)*10;
//         const char* str = msgg->getM_Payload();
//         string s = str;
//         int size = s.size();
//         while (index > size-1)
//         {
//             index=uniform(0,1)*10;
//         }
//         string mypayload= msgg->getM_Payload();
//         mypayload[index]=mypayload[index]+2;
//         msgg->setM_Payload(mypayload.c_str());
//         cout << "msg after modi " << msgg->getM_Payload() <<endl;
//         sent[index_msg] = true;
//    }
//    if(dup_error == '1')
//    {
//        cout << "dup msg " << endl;
//        error_msg_type = error_msg_type + " with Duplication ";
//        MyMessage_Base * msg_s = new MyMessage_Base();
//        msg_s->setSeq_Num(msgg->getSeq_Num());
//        msg_s->setM_Type(1);
//        msg_s->setM_Payload( msgg->getM_Payload());
//        msg_s->setpiggybackingID( msgg->getpiggybackingID());
//        msg_s->setMycheckbits(msgg->getMycheckbits());
//        msg_s->setackORnack(msgg->getackORnack());
//       EV << " AHUU H3ML SCHEDULE to send dupp att "  << simTime() + 0.01  << endl;
//       num_transmissions++;
//       double time = reply+0.01;
//       out = 2;
//       sendDelayed(msg_s,time,"out");
//       num_transmissions++;
//       sent[index_msg] = true;
//    }
//    if(delay_error == '1' )
//    {
//          cout << "delayed msg " << endl;
//          error_msg_type = error_msg_type + " with delay ";
//          //addtoLogFile(error_msg);
//          MyMessage_Base * msg_s = new MyMessage_Base();
//          msg_s->setSeq_Num(msgg->getSeq_Num());
//          msg_s->setM_Type(msgg->getM_Type());
//          msg_s->setM_Payload( msgg->getM_Payload());
//          msg_s->setpiggybackingID( msgg->getpiggybackingID());
//          msg_s->setMycheckbits(msgg->getMycheckbits());
//          msg_s->setackORnack(msgg->getackORnack());
//          error_msg = s + " : will send msg with id = "+  to_string(index_msg) +
//                  " and content = " +msgg->getM_Payload()  +error_msg_type +
//                  "and piggy backing ACK = "+  to_string(msg_ack) +" at time = " + to_string(t) ;
//          addtoLogFile(error_msg);
//          cout <<  s +"HAB3tttttttttttt delay" <<endl;
//          sendDelayed(msg_s,reply+delayy,"out"); // delay in second from ini file
//          num_transmissions++;
//          out = 1;
//          msg_seqno++;
//          sent[index_msg] = true;
//          return;
//    }
    // not lost and not delayed
    error_msg = s + " : will send msg with id = "+  to_string(index_msg) +
            " and content = " +msgg->getM_Payload()  +error_msg_type +
            "and piggy backing ACK = "+  to_string(msg_ack) +" at time = " + to_string(t) ;
    addtoLogFile(error_msg);
    cout << s +"HAB3tttttttttttt 3ady" <<endl;
    sendDelayed(msgg,0.05+propdelay,"out");//send(msgg,"out"); // dup w delay m3 b3d???
    num_transmissions++;
    ///////////////////msg_seqno++;
    sent[index_msg] = true;
   return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int Node::nodeSendData(MyMessage_Base *mmsg,double r) //print my ack
{

    double reply=0.05*index;
    index++;
    double t = simTime().dbl();
    const char* str=getName();
    string mess = "";
    std::string s = str;
    double delayy=par("delay").doubleValue();
    //double CRC=par("delay").doubleValue();
    MyMessage_Base * msgg = new MyMessage_Base(); // new msg to be sent
    msgg->setpiggybackingID(msg_ack); //mmsg->getpiggybackingID()
    msgg->setackORnack(mmsg->getackORnack());
    msgg->setM_Type(1);
    if(firstone)
    {
        msgg->setWS(SwindowSize);
        firstone = false;
    }
    string error_msg = "";
    string error_msg_type = "";
    //start sending new frame
    int index_msg = msg_seqno; //msg i am going to send
    msgg->setSeq_Num(index_msg);
    //byte stuffing
    cout << s + "ekhs? 2  " + to_string(index_msg)+" aaa"<<endl;
    msgg->setM_Payload(byteStuffing(messages[index_msg]).c_str());
    if ( CRC == 1)
    {
        string Payload=msgg->getM_Payload();
        string binaryStr="";
                        for (int i = 0; i < Payload.length(); i++)
                        {
                                bitset<8> binaryBits(Payload[i]);
                                binaryStr+=binaryBits.to_string();
                        }
                        cout<<binaryStr <<"     "<<binaryStr.length()<<endl;
                string CRCStr=calculateCRC(binaryStr);
                cout<<"CRC Bits: "<<CRCStr<<endl;
                EV<<"CRC Bits: "<<CRCStr<<endl;
                bits CRCbits(CRCStr);
                msgg->setMycheckbits(CRCbits);
    }
    else
    {
        //mess = s + " use hamming ";
        string Payload=messages[index_msg];
          string binaryStr="";
                          for (int i = 0; i < Payload.length(); i++)
                          {
                                  bitset<8> binaryBits(Payload[i]);
                                  binaryStr+=binaryBits.to_string();
                          }
                          cout<<binaryStr <<"     "<<binaryStr.length()<<endl;
        vector<int> out_hamm = hammingGeneration(binaryStr);
        for(int j=0;j<out_hamm.size();j++)
        {
            msgg->hamm.push_back(out_hamm[j]);
        }
    }
    // Errors
    string curr_error = errors[index_msg];
    int out = -1;
    bool dupp = false;
    int delayed = 0;
    // curr_error = 1101
    char mod_error = curr_error[0];
    char loss_error = curr_error[1];//out = 0 -> loss
    char dup_error = curr_error[2];// out = 2 -> dupw
    char delay_error = curr_error[3];// out = 1 -> delayed
    /// out = 3 -> dup w delay
    if(loss_error == '1') //skip this message
    {
        double t = simTime().dbl();
        const char* str=getName();
        std::string s = str;
        error_msg = s + " : will drop message with id = " +  to_string(index_msg) +
                " and content = " +  msgg->getM_Payload() +
                  "and piggy backing ACK ="+  to_string(msg_ack) +" at time = " +  to_string(t);
        cout << "loss msg" <<endl;
        addtoLogFile(error_msg);
        out = 0;
        //schedule new event to complete (Self message )
        msgg->setM_Type(4);
        cout << s +"HAB3tttttttttttt loss" <<endl;
        //scheduleAt(simTime()+0.2, msgg);
        //AREEJ
        sent[index_msg] = true;
        msg_seqno++;
        num_transmissions++;
        return 1;
    }
    if(mod_error == '1' )
    {
         cout << "modi msg " << endl;
         error_msg_type = error_msg_type + " Modification ";
         int index=uniform(0,1)*10;
         const char* str = msgg->getM_Payload();
         string s = str;
         int size = s.size();
         while (index > size-1)
         {
             index=uniform(0,1)*10;
         }
         string mypayload= msgg->getM_Payload();
         mypayload[index]=mypayload[index]+2;
         msgg->setM_Payload(mypayload.c_str());
         cout << "msg after modi " << msgg->getM_Payload() <<endl;
         sent[index_msg] = true;
    }
    if(dup_error == '1')
    {
        cout << "dup msg " << endl;
        error_msg_type = error_msg_type + " with Duplication ";
        MyMessage_Base * msg_s = new MyMessage_Base();
        msg_s->setSeq_Num(msgg->getSeq_Num());
        msg_s->setM_Type(1);
        msg_s->setM_Payload( msgg->getM_Payload());
        msg_s->setpiggybackingID( msgg->getpiggybackingID());
        msg_s->setMycheckbits(msgg->getMycheckbits());
        msg_s->setackORnack(msgg->getackORnack());
       EV << " AHUU H3ML SCHEDULE to send dupp att "  << simTime() + 0.01  << endl;
       num_transmissions++;
       double time = reply+0.01;
       out = 2;
        dupp = true;
       //sendDelayed(msg_s,time+propdelay,"out");
       num_transmissions++;
       sent[index_msg] = true;
    }
    if(delay_error == '1' )
    {
          cout << "delayed msg " << endl;
          error_msg_type = error_msg_type + " with delay ";
          //addtoLogFile(error_msg);
          MyMessage_Base * msg_s = new MyMessage_Base();
          msg_s->setSeq_Num(msgg->getSeq_Num());
          msg_s->setM_Type(msgg->getM_Type());
          msg_s->setM_Payload( msgg->getM_Payload());
          msg_s->setpiggybackingID( msgg->getpiggybackingID());
          msg_s->setMycheckbits(msgg->getMycheckbits());
          msg_s->setackORnack(msgg->getackORnack());
          error_msg = s + " : will send msg with id = "+  to_string(index_msg) +
                  " and content = " +msgg->getM_Payload()  +error_msg_type +
                  "and piggy backing ACK = "+  to_string(msg_ack) +" at time = " + to_string(t) ;
          //addtoLogFile(error_msg);
          cout <<  s +"HAB3tttttttttttt delay" <<endl;
          cout << "wdelay?" <<endl;
          //sendDelayed(msg_s,reply+ 3 +propdelay,"out"); // delay in second from ini file
          num_transmissions++;
          out = 1;
//          msg_seqno++;
//          sent[index_msg] = true;
           delayed = 3;
         // return 1;
    }
    // not lost and not delayed
    error_msg = s + " : will send msg with id = "+  to_string(index_msg) +
            " and content = " +msgg->getM_Payload()  +error_msg_type +
            "and piggy backing ACK = "+  to_string(msg_ack) +" at time = " + to_string(t) ;
    addtoLogFile(error_msg);
    cout << s +"HAB3tttttttttttt 3ady" <<endl;
    double time = reply+0.01;
    //out = 2;
//    if(dupp)
//    {
//        if(delayed > 0)
//        {
//
//            sendDelayed(msg_s,reply+propdelay+3,"out");
//            sendDelayed(msg_s,time+propdelay+3,"out");
//            return;
//        }
//        else
//        {
//            sendDelayed(msg_s,reply+propdelay+3,"out");
//        }
//    }

        sendDelayed(msgg,reply+propdelay+delayed,"out");//send(msgg,"out"); // dup w delay m3 b3d???
        num_transmissions++;
        msg_seqno++;
        sent[index_msg] = true;
        if(dupp)
        {
            MyMessage_Base * msg_s = new MyMessage_Base();
            msg_s->setSeq_Num(msgg->getSeq_Num());
            msg_s->setM_Type(1);
            msg_s->setM_Payload( msgg->getM_Payload());
            msg_s->setpiggybackingID( msgg->getpiggybackingID());
            msg_s->setMycheckbits(msgg->getMycheckbits());
            msg_s->setackORnack(1);
            sendDelayed(msg_s,reply+propdelay+delayed+0.01,"out");
        }


   return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
int Node:: nodeReceiveData(MyMessage_Base *mmsg) // print tHE OTHER NODE piggybacking ack
{
    //double CRC=par("delay").doubleValue();
    MyMessage_Base * msg1 = new MyMessage_Base(" ");
    const char* str=getName(); //node name
    std::string s = str;
    double t = simTime().dbl();
    // initialize receive window...
     if( !initWindow){
       RwindowSize=mmsg->getWS();
       initWindow =  true;
     }
    // first see ack/nack received to shift window
    int acktype = mmsg->getackORnack();
    int nodeack = mmsg->getpiggybackingID();
    bool is_nack = false;
    int id;
    if(acktype==7)
    {
        //nack send the frame[piggyback]
//        string su = s + " rece not ackkkkkkkk";
//        addtoLogFile(su);
        //node2 R khara on2
        cout << s + " R  on" +  to_string(nodeack) <<endl;
        cout << s + "YAAAAAAAAAAAAAAAAA AREEEEEEEEEEEEEEEEEEEEEEEEJ - received" <<endl;
        id = mmsg->getpiggybackingID();
        //int m = replyNACK(mmsg);
        is_nack = true;
    }
    if (acktype==2 || acktype==3)
        {
//            cout << "HENAAAAAAAAAAAAAA" << endl;
//            cout << "S window size baa" + to_string(nodeack) <<endl;
//            SWindowStart++;
//            int j =  SWindowStart;
//            //SWindowStart = nodeack;
//            while(sent[j]==true)
//            {
//                j++;
//
//            }
//            SWindowStart = j;
//            cout << "HOWOCOME"+ to_string(j) <<endl;
//           cout << "Received type = " + to_string(acktype) <<endl;
        SWindowStart = nodeack;


           // cout << s+ " ha-send new frame start from " + to_string(j) << endl;

        }
    // start see what i received...........
    if (mmsg->getM_Type()==55)
    {
        string s0= s +  " : received nth as the paired node finished sending frames ";
        EV << s0 <<endl;
        addtoLogFile(s0);
        mmsg->setM_Type(55);
        srSend(mmsg);
        return 1;
    }
    string mymsg = mmsg->getM_Payload();
    // error detection by CRC
    string m = "";
    string errordetection = "";
            int err_typee =1;
    if (CRC == 1)
    {
        string characterCRC = (mmsg->getMycheckbits()).to_string();
        string Payload=mmsg->getM_Payload();
        string binaryStr="";
        for (int i = 0; i < Payload.length(); i++)
        {
                 bitset<8> binaryBits(Payload[i]);
                 binaryStr+=binaryBits.to_string();
        }
        cout<<binaryStr <<"     "<<binaryStr.length()<<endl;
        binaryStr=binaryStr+characterCRC[5]+characterCRC[6]+characterCRC[7];
        string CRCStr=calculateCRC(binaryStr);
        cout<<"CRC Bits: "<<CRCStr<<"   "<<binaryStr <<"     "<<binaryStr.length()<<endl;
        EV<<"CRC Bits: "<<CRCStr<<endl;
        bits CRCbits(CRCStr);

        bits zeross(0);

        if(CRCbits==zeross)
        {
            err_typee=2;// ACK no error
            errordetection = errordetection + "ACK = 2 ";
        }
        else
        {
            err_typee=3;// NACK error
            errordetection = errordetection + "  with Modification and NACK = 3";
        }
    }
    else
    {
        string final_msg = deStuffing(mymsg);
        string binaryStr="";
        for (int i = 0; i < final_msg.length(); i++)
        {
                 bitset<8> binaryBits(final_msg[i]);
                 binaryStr+=binaryBits.to_string();

        }
        vector<int> outt = hammingGeneration(binaryStr);
        int error_bit=0;
               if(mmsg->hamm.size()!=outt.size())
                           m= s + "Payload has other type of error than modification";
                       else{
                       hammingCorrection(outt,error_bit,binaryStr.length(),mmsg->hamm);//910

                       if(error_bit!=-1)
                       {
                          string finalmessage="";
                           if(binaryStr[binaryStr.length()-error_bit+9] == '1')
                                    binaryStr[binaryStr.length()-error_bit+9] ='0';
                           else
                                    binaryStr[binaryStr.length()-error_bit+9] ='1';

                           for(int i=0;i<binaryStr.size();i=i+8)
                               {
                               string myStr=binaryStr.substr(i, i+8);
                               const char* myCharArr = myStr.c_str();
                               finalmessage+=BinaryStr(myCharArr);
                               }


                           m= s + " Error Position = " + to_string(error_bit) + "Corrected message = "
                                   + finalmessage + ".";

                           err_typee=3;// NACK error
                          errordetection = errordetection + "  with Modification and NACK = 3";

                       }
                       else{
                           m=  s + " No Error Deteced by hamming " ;
                           err_typee=2;// ACK no error
                           errordetection = errordetection + "ACK = 2 ";
                       }

                       }






    }

    // de-stuffing
    string final_msg = deStuffing(mymsg);
    string error_msg = "";
//    if(mmsg->getSeq_Num() > RWindowStart + RwindowSize || mmsg->getSeq_Num() < RWindowStart )
//    {
//        error_msg = s + "Received msg out of the received window size" ;
//        cout << error_msg<<endl;
//        addtoLogFile(error_msg);
//        return 1;
//    }
    if(mmsg->getSeq_Num() < msg_ack )
    {
        error_msg = s + " : received and drop  duplicated message with id = " +to_string(mmsg->getSeq_Num())+" with content = "+ final_msg
                +"and piggy backing ACK ="+  to_string(nodeack)
                + " at time = " + to_string(t);
        cout << s +" Receiver Dup frame .. discard frame " <<endl;
        // LOG FILE
        EV << error_msg <<endl;
        addtoLogFile(error_msg);
        if(CRC==0)
        {
            addtoLogFile(m);
        }
        // msh hn3ml haga hya kdakda duplicated message
        msg1->setM_Type(1);
        msg1->setpiggybackingID(msg_ack);
        msg1->setackORnack(5);
        received[mmsg->getSeq_Num()]=true;
        //send same msg_ack i need
        if(is_nack)
            int m = replyNACK(msg1,id);
        else
        srSend(msg1);//nodeSendData(msg1);//sendACK(msg1);//sendDelayed(msg1,0.2,"out");
    }
    else if (mmsg->getSeq_Num() > msg_ack )
    {
        cout << "Receiver ..received frame( loss message occured) .. send new ACK" << endl;
        error_msg = s + " : received message! with id = " +  to_string(mmsg->getSeq_Num()) +
                " and content = " +final_msg   + errordetection +
                  "and piggy backing ACK ="+  to_string(nodeack) +" at time = "  + to_string(t);
        // LOG FILE
        EV << error_msg <<endl;
        addtoLogFile(error_msg);
        if(CRC==0)
        {
            addtoLogFile(m);
        }
        received[mmsg->getSeq_Num()]=true;
        cout << "AYWA LOST AHUUUUUUUUUUUUUUUUUUUUU" <<endl;
        msg1->setM_Type(1);
        msg1->setackORnack(7);
        msg1->setpiggybackingID(msg_ack);
        cout << s + " AYWA LOST NACKKKK AHUUUUUUUUUUUUUUUUUUUUU" <<endl;
        // elmfrud a3ml hena TIMEOUT 3LA EL NACK
        if( nackSentID ==-1 || mmsg->getSeq_Num()!=nackSentID)
        {

            nackSentID =  mmsg->getSeq_Num();
            cout << s + "HENDAH" <<endl;

                recghalt = true;
                if(is_nack)
                {
                    cout << s + "HENDAH?" <<endl;
                    cout << s + " rece id = " + to_string(mmsg->getSeq_Num()) +
                            " wana 3yza " + to_string(msg_ack) <<endl;
                    cout << s + " tl type bet3 msg el gatly =  " + to_string(mmsg->getackORnack()) <<endl;
                    int m = replyNACK(msg1,id);
                }

                else
                {
                    cout << s + "HENDAH??" <<endl;

                    cout << s + " rece id = " + to_string(mmsg->getSeq_Num()) +
                            " wana 3yza " + to_string(msg_ack) <<endl;
                    int m =srSend(msg1);
                }


        }
        else{
            // hena by2of w msh bykml 3shan henak bykon lisa msh received el nack ?
            //msg1->setM_Type(1);
            msg1->setackORnack(1);
            if(is_nack)
               int m = replyNACK(msg1,id);
           else
              int m = srSend(msg1);
           // scheduleAt(simTime()+2, msg1);
        }
        //nodeSendData(msg1);//sendDelayed(msg1,0.2,"out");
        // make time out for this nack
        //MyMessage_Base * selfmsgforNACK;
        //????????????????????selfmsgforNACK->setM_Type(8);
       //????????????????????scheduleAt(simTime()+0.2, selfmsgforNACK);
    }
    else
    {
        cout << s + " Receiver ..received frame .. send new ACK" << endl;
        error_msg = s + " : received message with id = " +  to_string(mmsg->getSeq_Num()) +
                " and content = " + final_msg + errordetection +
                  "  and piggy backing ACK ="+  to_string(nodeack) +" at time = " + to_string(t) ;
        string dbg = s + " : received message with id = " +  to_string(mmsg->getSeq_Num());
        // LOG FILE
        received[msg_ack]=true;
        if(nackSentID == msg_ack)
        {
            //????????????????????cancelAndDelete (selfmsgforNACK);
            //update the received window start
            int i =msg_ack; //0;
            while (received[i] == true)
            {
                i++;
            }
            RWindowStart=i;
            msg_ack=i;
            nackSentID = -1;
        }
        else
        {
            RWindowStart++; //received msg el sah
            msg_ack++;
        }
        EV << error_msg <<endl;
        addtoLogFile(error_msg);
        if(CRC==0)
        {
            addtoLogFile(m);
        }
        msg1->setM_Type(1);
        msg1->setpiggybackingID(msg_ack);
        msg1->setackORnack(err_typee);
        //send same msg_ack i need

        if(dbg == "node2 : received message with id = 3"){
            if(is_nack)
               cout << "was isnack " <<endl;
            else
              cout << "was srSend " <<endl;
        }


        if(is_nack)
           int m = replyNACK(msg1,id);
        else
          srSend(msg1);


    }


    return 1;
}
int Node::srSend(MyMessage_Base *mmsg)
{

    double t = simTime().dbl();
    const char* str=getName();
           std::string s = str;
    ///////////////////////////// check if done sending msgs
    if ( msg_seqno > messages.size()-1  )
    {
        // LOG FILE
        if (doneOnce == false)
        {
            cout << "Sender finish sending messages.." <<endl;
            string s0= s +  " : finish sending frames at time = " + to_string(t);
            EV << s0 <<endl;
            addtoLogFile(s0);
            doneOnce = true;
        }
        if(mmsg->getM_Type()==55)
        {
         //both finished
            mmsg->setM_Type(55);
            cout << "Both nodes finished..session finished.." <<endl;
            double t = simTime().dbl();
            string s0= "Both nodes finished..session finished.. at time = " + to_string(t);
            EV << s0 <<endl;
            addtoLogFile(s0);
            return 1;
        }
            mmsg->setM_Type(55);
            cout << "okkk ana khlst?" << endl;
            index++;
            sendDelayed(mmsg,0.05*index+propdelay,"out");//send(msgg,"out");
            return 1;
    }
    /////////////////////////check if sent buffer malyan
    int j = msg_seqno;
    if(sent[j] == true)
    {
        int j = msg_seqno;
        int f = false;
        while ( j < SwindowSize+SWindowStart)
        {
            if(sent[j]== false )
            {
                f= true;
                break;
            }
            j++;
        }
        if(f==true){
            msg_seqno=j;
            //SWindowStart = j;
        }
        else{
            //scedule   SELFMSG to complete
            //re send the frame at the begining of the window
            mmsg->setM_Type(9);
            scheduleAt(simTime()+1, mmsg);
            return 1;
        }
    }

    //reply = 0.05;
    int i =0;
    cout<< s +" start wind at sending "<<SWindowStart <<endl;
    cout<< s+ " full wind at sending "<<SwindowSize <<endl;
    cout<< s +" msg_seqno at sending "<<msg_seqno <<endl;
//    if(msg_seqno >= SwindowSize+SWindowStart)
//    {
//        // time out 3shan ahrk windowsender?
//        mmsg->setM_Type(9);
//        cout << s + " ana hena?" <<endl;
//        //sent[SWindowStart]=false;
//        int x = msg_seqno+10;
//        no_ack = msg_seqno;
//        scheduleAt(simTime()+0.05*x, mmsg);
//    }
    while (msg_seqno < SwindowSize+SWindowStart)
    {


        if ( msg_seqno > messages.size()-1  )
        {
            return 1;
        }
        if(recghalt)
        {

            nackSentID = -1;
            nackSent = true;
            //nackSentID = msg_ack;
            recghalt= false;
        }
        cout << s + " wla ana hena?" <<endl;
        EV <<"Sender schedule self message" << endl;
        //i++;
        //scheduleAt(simTime()+0.05*i, msg_s);
        //reply = reply*i;
        cout << "REPPPPLYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY" << endl;
        cout << to_string(i*0.05) <<endl;
        int m =nodeSendData(mmsg,i);
        if (mmsg->getackORnack()==7)
            mmsg->setackORnack(1);
        i++;
    }

return 1;
}

void Node::handleMessage(cMessage *msg)
{
    double delayy=par("delay").doubleValue();

    // TODO - Generated method body
    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
    int start = mmsg->getSeq_Num();
    int type = mmsg->getM_Type();
    if(msg->isSelfMessage()) // self message to start send messages to receiver node
    {
        double t = simTime().dbl();
        const char* str=getName();
        std::string s = str;
        //LOG FILE
        if (type == 6)
        {
            string m =  s +" : received self message to start sending messages at time = " + to_string(t) ;
            EV << m <<endl;
            addtoLogFile(m);
            int ackt = mmsg->getackORnack();
            int done = srSend(mmsg);
        }

         if( type == 4 )
         {
             string m2 = s + " : time out .. complete sending messages";
             EV << m2 <<endl;
             addtoLogFile(m2);
             int ackt = mmsg->getackORnack();
             int done = srSend(mmsg);
         }
        if(type ==9)
        {
            string m2 = s + " : got time out for receiving ACK to move sending window .. complete sending messages";
            EV << m2 <<endl;
            addtoLogFile(m2);
            if(SWindowStart >no_ack){
                string h = s + "already received ack and moved the sender windows";
            }
            else{

                sent[no_ack]=false;
                msg_seqno=no_ack;
                int done = srSend(mmsg);
            }


        }
        if(type==3)
        {
            int m =nodeSendData(mmsg,0.05);
        }

    }
    else if( type == 0  ) // message from coordinator to any node (Sender/receiver)
    {
        if(start != -1 ) // if node is sender then read file
        {
            //sender
            cout << "yarabbbbbbb???" <<endl;
            senderStart(start,mmsg);
        }
        else // msg from coordinator to receiver
        {
            //receiver
            cout << "yarabbbbbbb??" <<endl;
            receiverStart(mmsg);
        }

    }
    else if( type == 1 || type == 55   )// receiver receive data message from the other node
    {
        cout << "yarabbbbbbb" <<endl;
        int y = nodeReceiveData(mmsg); // receive data and send ACK (and send msg????)
        // 2 ACK
        // 3 NACK error in msg only
        // 3 in order,dup
        // 5 inorder,loss
    }

}
