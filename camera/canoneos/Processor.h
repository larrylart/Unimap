/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : Processor.h	                                                  *
*                                                                             *
*   Description: This is the Sample code to show the usage of EDSDK.          *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*   Written and developed by Camera Design Dept.53                            *
*   Copyright Canon Inc. 2006-2008 All Rights Reserved                        *
*                                                                             *
*******************************************************************************/


#pragma once


#include <deque>
//#include "Thread.h"
//#include "Synchronized.h"
//#include "Command.h"

class Processor : public Thread 
{

protected:
    // Whether it is executing it or not?
	bool _running;
	// Que
	std::deque<Command*>  _queue;

	// Command when ending
	Command*	_closeCommand;
		
	// Synchronized Object
    Synchronized _syncObject;


public:
	// Constructor  
	Processor(): _running(false), _closeCommand(0){ }	

	// Destoracta
	virtual ~Processor(){clear();}

	// Set command when ending
	void setCloseCommand(Command* closeCommand){_closeCommand = closeCommand;}


	/*
	void enqueue(Command* command)
	{
		_syncObject.lock();
		_queue.push_back(command);
		_syncObject.unlock();
		resume();	
	}*/

	
	void enqueue(Command* command)
	{
		_syncObject.lock();
		_queue.push_back(command);
		_syncObject.notify();	
		_syncObject.unlock();
	}



	void stop()
	{
		_syncObject.lock();
		_running = false;
		_syncObject.unlock();
		//resume();
	}  


	void clear() 
	{
		_syncObject.lock();

		std::deque<Command*>::iterator it = _queue.begin();
		while (it != _queue.end())
		{
			delete (*it);
			++it;
		}
		_queue.clear();

		_syncObject.unlock();
	}


public:
	virtual void run()
	{
		//When using the SDK from another thread in Windows, 
		// you must initialize the COM library by calling CoInitialize 
		CoInitializeEx( NULL, COINIT_MULTITHREADED );

		_running = true;
		while (_running)
		{
			Sleep(1);

			Command* command = take();
			if(command != NULL)
			{
				bool complete = command->execute();
				
				if(complete == false)
				{
					//If commands that were issued fail ( because of DeviceBusy or other reasons )
					// and retry is required , note that some cameras may become unstable if multiple 
					// commands are issued in succession without an intervening interval.
					//Thus, leave an interval of about 500 ms before commands are reissued.
					Sleep(500);
					enqueue(command);
				}
				else
				{
					delete command;
				}
			}
		}
		
		// Clear que
		clear();

		// Command of end
		if(_closeCommand != NULL)
		{
			_closeCommand->execute();
			delete _closeCommand;
			_closeCommand = NULL;
		}

		CoUninitialize();

	}

protected:

	//The command is taken out of the que

	/*
	Command* take()
	{
	
		Command* command = NULL;
		
		// Que stands by between emptiness.
		while (true)
		{
			_syncObject.lock();
			bool empty = _queue.empty();
			_syncObject.unlock();
			
			if(empty == false)break;

			suspend();

			if(_running == false)
			{
				return NULL;
			}
		}
	
		_syncObject.lock();
		
		command = _queue.front();
		_queue.pop_front();

		_syncObject.unlock();
		
		return command;
	}*/

	
 	Command* take()
	{
	
		Command* command = NULL;
	
		_syncObject.lock();

		// Que stands by between emptiness.
		while (_queue.empty() && _running)
		{
			_syncObject.wait(10);
		}
	
		if (_running)
		{
			command = _queue.front();
			_queue.pop_front();
		}

		_syncObject.unlock();

		return command;
	}
 

	bool isEmpty()
	{
		_syncObject.lock();
		bool ret = _queue.empty();
		_syncObject.unlock();
		
		return ret;
	}

};






