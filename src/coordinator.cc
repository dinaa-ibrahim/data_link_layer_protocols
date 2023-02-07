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

#include "coordinator.h"
#include "MyMessage_m.h"
using namespace std;
Define_Module(Coordinator);

void Coordinator::initialize()
{
    // TODO - Generated method body
    //read file n
    cout << "Coordinator: initialize" <<endl;
    std::string file_name = "../inputsSamples/coordinator.txt";
    coordinationFile.open(file_name, std::ios::in);
    EV << "Coordinator start initialization" <<endl;
    if (!coordinationFile)
        ended = true;
    else
        ended = false;
     string line;
     while (getline(coordinationFile, line))
     {
        cout << "Coordinator: new line : "<< line <<endl;
        bool endoffile = coordinationFile.eof();
        if (endoffile || ended)
        {
            coordinationFile.close();
            ended = true;
            cout << "111111111111" <<endl;
            break;
        }
        string inputs;
        vector<string> inputs_arr;
        string space = " ";
        size_t start_pos = 0, end_pos;
        bool newline = false;
        while ((end_pos = line.find(space, start_pos)) != std::string::npos)
        {
            cout << "2222" <<endl;
            newline = true;
            string ss = line.substr(start_pos, end_pos - start_pos);
            inputs_arr.push_back(ss);
            cout << "Coordinator: splitting each part of the line : "<< ss << endl;
            start_pos = end_pos + space.size();
        }
        string ss = line.substr(start_pos);

        if (newline == true)
        {
            inputs_arr.push_back(ss);
            cout << "Coordinator: end of the line : "<< ss << endl;
        }
        // send message to each node
        MyMessage_Base * msg = new MyMessage_Base(" ");
        msg->setM_Payload(inputs_arr[1].c_str());
        msg->setSeq_Num(-1);
        msg->setM_Type(0);
        if (inputs_arr[2]=="start") //sender seqNum = start time
        {
            cout << "check if the node is sender" << endl;
            msg->setSeq_Num(stoi(inputs_arr[3]));
        }
        EV << "Coordinator will send data to the node" << stoi(inputs_arr[0]) <<endl;
        send(msg,"out",stoi(inputs_arr[0]));
        cout << "send the data to the node" << endl;
        inputs_arr.clear();
      }

     EV << "Coordinator finished" << endl;

}
void Coordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body

}
