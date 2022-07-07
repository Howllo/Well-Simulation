﻿/****************************************
*
*	File: WellHandler.cpp
*	Author: Tony A. Hardiman Jr.
*	Purpose: Handles all the functionality for the class WellHandler.h.
*	Assignment: Programming Assignment 1
*	Declaration: This program is entirely my own work.
*
****************************************/

#include "WellHandler.h"
#include "OilFieldDataParser.h"
#include <iostream>
#include <string>
#include <chrono>

WellHandler::WellHandler()
{
    char TestArray[] = "I:/CS-307/Program/ProgramAssignment1/OilFieldData.xml";
    //std::cout << "Enter file location: ";
    //std::cin >> fileName;
    
    dataParserXML = new OilFieldDataParser(const_cast<const char*>(TestArray));
    displayClass = new DisplayClass();
    sensorReader = new SensorReader();
    totalActiveWell = 0;
    
    // Creates the Wells
    for(int i = 0; i < dataParserXML->getWellCount(); i++)
    {
        CreateWell();
    }
    
    // Delay until wells are in place.
    inputWindow = new InputWindow();
    WellClass* activeTemp = m_pActiveWellHead;
    while(activeTemp != nullptr)
    {
        totalActiveWell++;
        activeTemp = activeTemp->m_pNext;
    }
    
    // Delete Data Parser
    delete dataParserXML;
}

WellHandler::~WellHandler()
{
    delete displayClass;
    delete sensorReader;
    delete inputWindow;
}

void WellHandler::Update()
{
    bool bProgramStillRunning = true;
    int wellCount = 0;
    
    while(true)
    {
        std::cout << "How many well(s) you want to add?" << std::endl;
        std::cin >> wellCount;
        if(wellCount > 0 && wellCount < totalActiveWell)
        {
            break;
        }
    }
    
    std::cout << std::endl;
    for(int i = 0; i < wellCount; i++)
    {
        char userChoice[8];
        inputWindow->GetWell(m_pActiveWellHead);
        std::cout << "Select Well: ";
        std::cin >> userChoice;
        std::cout << std::endl;
        AddSelectedWell(userChoice);
    }

    //Main Loop
   while(bProgramStillRunning != false)
   {
       auto currentTime = std::chrono::system_clock::now();

       // Print Data.
       sensorReader->PrintSensorData(m_pSelectedWellHead);

       // User Input2
       if(currentTime >= inputWindow->GetNextTime())
       {
           std::cout << std::endl;
           AddSelectedWell(inputWindow->SelectWell(m_pActiveWellHead).get());
           RemoveSelectedWell(inputWindow->RemoveSelectedWell(m_pSelectedWellHead).get());
           Select_Sensor(inputWindow->SensorSettings().get());
           if(inputWindow->CheckTime())
           {
               bProgramStillRunning = false;
           }
           std::cout << std::endl;
       }
   }
}

bool WellHandler::CreateWell()
{
    WellClass* createWell = new WellClass(dataParserXML);
    WellClass* temp = m_pActiveWellHead;

    if(m_pActiveWellHead == nullptr)
    {
        m_pActiveWellHead = createWell;
        dataParserXML->getWellData(createWell->well_ID, createWell->wellOperator, &createWell->numberSensor);
        return true;
    }

    while (temp != nullptr)
    {
        if(temp->m_pNext == nullptr)
        {
            temp->m_pNext = createWell;
            dataParserXML->getWellData(createWell->well_ID, createWell->wellOperator, &createWell->numberSensor);
            return true;
        }
        temp = temp->m_pNext;
    }
    return false;
}

