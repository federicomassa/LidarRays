// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <exception>
#include <string>

namespace Race {
	class UninitializedException : public std::exception
	{
		const char* var_name;
	public:
		UninitializedException(const char* v_name) : var_name(v_name) {};
		const char* what() const throw()
		{
			return (std::string("Variable ") + var_name + " was not initialized").c_str();
		}

	};

	class UnregisteredContestantException : public std::exception
	{
		const char* contestant_id;
	public:
		UnregisteredContestantException(const char* id) : contestant_id(id) {};
		const char* what() const throw()
		{
			return (std::string("Agent ") + contestant_id + " was not registered to RaceControl").c_str();
		}

	};

	class InexistentStateException : public std::exception
	{
		const char* var_name;
	public:
		InexistentStateException(const char* v_name) : var_name(v_name) {};
		const char* what() const throw()
		{
			return (std::string("State with name ") + var_name + " does not exist").c_str();
		}

	};

	class TerminatedActionException : public std::exception
	{
		const char* action_info;
	public:
		TerminatedActionException(const char* info) : action_info(info) {};
		const char* what() const throw()
		{
			return (std::string("Called listen on already terminated action with info: ") + action_info).c_str();
		}
	};

	class LocalMonitorFatalException : public std::exception
	{
		const char* description;
	public:
		LocalMonitorFatalException(const char* str) : description(str) {};
		const char* what() const throw()
		{
			return description;
		}
	};

	class LocalMonitorWarningException : public std::exception
	{
		const char* description;
	public:
		LocalMonitorWarningException(const char* str) : description(str) {};
		const char* what() const throw()
		{
			return description;
		}
	};

	class LocalMonitorInfoException : public std::exception
	{
		const char* description;
	public:
		LocalMonitorInfoException(const char* str) : description(str) {};
		const char* what() const throw()
		{
			return description;
		}
	};
}
