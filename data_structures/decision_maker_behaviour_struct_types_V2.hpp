#ifndef CADMIUM_DECISION_MAKER_BEHAVIOUR_STRUCT_TYPES_HPP
#define CADMIUM_DECISION_MAKER_BEHAVIOUR_STRUCT_TYPES_HPP

#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <chrono>
#include <algorithm>
#include <limits>
#include <math.h>
#include <memory>
#include <iomanip>

#include "nep_model_enum_types.hpp"
#include "decision_maker_behaviour_enum_types.hpp"
#include "../tinyXML/tinyxml.h"
#include "../tinyXML/tinystr.h"

using namespace std;
using namespace nep_model_enum_types;
using namespace decision_maker_behaviour_enum_types;

namespace decision_maker_behaviour_structures{

  /*******************************************/
  /***** XML Auxiliary structures  ***********/
  /*******************************************/
  class PriorityDevice{
  public:
    PriorityDevice(){}
    PriorityDevice(int priority, DeviceType device)  {
      this->priority=priority;
      this->device=device;
    }
    PriorityDevice(int priority, DeviceType device,bool send2Multiple, bool sendSeparateFromReceive)  {
      this->priority=priority;
      this->device=device;
      this->send2Multiple=send2Multiple;
      this->sendSeparateFromReceive = sendSeparateFromReceive;
    }
    int priority;
    DeviceType device;
    bool send2Multiple;
    bool sendSeparateFromReceive;
  };

  class PriorityTask{
  public:
    PriorityTask(){}
    PriorityTask(int priority, TaskPriorityType task){
      this->priority=priority;
      this->task=task;
    }
    int priority;
    TaskPriorityType task;
  };

  class PriorityPerson{
  public:
    PriorityPerson(){}
    PriorityPerson(int priority, const string& personId){
      this->priority=priority;
      this->personId=personId;
    }
    int priority;
    string personId;
  };

  class PriorityCommandTo{
  public:
    PriorityCommandTo(){}
    PriorityCommandTo(int priority, const string& to,  const string& msg)  {
      this->priority=priority;
      this->to=to;
      this->msg=msg;
    }
    int priority;
    string to;
    string msg;
  };

  class RelationPersonDevices{
  public:
    RelationPersonDevices(){}
    RelationPersonDevices(const string& personId, vector<DeviceType> devices)  {
      this->personId=personId;
      this->devices=devices;
    }
    string personId;
    vector<DeviceType> devices;
  };

  class Confirmation{
  public:
    Confirmation(){}
    Confirmation(const string& from,const string& content)  {
      this->from=from;
      this->content=content;
    }
    string from;
    string content;
    bool equal(Confirmation a){
      bool res = true;
      res = res && (a.from == this->from);      
      res = res && (a.content == this->content);
      return res;
    }
  };

  class Msg2Send{
  public:
    Msg2Send(){}
    Msg2Send(const string& to,const string& content)  {
      this->to=to;
      this->content=content;
    }
    string to;
    string content;
    bool   compulsory;
    bool   send2Multiple;
    vector<DeviceType>   deviceCompulsory;
    vector<Confirmation> confirmations;

    bool equal(Msg2Send a){
      bool res = true;
      res = res && (a.to == this->to);
      res = res && (a.content == this->content);      
      res = res && (a.compulsory == this->compulsory);
      res = res && (a.send2Multiple == this->send2Multiple);
      res = res && (a.deviceCompulsory == this->deviceCompulsory);
      if(a.confirmations.size() == this->confirmations.size()){
        for(int i = 0; i<a.confirmations.size(); i++){
          res = res && (a.confirmations[i].equal(this->confirmations[i]));
        }
      }else{
        res = res && (a.confirmations.size() == this->confirmations.size());
      }
      return res;
    }
  };

  class MsgReceivedBehaviour{
  public:
    MsgReceivedBehaviour(){}
    MsgReceivedBehaviour(const string& from,const string& content, vector<Msg2Send> msg2send, vector<string> actions2Do)  {
      this->from=from;
      this->content=content;
      this->msg2send=msg2send;
      this->actions2Do=actions2Do;
    }
    string from;
    string content;
    vector<Msg2Send> msg2send;
    vector<string> actions2Do;
  };

  class ActionBehaviour{
  public:
    ActionBehaviour(){}
    ActionBehaviour(const string& id,const string& averageExectionTime, const string& location, vector<Msg2Send> msg2send)  {
      this->id=id;
      this->averageExectionTime=averageExectionTime;
      this->location=location;
      this->msg2send=msg2send;
    }
    string id;
    string averageExectionTime;
    string location;
    vector<Msg2Send> msg2send;

