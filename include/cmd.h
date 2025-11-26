/**
 * @author Erwin Meza Vega <emezav@gmail.com>, <emezav@unicauca.edu.co>
 * @brief Command execution utilities
 * @date 2025-11-16
 * @copyright Copyright (c) 2025
 */

#ifndef CMD_H
#define CMD_H

#include <string>

#ifdef WIN32
#define createPipe _popen
#define closePipe _pclose
#elif __linux__
#define createPipe popen
#define closePipe pclose
#endif

/**
 * @brief Execute a shell command and capture its output
 *
 * @param command The command to execute
 * @param showOutput Whether to display the command output
 * @return int The exit code of the command
 */
int executeCommand(std::string command, bool showOutput = false);

#endif