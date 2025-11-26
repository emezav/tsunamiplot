/**
 * @author Erwin Meza Vega <emezav@gmail.com>, <emezav@unicauca.edu.co>
 * @brief Command execution utilities
 * @date 2025-11-16
 * @copyright Copyright (c) 2025
 */

#include <stdlib.h>
#include <string>
#include <array>
#include <iostream>

#include "cmd.h"

int executeCommand(std::string command, bool showOutput)
{
  FILE *execPipe = createPipe(command.c_str(), "r");

  if (!execPipe)
  {
    // Handle error
    return -1;
  }

  std::string result;
  std::array<char, 128> buffer; // A buffer to read output in chunks

  while (fgets(buffer.data(), buffer.size(), execPipe) != nullptr)
  {
    result += buffer.data();
  }

  int exitCode = closePipe(execPipe); // Close the pipe and get the exit status

  if (showOutput)
  {
    std::cout << result << std::endl;
  }

  if (exitCode == -1)
  {
    return -1;
  }
  return exitCode;
}