    bool equal(ActionBehaviour a){
      bool res = true;
      res = res && (a.id == this->id);
      res = res && (a.averageExectionTime == this->averageExectionTime);      
      res = res && (a.location == this->location);
      if(a.msg2send.size() == this->msg2send.size()){
        for(int i = 0; i<a.msg2send.size(); i++){
          res = res && (a.msg2send[i].equal(this->msg2send[i]));
        }
      }else{
        res = res && (a.msg2send.size() == this->msg2send.size());
      }
      return res;
    }
  };  

  /*******************************************/
  /**** XML Behaviour structures *************/
  /*******************************************/
  class DecisionMakerBehaviour{
  public:
    DecisionMakerBehaviour() = default;
    string                         id;
    string                         location;
    string                         reactionTime;
    AnswerPriorityType             answerPriority;
    SendPriorityType               sendPriority;
    vector<PriorityDevice>         myDevices;
    vector<PriorityTask>           taskPrioritized;
    vector<PriorityDevice>         answerDevicePriority;
    vector<PriorityPerson>         answerPersonPriority;
    vector<PriorityCommandTo>      sendCommandPriority;
    vector<string>                 actionExecutionPriority;
    vector<RelationPersonDevices>  communicationRelations;
    vector<string>                 peopleSameLocation;
    vector<MsgReceivedBehaviour>   msgBehaviour;
    vector<ActionBehaviour>        actionBehaviour;