bool WellHandler::DeleteWell(char* Well_ID)
{
    if(Well_ID == nullptr) return  false;
    
    WellClass* temp = m_pActiveWellHead;
    WellClass* back = nullptr;
    
    while (temp != nullptr)
    {   
        if(strcmp(temp->well_ID, Well_ID) == 0)
        {
            if(back != nullptr)
            {
                back->m_pNext = temp->m_pNext;
                temp->m_pNext = nullptr;
                return true;
            }
            m_pActiveWellHead = temp->m_pNext;
            temp->m_pNext = nullptr;
            delete temp;
            return true;
        }
        back = temp;
        temp = temp->m_pNext;
    }
    return false;
}

bool WellHandler::AddActiveWell(WellClass* Well)
{
    WellClass* temp = m_pActiveWellHead;

    while(temp != nullptr)
    {
        if(temp->m_pNext == nullptr)
        {
            temp->m_pNext = Well;
            return true;
        }
        temp = temp->m_pNext;
    }
    return false;
}

bool WellHandler::RemoveActiveWell(WellClass* Well)
{
    WellClass* temp = m_pActiveWellHead;

    if(strcmp(m_pActiveWellHead->well_ID, Well->well_ID) == 0)
    {
        m_pActiveWellHead = m_pActiveWellHead->m_pNext;
        Well->m_pNext = nullptr;
        return true;
    }
    
    while(temp != nullptr)
    {
        if(strcmp(temp->m_pNext->well_ID, Well->well_ID) == 0)
        {
            temp->m_pNext = Well->m_pNext;
            Well->m_pNext = nullptr;
            return true;
        }
        temp = temp->m_pNext;
    }
    return false;
}

bool WellHandler::AddSelectedWell(char* Well_ID)
{
    if(Well_ID == nullptr) return false;
    
    WellClass* temp = m_pActiveWellHead;
    WellClass* temp2 = m_pSelectedWellHead;
    
    while(temp != nullptr)
    {
        if(strcmp(Well_ID, temp->well_ID) == 0)
        {
            if(temp->isSelect == true)
                return false;

            if(!m_pSelectedWellHead)
            {
                RemoveActiveWell(temp);
                m_pSelectedWellHead = temp;
                temp->isSelect = true;
                sensorReader->SelectSensor(temp, sensor_add);
                return true;
            }
            
            while(temp2 != nullptr)
            {
                if(temp2->m_pNext == nullptr)
                {
                    RemoveActiveWell(temp);
                    temp2->m_pNext = temp;
                    temp->isSelect = true;
                    sensorReader->SelectSensor(temp, sensor_add);
                    return true;
                }
                temp2 = temp2->m_pNext;
            }
        }
        temp = temp->m_pNext;
    }
    
    return false;
}

bool WellHandler::RemoveSelectedWell(char* Well_ID)
{
    if(Well_ID == nullptr) return false;
    
    WellClass* temp = m_pSelectedWellHead;
    WellClass* back = nullptr;
    
    while(temp != nullptr)
    {
        if(strcmp(Well_ID, temp->well_ID) == 0)
        {
            if(back != nullptr)
            {
                back->m_pNext = temp->m_pNext;
                temp->m_pNext = nullptr;
                AddActiveWell(temp);
                return true;
            }
            m_pActiveWellHead = temp->m_pNext;
            temp->m_pNext = nullptr;
            return true;
        }
        back = temp;
        temp = temp->m_pNext;
    }
    return false;
}

void WellHandler::Select_Sensor(char* Well_ID)
{
    if(Well_ID == nullptr) return;
    
    WellClass* temp = m_pSelectedWellHead;

    while(temp != nullptr)
    {
        if(temp->well_ID == Well_ID)
        {
            char userSelection = ' ';
            std::cout << "What do want to do to sensors? (A for Add | R for Remove)" << std::endl;
            std::cin >> userSelection;
            if(userSelection == 'A' || userSelection == 'a')
            {
                sensorReader->SelectSensor(temp, sensor_add);
            }
            else if(userSelection == 'R' || userSelection == 'r')
            {
                sensorReader->SelectSensor(temp, sensor_remove);
            }
        }
        
        temp = temp->m_pNext;
    }
}