    void save(const char* pFilename){
      //cout << "entra aqui save" << endl;
      TiXmlDocument doc;  
      TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
      doc.LinkEndChild(decl); 
     
      TiXmlElement * root = new TiXmlElement("DecisionMakerBehaviour");  
      doc.LinkEndChild( root );  
      // block: Id
        {           
          TiXmlElement * pId = new TiXmlElement( "Id" );          
          pId->LinkEndChild(new TiXmlText(id.c_str()));
          root->LinkEndChild( pId );  
        }
      // block: Location
        {           
          TiXmlElement * pLocation = new TiXmlElement( "Location" );          
          pLocation->LinkEndChild(new TiXmlText(location.c_str()));
          root->LinkEndChild( pLocation );  
        }
      // block: ReactionTime
        {           
          TiXmlElement * pReactionTime = new TiXmlElement( "ReactionTime" );          
          pReactionTime->LinkEndChild(new TiXmlText(reactionTime.c_str()));
          root->LinkEndChild( pReactionTime );  
        }
      
      // block: AnswerPriorityType
        {    
          TiXmlElement * pAnswerPriorityType = new TiXmlElement( "AnswerPriorityType" );
          string aux = ToString(answerPriority);
          pAnswerPriorityType->LinkEndChild(new TiXmlText(aux.c_str()));
          root->LinkEndChild( pAnswerPriorityType );  
        }
      // block: SendPriorityType
        {    
          TiXmlElement * pSendPriorityType = new TiXmlElement( "SendPriorityType" );
          string aux = ToString(sendPriority);
          pSendPriorityType->LinkEndChild(new TiXmlText(aux.c_str()));
          root->LinkEndChild( pSendPriorityType );  
        }      
      // block: MyDevices
        {    
          vector<PriorityDevice>::iterator iter;
          TiXmlElement * MyDevicesNode = new TiXmlElement("MyDevices");
          root ->LinkEndChild(MyDevicesNode);
          for (iter=myDevices.begin(); iter != myDevices.end(); iter++)
          {
            const PriorityDevice& w=*iter;      
            TiXmlElement * priorityD = new TiXmlElement( "PriorityDevice" );
            MyDevicesNode->LinkEndChild( priorityD );            
            priorityD->SetAttribute("priority", w.priority);
            string aux = ToString(w.device);  
            priorityD->SetAttribute("device", aux.c_str());
            (w.send2Multiple) ? aux = string("true") : aux = string("false");
            priorityD->SetAttribute("send2Multiple", aux.c_str());
            (w.sendSeparateFromReceive) ? aux = string("true") : aux = string("false");
            priorityD->SetAttribute("sendSeparateFromReceive", aux.c_str());             
          }
        }    
      // block: TaskPrioritized
        {    
          vector<PriorityTask>::iterator iter;
          TiXmlElement * TaskPrioritizedNode = new TiXmlElement("TaskPrioritized");
          root ->LinkEndChild(TaskPrioritizedNode);
          for (iter=taskPrioritized.begin(); iter != taskPrioritized.end(); iter++)
          {
            const PriorityTask& w=*iter;      
            TiXmlElement * priorityT = new TiXmlElement( "PriorityTask" );
            TaskPrioritizedNode->LinkEndChild( priorityT );            
            priorityT->SetAttribute("priority", w.priority);
            string aux = ToString(w.task);  
            priorityT->SetAttribute("task", aux.c_str());            
          }
        }      
      // block: AnswerDevicePriority
        {    
          vector<PriorityDevice>::iterator iter;
          TiXmlElement * AnswerDevicePriorityNode = new TiXmlElement("AnswerDevicePriority");
          root ->LinkEndChild(AnswerDevicePriorityNode);
          for (iter=answerDevicePriority.begin(); iter != answerDevicePriority.end(); iter++)
          {
            const PriorityDevice& w=*iter;      
            TiXmlElement * priorityD = new TiXmlElement( "PriorityDevice" );
            AnswerDevicePriorityNode->LinkEndChild( priorityD );            
            priorityD->SetAttribute("priority", w.priority);
            string aux = ToString(w.device);  
            priorityD->SetAttribute("device", aux.c_str());            
          }
        }      
      // block: AnswerPersonPriority
        {    
          vector<PriorityPerson>::iterator iter;
          TiXmlElement * AnswerPersonPriorityNode = new TiXmlElement("AnswerPersonPriority");
          root ->LinkEndChild(AnswerPersonPriorityNode);
          for (iter=answerPersonPriority.begin(); iter != answerPersonPriority.end(); iter++)
          {
            const PriorityPerson& w=*iter;      
            TiXmlElement * priorityD = new TiXmlElement( "PriorityPerson" );
            AnswerPersonPriorityNode->LinkEndChild( priorityD );
            priorityD->SetAttribute("priority", w.priority); 
            priorityD->SetAttribute("id", w.personId.c_str());            
          }
        }
      // block: SendCommandPriority
        {    
          vector<PriorityCommandTo>::iterator iter;
          TiXmlElement * SendCommandPriorityNode = new TiXmlElement("SendCommandPriority");
          root ->LinkEndChild(SendCommandPriorityNode);
          for (iter=sendCommandPriority.begin(); iter != sendCommandPriority.end(); iter++)
          {
            const PriorityCommandTo& w=*iter;      
            TiXmlElement * priorityD = new TiXmlElement( "PriorityCommandTo" );
            SendCommandPriorityNode->LinkEndChild( priorityD );
            priorityD->SetAttribute("priority", w.priority); 
            priorityD->SetAttribute("to", w.to.c_str());
            priorityD->SetAttribute("msg", w.msg.c_str());            
          }
        }
      // block: ActionExecutionPriority
        {    
          vector<string>::iterator iter;
          TiXmlElement * ActionExecutionPriorityNode = new TiXmlElement("ActionExecutionPriority");
          root ->LinkEndChild(ActionExecutionPriorityNode);
          for (iter=actionExecutionPriority.begin(); iter != actionExecutionPriority.end(); iter++)
          {
            const string& w=*iter;      
            TiXmlElement * priorityD = new TiXmlElement( "PriorityAction" );
            ActionExecutionPriorityNode->LinkEndChild( priorityD );
            priorityD->SetAttribute("id", w.c_str());          
          }
        }      
      // block: CommunicationRelations
        {    
          vector<RelationPersonDevices>::iterator iter;
          TiXmlElement * CommunicationRelationsNode = new TiXmlElement("CommunicationRelations");
          root ->LinkEndChild(CommunicationRelationsNode);
          for (iter=communicationRelations.begin(); iter != communicationRelations.end(); iter++)
          { 
            const RelationPersonDevices& w=*iter;
            TiXmlElement * Relations = new TiXmlElement("RelationPerson");
            CommunicationRelationsNode ->LinkEndChild(Relations);
            Relations->SetAttribute("id", w.personId.c_str());            
            vector<DeviceType>::iterator iter2;
            vector<DeviceType> aux_devices;
            aux_devices = w.devices;
            for (iter2=aux_devices.begin(); iter2 != aux_devices.end(); iter2++)
            {
              const DeviceType& w2=*iter2;      
              TiXmlElement * DeviceNode = new TiXmlElement( "Device" );
              Relations->LinkEndChild( DeviceNode );
              string aux = ToString(w2); 
              DeviceNode->SetAttribute("device", aux.c_str());
            }
          }
        }      
      // block: PeopleSameLocation
        {    
          vector<string>::iterator iter;
          TiXmlElement * PersonSameLocationNode = new TiXmlElement("PeopleSameLocation");
          root ->LinkEndChild(PersonSameLocationNode);
          for (iter=peopleSameLocation.begin(); iter != peopleSameLocation.end(); iter++)
          {
            const string& w=*iter;      
            TiXmlElement * priorityD = new TiXmlElement( "Person" );
            PersonSameLocationNode->LinkEndChild( priorityD ); 
            priorityD->SetAttribute("id", w.c_str());
          }
        }
      // block: MsgBehaviour
        {    
          vector<MsgReceivedBehaviour>::iterator iter;
          TiXmlElement * MsgBehaviourNode = new TiXmlElement("MsgBehaviour");
          root ->LinkEndChild(MsgBehaviourNode);
          for (iter=msgBehaviour.begin(); iter != msgBehaviour.end(); iter++)
          { 
            TiXmlElement * MsgReceivedNode = new TiXmlElement("MsgReceived");
            MsgBehaviourNode ->LinkEndChild(MsgReceivedNode);
            const MsgReceivedBehaviour& w=*iter;
            MsgReceivedNode->SetAttribute("from", w.from.c_str());
            MsgReceivedNode->SetAttribute("content", w.content.c_str());            
            vector<Msg2Send>::iterator iter2;
            vector<Msg2Send> aux_msg;
            aux_msg = w.msg2send;
            for (iter2=aux_msg.begin(); iter2 != aux_msg.end(); iter2++)
            {
              const Msg2Send& w2=*iter2;      
              TiXmlElement * Msg2SendNode = new TiXmlElement( "Msg2Send" );
              MsgReceivedNode->LinkEndChild( Msg2SendNode );
              Msg2SendNode->SetAttribute("to", w2.to.c_str());
              Msg2SendNode->SetAttribute("content", w2.content.c_str());
              string aux;
              (w2.compulsory) ? aux=string("true") : aux =string("false"); 
              Msg2SendNode->SetAttribute("compulsory", aux.c_str());
              (w2.send2Multiple) ? aux=string("true") : aux =string("false"); 
              Msg2SendNode->SetAttribute("send2Multiple", aux.c_str());      
              vector<DeviceType>::iterator iter3;
              vector<DeviceType> aux_device;
              aux_device = w2.deviceCompulsory;
              if(!aux_device.empty()){
                for(iter3=aux_device.begin(); iter3 != aux_device.end(); iter3++){
                  const DeviceType& w3=*iter3;   
                  TiXmlElement * Device = new TiXmlElement( "Device" );
                  string aux = ToString(w3);
                  Device->SetAttribute("device", aux.c_str());             
                  Msg2SendNode->LinkEndChild( Device );
                }
              }              
              vector<Confirmation>::iterator iter4;
              vector<Confirmation> aux_confirmations;
              aux_confirmations = w2.confirmations;
              if(!aux_confirmations.empty()){
                for(iter4=aux_confirmations.begin(); iter4 != aux_confirmations.end(); iter4++){
                 const Confirmation& w4=*iter4;
                  TiXmlElement * Confirmation = new TiXmlElement( "Confirmation" );
                  Msg2SendNode->LinkEndChild( Confirmation );
                  Confirmation->SetAttribute("from", w4.from.c_str());
                  Confirmation->SetAttribute("content", w4.content.c_str());             
                }
              }              
            }
            vector<string>::iterator iter5;
            vector<string> aux_action;
            aux_action = w.actions2Do;
            if(!aux_action.empty()){
              for(iter5=aux_action.begin(); iter5!= aux_action.end(); iter5++){
                const string& w5=*iter5;
                TiXmlElement * Action2Do = new TiXmlElement( "Action2Do" );
                MsgReceivedNode->LinkEndChild( Action2Do );
                Action2Do->SetAttribute("id", w5.c_str());        
              }
            }            
          }
        }
      // block: ActionBehaviour
        {    
          vector<ActionBehaviour>::iterator iter;
          TiXmlElement * ActionBehaviourNode = new TiXmlElement("ActionBehaviour");
          root ->LinkEndChild(ActionBehaviourNode);
          for (iter=actionBehaviour.begin(); iter != actionBehaviour.end(); iter++)
          { 
            TiXmlElement * Action = new TiXmlElement("Action");
            ActionBehaviourNode ->LinkEndChild(Action);
            const ActionBehaviour& w=*iter;
            Action->SetAttribute("id", w.id.c_str());
            TiXmlElement * AverageExecutionTime = new TiXmlElement( "AverageExecutionTime" );
            AverageExecutionTime->SetAttribute("time", w.averageExectionTime.c_str());
            Action->LinkEndChild( AverageExecutionTime );              
            TiXmlElement * Location = new TiXmlElement( "Location" );
            Location->LinkEndChild( new TiXmlText(w.location.c_str()) );              
            Action->LinkEndChild( Location );
            vector<Msg2Send>::iterator iter2;
            vector<Msg2Send> aux_msg;
            aux_msg = w.msg2send;
            for (iter2=aux_msg.begin(); iter2 != aux_msg.end(); iter2++)
            {
              const Msg2Send& w2=*iter2;      
              TiXmlElement * Msg2SendNode = new TiXmlElement( "Msg2Send" );
              Action->LinkEndChild( Msg2SendNode );
              Msg2SendNode->SetAttribute("to", w2.to.c_str());
              Msg2SendNode->SetAttribute("content", w2.content.c_str());
              string aux;
              (w2.compulsory) ? aux=string("true") : aux =string("false");
              Msg2SendNode->SetAttribute("compulsory", aux.c_str());
              (w2.send2Multiple) ? aux=string("true") : aux =string("false");
              Msg2SendNode->SetAttribute("send2Multiple", aux.c_str());     
              vector<DeviceType>::iterator iter3;
              vector<DeviceType> aux_device;
              aux_device = w2.deviceCompulsory;
              for(iter3=aux_device.begin(); iter3 != aux_device.end(); iter3++){
                const DeviceType& w3=*iter3;    
                TiXmlElement * Device = new TiXmlElement( "Device" );
                string aux = ToString(w3);
                Device->SetAttribute("device", aux.c_str());              
                Msg2SendNode->LinkEndChild( Device );
              }
              vector<Confirmation>::iterator iter4;
              vector<Confirmation> aux_confirmations;
              aux_confirmations = w2.confirmations;             
              for(iter4=aux_confirmations.begin(); iter4 != aux_confirmations.end(); iter4++){
                const Confirmation& w4=*iter4; 
                TiXmlElement * Confirmation = new TiXmlElement( "Confirmation" );
                Msg2SendNode->LinkEndChild( Confirmation );
                Confirmation->SetAttribute("from", w4.from.c_str());
                Confirmation->SetAttribute("content", w4.content.c_str());             
              }
            }
          }
        }
      doc.SaveFile(pFilename); 
    }
    void load(const char* pFilename){     
      //cout << "entra aqui load" << endl;         
      TiXmlDocument _document;
      bool result = _document.LoadFile(pFilename);
      if (!result) {cout << "file not loaded" << _document.ErrorDesc() << endl; return;}   
      TiXmlHandle hDoc(&_document);
      TiXmlElement* pElem;
      TiXmlHandle hRoot(0);
      // block: root
        {
          pElem=hDoc.FirstChildElement().Element();
          // should always have a valid root but handle gracefully if it does
          if (!pElem) return;    
          // save this for later
          hRoot=TiXmlHandle(pElem);
        }      
      // block: Id
        { 
          pElem=hRoot.FirstChild("Id").Element();
          if (!pElem) return;
          const char* pId = pElem->GetText();
          if(pId) id = pId;
        }
       // block: Location
        { 
          pElem=hRoot.FirstChild("Location").Element();
          if (!pElem) return;
          const char* pLocation = pElem->GetText();
          if(pLocation) location = pLocation;
        }
      // block: ReactionTime
        {
          pElem=hRoot.FirstChild("ReactionTime").Element();
          if (!pElem) return;
          const char* pReactionTime = pElem->GetText();
          if(pReactionTime) reactionTime = pReactionTime;
        }
      // block: AnswerPriorityType
        {
          pElem=hRoot.FirstChild("AnswerPriorityType").Element();
          if (!pElem) return;
          const char* pAnswerPriorityType = pElem->GetText();
          string aux;
          if(pAnswerPriorityType) aux = pAnswerPriorityType;
          istringstream ss;
          ss.str(aux);
          ss >> answerPriority;
        }
      // block: SendPriorityType
        {
          pElem=hRoot.FirstChild("SendPriorityType").Element();
          if (!pElem) return;
          const char* pSendPriorityType = pElem->GetText();
          string aux;
          if(pSendPriorityType) aux = pSendPriorityType;
          istringstream ss;
          ss.str(aux);
          ss >> sendPriority;
        }
      // block: myDevices
        {
          myDevices.clear(); // trash existing list    
          TiXmlElement* pMyDevicesNode=hRoot.FirstChild( "MyDevices" ).FirstChild().Element();
          for( pMyDevicesNode; pMyDevicesNode; pMyDevicesNode=pMyDevicesNode->NextSiblingElement())
          {
            PriorityDevice c;
            string aux;
            const char *pdevice=pMyDevicesNode->Attribute("device");
            if (pdevice) aux=pdevice;
            istringstream ss;
            ss.str(aux);
            ss >> c.device;       
            pMyDevicesNode->QueryIntAttribute("priority", &c.priority); // If this fails, original value is left as-is
            const char *psend2multiple=pMyDevicesNode->Attribute("send2Multiple");
            if (psend2multiple) aux=psend2multiple;
            (aux == string("true")) ? c.send2Multiple = true : c.send2Multiple = false;
            const char *psendSeparateFromReceive=pMyDevicesNode->Attribute("sendSeparateFromReceive");
            if (psendSeparateFromReceive) aux=psendSeparateFromReceive;
            (aux == string("true")) ? c.sendSeparateFromReceive = true : c.sendSeparateFromReceive = false;
            myDevices.push_back(c);
          }
        }
      // block: taskPrioritized
        {
          taskPrioritized.clear(); // trash existing list    
          TiXmlElement* pTaskPrioritized=hRoot.FirstChild( "TaskPrioritized" ).FirstChild().Element();
          for( pTaskPrioritized; pTaskPrioritized; pTaskPrioritized=pTaskPrioritized->NextSiblingElement())
          {
            PriorityTask c;
            string aux;
            const char *ptask=pTaskPrioritized->Attribute("task");
            if (ptask) aux=ptask;
            istringstream ss;
            ss.str(aux);
            ss >> c.task;       
            pTaskPrioritized->QueryIntAttribute("priority", &c.priority); // If this fails, original value is left as-is
            taskPrioritized.push_back(c);
          }
        }
      // block: answerDevicePriority
        {
          answerDevicePriority.clear(); // trash existing list    
          TiXmlElement* pAnswerPriorityNode=hRoot.FirstChild( "AnswerDevicePriority" ).FirstChild().Element();
          for( pAnswerPriorityNode; pAnswerPriorityNode; pAnswerPriorityNode=pAnswerPriorityNode->NextSiblingElement())
          {
            PriorityDevice c;
            string aux;
            const char *pdevice=pAnswerPriorityNode->Attribute("device");
            if (pdevice) aux=pdevice;
            istringstream ss;
            ss.str(aux);
            ss >> c.device;       
            pAnswerPriorityNode->QueryIntAttribute("priority", &c.priority); // If this fails, original value is left as-is
            answerDevicePriority.push_back(c);
          }
        }
      // block: answerPersonPriority
        {
          answerPersonPriority.clear(); // trash existing list    
          TiXmlElement* pAnswerPriorityNode=hRoot.FirstChild( "AnswerPersonPriority" ).FirstChild().Element();
          for( pAnswerPriorityNode; pAnswerPriorityNode; pAnswerPriorityNode=pAnswerPriorityNode->NextSiblingElement())
          {
            PriorityPerson c;
            const char *pid=pAnswerPriorityNode->Attribute("id");
            if (pid) c.personId=pid;     
            pAnswerPriorityNode->QueryIntAttribute("priority", &c.priority); // If this fails, original value is left as-is
            answerPersonPriority.push_back(c);
          }
        }
      // block: sendCommandPriority
        {
          sendCommandPriority.clear(); // trash existing list    
          TiXmlElement* pcommandPriorityNode=hRoot.FirstChild( "SendCommandPriority" ).FirstChild().Element();
          for( pcommandPriorityNode; pcommandPriorityNode; pcommandPriorityNode=pcommandPriorityNode->NextSiblingElement())
          {
            PriorityCommandTo c;
            const char *pTo=pcommandPriorityNode->Attribute("to");
            if (pTo) c.to=pTo;
            const char *pMsg=pcommandPriorityNode->Attribute("msg");
            if (pTo) c.msg=pMsg;          
            pcommandPriorityNode->QueryIntAttribute("priority", &c.priority); // If this fails, original value is left as-is
            sendCommandPriority.push_back(c);
          }
        }
      // block: ActionExecutionPriority
        {
          actionExecutionPriority.clear(); // trash existing list    
          TiXmlElement* pActionExecutionPriorityNode=hRoot.FirstChild( "ActionExecutionPriority" ).FirstChild().Element();
          for( pActionExecutionPriorityNode; pActionExecutionPriorityNode; pActionExecutionPriorityNode=pActionExecutionPriorityNode->NextSiblingElement())
          {
            string c;
            const char *pId=pActionExecutionPriorityNode->Attribute("id");
            if (pId) c=pId;
            actionExecutionPriority.push_back(c);
          }
        }
      // block: CommunicationRelations
        {
          communicationRelations.clear(); // trash existing list    
          TiXmlElement* pCommunicationRelationsNode=hRoot.FirstChild( "CommunicationRelations" ).FirstChild().Element();
          for( pCommunicationRelationsNode; pCommunicationRelationsNode; pCommunicationRelationsNode=pCommunicationRelationsNode->NextSiblingElement())
          {   
            RelationPersonDevices c; 
            const char* pId = pCommunicationRelationsNode->Attribute("id");
            if(pId) c.personId = pId;
            TiXmlElement* pDeviceNode=pCommunicationRelationsNode ->FirstChildElement();
            for( pDeviceNode; pDeviceNode; pDeviceNode=pDeviceNode->NextSiblingElement())
            {
              DeviceType dt;
              string aux;
              const char *pdevice=pDeviceNode->Attribute("device");
              if (pdevice) aux=pdevice;
              istringstream ss;
              ss.str(aux);
              ss >> dt;
              c.devices.push_back(dt);
            }                
            communicationRelations.push_back(c);
          }
        }
      // block: peopleSameLocation
        {
          peopleSameLocation.clear(); // trash existing list    
          TiXmlElement* pPeopleSameLocationNode=hRoot.FirstChild( "PeopleSameLocation" ).FirstChild().Element();
          for( pPeopleSameLocationNode; pPeopleSameLocationNode; pPeopleSameLocationNode=pPeopleSameLocationNode->NextSiblingElement())
          {
            string c;
            const char *pPerson=pPeopleSameLocationNode->Attribute("id");
            if (pPerson) c=pPerson;              
            peopleSameLocation.push_back(c);
          }
        }
      // block: msgBehaviour
        {
          msgBehaviour.clear(); // trash existing list    
          TiXmlElement* pMsgBehaviourNode=hRoot.FirstChild( "MsgBehaviour" ).FirstChild().Element();
          for( pMsgBehaviourNode; pMsgBehaviourNode; pMsgBehaviourNode=pMsgBehaviourNode->NextSiblingElement())
          {
            MsgReceivedBehaviour c;
            if (!pMsgBehaviourNode) return;
            const char* pFrom = pMsgBehaviourNode->Attribute("from");
            if(pFrom) c.from = pFrom;
            const char* pContent = pMsgBehaviourNode->Attribute("content");
            if(pContent) c.content = pContent;
            TiXmlElement* pMsgReceivedChild=pMsgBehaviourNode->FirstChildElement();
            for( pMsgReceivedChild; pMsgReceivedChild; pMsgReceivedChild=pMsgReceivedChild->NextSiblingElement())
            {
              if(pMsgReceivedChild->Value() == string("Msg2Send")){
                Msg2Send msg;
                const char *pTo=pMsgReceivedChild->Attribute("to");
                if (pTo) msg.to=pTo;
                const char *pContent=pMsgReceivedChild->Attribute("content");
                if (pContent) msg.content=pContent;
                const char* pCompulsory = pMsgReceivedChild->Attribute("compulsory");
                string aux;
                if(pCompulsory) aux = pCompulsory;
                (aux == string("true")) ? msg.compulsory = true : msg.compulsory = false;
                const char* pSend2Multiple = pMsgReceivedChild->Attribute("send2Multiple");
                if(pSend2Multiple) aux = pSend2Multiple;
                (aux == string("true")) ? msg.send2Multiple = true : msg.send2Multiple = false; 
                TiXmlElement* pMsg2SendNodeChild=pMsgReceivedChild->FirstChildElement();
                if(pMsg2SendNodeChild){
                  for(pMsg2SendNodeChild; pMsg2SendNodeChild; pMsg2SendNodeChild=pMsg2SendNodeChild->NextSiblingElement()){
                    if(pMsg2SendNodeChild->Value() == string("Device")){
                      DeviceType c;
                      string aux;
                      const char *pdevice=pMsg2SendNodeChild->Attribute("device");
                      if (pdevice) aux=pdevice;
                      istringstream ss;
                      ss.str(aux);
                      ss >> c;
                      msg.deviceCompulsory.push_back(c);
                    }else if(pMsg2SendNodeChild->Value() == string("Confirmation")){
                      Confirmation d;
                      const char *pFrom=pMsg2SendNodeChild->Attribute("from");
                      if (pFrom) d.from=pFrom;
                      const char *pContent=pMsg2SendNodeChild->Attribute("content");
                      if (pContent) d.content=pContent;
                      msg.confirmations.push_back(d);
                    }else{
                      assert(false && "Not that field in the xml file");
                    }                    
                  }
                }
                c.msg2send.push_back(msg);            
              }else if(pMsgReceivedChild->Value() == string("Action2Do")){
                string action;
                const char *pId=pMsgReceivedChild->Attribute("id");
                if (pId) action=pId;
                c.actions2Do.push_back(action);
              }else{
                assert(false && "Not that field in the xml file");
              }        
            }           
            msgBehaviour.push_back(c);
          }
        }
      // block: ActionBehaviour
        {
          actionBehaviour.clear(); // trash existing list    
          TiXmlElement* pActionBehaviour=hRoot.FirstChild( "ActionBehaviour" ).FirstChild().Element();
          for( pActionBehaviour; pActionBehaviour; pActionBehaviour=pActionBehaviour->NextSiblingElement())
          {
            ActionBehaviour c;
            if (!pActionBehaviour) return;
            const char* pID = pActionBehaviour->Attribute("id");
            if(pID) c.id = pID;
            TiXmlElement* pActionBehaviourChild=pActionBehaviour->FirstChildElement();
            for( pActionBehaviourChild; pActionBehaviourChild; pActionBehaviourChild=pActionBehaviourChild->NextSiblingElement())
            {
              if(pActionBehaviourChild->Value() == string("Msg2Send")){
                Msg2Send msg;
                const char *pTo=pActionBehaviourChild->Attribute("to");
                if (pTo) msg.to=pTo;
                const char *pContent=pActionBehaviourChild->Attribute("content");
                if (pContent) msg.content=pContent;
                const char* pCompulsory = pActionBehaviourChild->Attribute("compulsory");
                string aux;
                if(pCompulsory) aux = pCompulsory;
                (aux == string("true")) ? msg.compulsory = true : msg.compulsory = false;
                const char* pSend2Multiple = pActionBehaviourChild->Attribute("send2Multiple");
                if(pSend2Multiple) aux = pSend2Multiple;
                (aux == string("true")) ? msg.send2Multiple = true : msg.send2Multiple = false; 
                TiXmlElement* pMsg2SendNodeChild=pActionBehaviourChild->FirstChildElement();
                if(pMsg2SendNodeChild){
                  for(pMsg2SendNodeChild; pMsg2SendNodeChild; pMsg2SendNodeChild=pMsg2SendNodeChild->NextSiblingElement()){
                    if(pMsg2SendNodeChild->Value() == string("Device")){
                      DeviceType c;
                      string aux;
                      const char *pdevice=pMsg2SendNodeChild->Attribute("device");
                      if (pdevice) aux=pdevice;
                      istringstream ss;
                      ss.str(aux);
                      ss >> c;
                      msg.deviceCompulsory.push_back(c);
                    }else if(pMsg2SendNodeChild->Value() == string("Confirmation")){
                      Confirmation d;
                      const char *pFrom=pMsg2SendNodeChild->Attribute("from");
                      if (pFrom) d.from=pFrom;
                      const char *pContent=pMsg2SendNodeChild->Attribute("content");
                      if (pContent) d.content=pContent;
                      msg.confirmations.push_back(d);
                    }else{
                      assert(false && "Not that field in the xml file");
                    }                    
                  }
                }
                c.msg2send.push_back(msg);            
              }else if(pActionBehaviourChild->Value() == string("Location")){
                const char *pLocation=pActionBehaviourChild->GetText();
                if (pLocation) c.location=pLocation;
              }else if(pActionBehaviourChild->Value() == string("AverageExecutionTime")){
                const char *pTime=pActionBehaviourChild->Attribute("time");
                if (pTime) c.averageExectionTime=pTime;
              }else{
                assert(false && "Not that field in the xml file");
              }        
            }           
            actionBehaviour.push_back(c);
          }
        }        
    }
  };
}
#endif // CADMIUM_DECISION_MAKER_BEHAVIOUR_STRUCT_TYPES_HPP